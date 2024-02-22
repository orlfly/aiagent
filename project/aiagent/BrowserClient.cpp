#include "BrowserClient.hpp"
#include "BrowserView.hpp"
#include "ApiServer.hpp"
#include "json.hpp"
#include <string>
#include <sstream>
#include <regex>
#include "include/cef_command_line.h"

using json = nlohmann::json;

BrowserClient::BrowserClient(CefRefPtr<OpenAI> openai,
			     CefRefPtr<BrowserView> browser,
			     CefRefPtr<CefRenderHandler> ptr,
			     CefRefPtr<LoadHandler> loadptr,
			     CefRefPtr<CefLifeSpanHandler> lifespanptr)
  : m_openai(openai),
    m_browser(browser),
    m_renderHandler(ptr),
    m_loadHandler(loadptr),
    m_lifeSpanHandler(lifespanptr),
    m_currentStep("无"),
    m_currentProblem("")
{
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

bool BrowserClient::OnProcessMessageReceived( CefRefPtr<CefBrowser> browser,
					      CefRefPtr<CefFrame> frame,
					      CefProcessId source_process,
					      CefRefPtr<CefProcessMessage> message )
{
    LOG(INFO)<<"BrowserClient handle message:"<<message->GetName()<<std::endl;
    CefRefPtr<CefListValue> args = message->GetArgumentList();
    //LOG(INFO) << args->GetString(0).ToString() << std::endl;
    CefRefPtr<CefCommandLine> command_line =
      CefCommandLine::GetGlobalCommandLine();
    std::string name = command_line->GetSwitchValue("name");
    if(name.length()==0)
    {
	name = "金大能";
    }
    if(message->GetName() == "ExecutFinish"){

        std::string problem = args->GetString(0).ToString();
	std::string site = args->GetString(1).ToString();
        std::string content = args->GetString(2).ToString();
	json dict;
	dict["name"] = name;
	dict["site"] = site;
	dict["problem"] = problem;
	dict["content"] = content;
	dict["step"] =  m_currentStep;
	m_currentProblem = problem;
	
	std::string prompt = m_browser->GetOpenAI()->prompt_template("task.prom",dict);
	LOG(INFO)<<prompt<<std::endl;
	
	m_browser->GetOpenAI()->chatCompletionAzure(prompt, base::BindOnce(&BrowserClient::TaskCompletionCallback, this, browser, content));
    }else if(message->GetName() == "EvaluateFinish"){
        std::string step = args->GetString(0).ToString();
	std::string site = args->GetString(1).ToString();
	std::string content = args->GetString(2).ToString();
	json dict;
	dict["name"] = name;
	dict["problem"] = m_currentProblem;
	dict["content"] = content;
	dict["step"] =  step;
	
	std::string prompt = m_browser->GetOpenAI()->prompt_template("evaluate.prom",dict);
	LOG(INFO)<<prompt<<std::endl;
	
	m_browser->GetOpenAI()->chatCompletionAzure(prompt, base::BindOnce(&BrowserClient::EvaluateCompletionCallback,
									   this,
									   browser,
									   site,
									   m_currentProblem,
									   step,
									   content));
    }else if(message->GetName() == "ContentLoaded"){
        LOG(INFO) << "current step:" << m_currentStep << std::endl;
        if(m_currentStep!="无"){
	    std::stringstream cmd;
	    cmd<<"aiagent.EvaluateContent('"<<m_currentStep<<"');"<<std::endl;
	    frame->ExecuteJavaScript(cmd.str(), frame->GetURL(), 0);
	}
    }else if(message->GetName() == "Timeout"){
        std::string cmd = "aiagent.visualBox();";
	frame->ExecuteJavaScript(cmd, frame->GetURL(), 0);
    }else if(message->GetName() == "Summary"){
        std::string content = args->GetString(0).ToString();
	std::string task = args->GetString(1).ToString();
	json dict;
	dict["name"] = name;
	dict["text"] = content;
	dict["task"] = task;
	std::string prompt = m_openai->prompt_template("summary.prom",dict);
	LOG(INFO) << prompt << std::endl;
	
	m_openai->chatCompletionAzure(prompt, base::BindOnce(&BrowserClient::SummaryCompletionCallback, this, browser));
    }
    return true;
}
void BrowserClient::TaskCompletionCallback(CefRefPtr<CefBrowser> browser, const std::string content, const json& msg)
{
    std::string nextstep = msg["choices"][0]["message"]["content"].get<std::string>();
    LOG(INFO) << "next step:" << nextstep << std::endl;
    std::stringstream ss(nextstep);
    CefRefPtr<CefCommandLine> command_line =
      CefCommandLine::GetGlobalCommandLine();
    std::string name = command_line->GetSwitchValue("name");
    std::string line;
    std::string step="";
    while(std::getline(ss, line))
    {
        if(line.size()>0){
  	    std::size_t found = line.find("Plan:");
	    if(found==std::string::npos){
  	        step = line;
		break;
	    }
	}
    }
    step = std::regex_replace(step,std::regex("[0-9]+\\."),"");
    step = std::regex_replace(step,std::regex("'"),"\"");
    LOG(INFO) << "step:" << step << std::endl;
    if(name.length()==0)
    {
        name = "金大能";
    }
    json dict;
    dict["name"] = name;
    dict["context"] = content;
    dict["step"] = step;
    m_currentStep = step;
    std::string prompt = m_openai->prompt_template("script.prom",dict);
    LOG(INFO) << prompt << std::endl;
    
    m_openai->chatCompletionAzure(prompt, base::BindOnce(&BrowserClient::ScriptCompletionCallback, this, browser));
}
void BrowserClient::ScriptCompletionCallback(CefRefPtr<CefBrowser> browser, const json& msg)
{
    std::string jscode = msg["choices"][0]["message"]["content"].get<std::string>();
    LOG(INFO) << "JS Code:" << jscode << std::endl;
    CefRefPtr<CefProcessMessage> jsmsg = CefProcessMessage::Create("JSCode");
    CefRefPtr<CefListValue> args = jsmsg->GetArgumentList();
    args->SetString(0, jscode);
    browser->GetMainFrame()->SendProcessMessage(PID_RENDERER, jsmsg);
}

void BrowserClient::EvaluateCompletionCallback(CefRefPtr<CefBrowser> browser,
					       const std::string site,
					       const std::string problem,
					       const std::string step,
					       const std::string content,
					       const json& msg)
{
    CefRefPtr<CefCommandLine> command_line =
      CefCommandLine::GetGlobalCommandLine();
    std::string name = command_line->GetSwitchValue("name");
    if(name.length()==0)
    {
        name = "金大能";
    }
    std::string result = msg["choices"][0]["message"]["content"].get<std::string>();
    LOG(INFO) << "diff result:" << result << std::endl;
    
    std::size_t ffound = result.rfind(":");
    
    result = result.substr(ffound+1);
    
    if(result=="正确")
    {
	json dict;
	dict["name"] = name;
	dict["site"] = site;
	dict["problem"] = problem;
	dict["content"] = content;
	dict["step"] =  step;
	
	std::string prompt = m_browser->GetOpenAI()->prompt_template("task.prom",dict);
	LOG(INFO)<<prompt<<std::endl;

	m_browser->GetOpenAI()->chatCompletionAzure(prompt, base::BindOnce(&BrowserClient::TaskCompletionCallback, this, browser, content));
    }else{
        json dict;
	dict["name"] = name;
	dict["site"] = site;
	dict["problem"] = problem;
	dict["content"] = content;
	dict["step"] =  "无";
	m_currentStep = "无";
	std::string prompt = m_browser->GetOpenAI()->prompt_template("task.prom",dict);
	
	LOG(INFO)<<prompt<<std::endl;

	m_browser->GetOpenAI()->chatCompletionAzure(prompt, base::BindOnce(&BrowserClient::TaskCompletionCallback, this, browser, content));
    }
}

void BrowserClient::SummaryCompletionCallback(CefRefPtr<CefBrowser> browser, const json& msg)
{
    std::string result = msg["choices"][0]["message"]["content"].get<std::string>();
    LOG(INFO) << "diff result:" << result << std::endl;
    m_browser->OnExecutFinish(m_currentStep+"执行"+result);
}
