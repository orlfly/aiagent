#include "BrowserClient.hpp"
#include "json.hpp"
#include <string>
#include <sstream>

using json = nlohmann::json;

BrowserClient::BrowserClient(CefRefPtr<CefRenderHandler> ptr,
	      CefRefPtr<CefLoadHandler> loadptr)
    : m_renderHandler(ptr),
    m_loadHandler(loadptr)
{
    m_openai = new OpenAI("http://192.168.8.37:8000/", "EMPTY", "v1");
    //m_openai = new OpenAI("https://kingsware3.openai.azure.com/openai/deployments/", "3d93f1f773414565813b79e1767000ef", "2023-07-01-preview");
}
CefRefPtr<CefRenderHandler> BrowserClient::GetRenderHandler()
{
    return m_renderHandler;
}
CefRefPtr<CefLoadHandler> BrowserClient::GetLoadHandler()
{
    return m_loadHandler;
}
bool BrowserClient::OnProcessMessageReceived( CefRefPtr< CefBrowser > browser,
					      CefRefPtr< CefFrame > frame,
					      CefProcessId source_process,
					      CefRefPtr< CefProcessMessage > message )
{
    CefRefPtr<CefListValue> args = message->GetArgumentList();

    json eleArr = json::parse(args->GetString(0).ToString());
    
    std::stringstream ssprompt;

    ssprompt << "下面是HTML的页面元素信息：" <<std::endl;
    for(json::iterator it = eleArr.begin(); it != eleArr.end(); ++it)
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
        ssprompt << "Xpath:'" << Xpath <<"' ";
	if (text.length() > 0)
	  ssprompt << "文本信息:'" << text <<"' ";
	else if(value.length()>0)
	  ssprompt << "文本信息:'" << value <<"' ";
        ssprompt << std::endl;
    }
    ssprompt << "基于HTML的页面元素信息生成搜索'淘宝'的javascript页面操作脚本。仅输出javascript脚本,去除备注和注释信息。" <<std::endl;
    
    json data;
    data["model"] = "Qwen-14B-Chat";
    //data["model"] = "kingsgpt35_16k";
    data["messages"][0]["role"] = "user";
    data["messages"][0]["content"] = ssprompt.str();
    data["max_tokens"] = 2000;
    data["temperature"] = 0;

    std::cout << data.dump() << std::endl;
    
    m_openai->setFrame(frame);
    m_openai->completion(data);
    //m_openai->completionAzure(data);
    return true;
}
