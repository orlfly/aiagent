#include "BrowserClient.hpp"
#include "json.hpp"
#include <string>
#include <sstream>
#include "include/cef_command_line.h"

using json = nlohmann::json;

BrowserClient::BrowserClient(CefRefPtr<OpenAI> openai,
			     std::shared_ptr<std::queue<std::shared_ptr<BrowserTask>>> taskQue,
			     CefRefPtr<CefRenderHandler> ptr,
			     CefRefPtr<LoadHandler> loadptr,
			     CefRefPtr<CefLifeSpanHandler> lifespanptr)
  : m_openai(openai),
    m_renderHandler(ptr),
    m_loadHandler(loadptr),
    m_lifeSpanHandler(lifespanptr),
    m_currentOp("")
{
    m_taskQue=taskQue;
}
CefRefPtr<CefRenderHandler> BrowserClient::GetRenderHandler()
{
    return m_renderHandler;
}
CefRefPtr<CefLoadHandler> BrowserClient::GetLoadHandler()
{
    return m_loadHandler;
}
CefRefPtr<CefDisplayHandler> BrowserClient::GetDisplayHandler()
{
    return m_loadHandler;
}
CefRefPtr<CefLifeSpanHandler> BrowserClient::GetLifeSpanHandler()
{
    return m_lifeSpanHandler;
}
std::string BrowserClient::CalEleDesc(json &data){
    std::stringstream ssprompt;
    int count = 0;
    for(json::iterator it = data.begin(); it != data.end() && count < 60; ++it)
    {
        std::string tag = (*it)["tag"].template get<std::string>();
	std::string Xpath = (*it)["xpath"].template get<std::string>();
	std::string text = (*it)["text"].template get<std::string>();
	std::string selector = (*it)["selector"].template get<std::string>();
	std::string value = "";
	if (it->contains("value")){
	    value = (*it)["value"].template get<std::string>();
	}
	
        ssprompt << "HTML标签:'" << tag <<"' ";
        ssprompt << "selector:'" << selector <<"' ";
	if (text.length() > 0)
	    ssprompt << "文本信息:'" << text <<"' ";
	else if(value.length()>0)
	    ssprompt << "文本信息:'" << value <<"' ";
        ssprompt << std::endl;
	count++;
    }
    std::string retStr = ssprompt.str();
    if(retStr.length()==0)
    {
        retStr = "无";
    }
    return retStr;
}
bool BrowserClient::OnProcessMessageReceived( CefRefPtr<CefBrowser> browser,
					      CefRefPtr<CefFrame> frame,
					      CefProcessId source_process,
					      CefRefPtr<CefProcessMessage> message )
{
    LOG(INFO)<<"BrowserClient handle message:"<<message->GetName()<<std::endl;
    CefRefPtr<CefListValue> args = message->GetArgumentList();
    //LOG(INFO) << args->GetString(0).ToString() << std::endl;
    if(message->GetName() == "HtmlContent"){
        CefRefPtr<CefCommandLine> command_line =
	  CefCommandLine::GetGlobalCommandLine();
	std::string name = command_line->GetSwitchValue("name");
	if(name.length()==0)
	{
	    name = "金大能";
	}
	
        json eleArr = json::parse(args->GetString(0).ToString());
	std::string content = CalEleDesc(eleArr);
	if(content == "无")
	{
	  DoTask(browser,content);	    
	}
	else{
	    if(m_currentOp.length()>0)
	    {
	        json dict;
		dict["name"] = name;
		dict["operation"] = m_currentOp;
		dict["page"] = content;
		m_currentOp = "";
		std::string prompt = m_openai->prompt_template("result.prom",dict);
		LOG(INFO) << prompt << std::endl;
		m_openai->chatCompletionAzure(prompt, base::BindOnce(&BrowserClient::ResultCompletionCallback, this, browser, content));
	    }else{
	        DoTask(browser,content);
	    }
	}
    }else if(message->GetName() == "Timeout"){
        int tid = args->GetInt(0);
	LOG(INFO)<<"call timeout nexttask:" <<tid<<" queue:"<<m_taskQue->empty()<<std::endl;
	if(tid > 0 && !m_taskQue->empty())
	{
	    std::shared_ptr<BrowserTask> nexttask = m_taskQue->front();
	    if(tid == nexttask->m_tid)
	    {
	      std::string cmd = "aiagent.visualBox();";
	      frame->ExecuteJavaScript(cmd, frame->GetURL(), 0);
	    }
	}
    }
    return true;
}

void BrowserClient::ScriptCompletionCallback(CefRefPtr<CefBrowser> browser, int nexttask, const json& msg)
{
    std::string jscode = msg["choices"][0]["message"]["content"].get<std::string>();
    LOG(INFO) << "JS Code:" << jscode << std::endl;
    CefRefPtr<CefProcessMessage> jsmsg = CefProcessMessage::Create("JSCode");
    CefRefPtr<CefListValue> args = jsmsg->GetArgumentList();
    args->SetString(0, jscode);
    args->SetInt(1, nexttask);
    browser->GetMainFrame()->SendProcessMessage(PID_RENDERER, jsmsg);
}
void BrowserClient::DoTask(CefRefPtr<CefBrowser> browser, std::string content)
{
    LOG(INFO) << "Do task-----------------------" << std::endl;
    if(!m_taskQue->empty()){
        std::shared_ptr<BrowserTask> task = m_taskQue->front();
	m_taskQue->pop();
        int tid = -1;
	if(!m_taskQue->empty())
	{
	    std::shared_ptr<BrowserTask> nexttask = m_taskQue->front();
	    tid = nexttask->m_tid;
	}

	CefRefPtr<CefCommandLine> command_line =
	  CefCommandLine::GetGlobalCommandLine();
	std::string name = command_line->GetSwitchValue("name");
	if(name.length()==0)
	{
	    name = "金大能";
	}
	json dict;    
	dict["name"] = name;
	dict["context"] = content;
	dict["task"] = task->m_taskDesc;
	m_currentOp = task->m_taskDesc;
	std::string prompt = m_openai->prompt_template("script.prom",dict);
	LOG(INFO) << prompt << std::endl;
	m_taskMap.insert(std::make_pair(task->m_tid,task));
	json resp;
	resp["msg"] = "执行"+task->m_taskDesc+"....";
	std::move(task->m_callback).Run(resp);
	LOG(INFO) << "task queue empty:"<< m_taskQue->empty() << std::endl;
	//m_openai->chatCompletion(prompt, base::BindOnce(&BrowserView::ScriptCompletioncallback, browser));
	m_openai->chatCompletionAzure(prompt, base::BindOnce(&BrowserClient::ScriptCompletionCallback, this, browser, tid));
	
    }
}
void BrowserClient::ResultCompletionCallback(CefRefPtr<CefBrowser> browser, std::string content, const json& msg)
{
    std::string result = msg["choices"][0]["message"]["content"].get<std::string>();
    LOG(INFO) << "diff result:" << result << std::endl;
    
    std::size_t ffound = result.rfind(":");
    
    result = result.substr(ffound+1);
    
    if(result=="正确")
    {
        DoTask(browser, content);
    }else{
        std::string cmd = "aiagent.visualBox();";
	browser->GetMainFrame()->ExecuteJavaScript(cmd, browser->GetMainFrame()->GetURL(), 0);
    }
}
