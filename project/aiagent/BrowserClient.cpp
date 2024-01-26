#include "BrowserClient.hpp"
#include "json.hpp"
#include <string>
#include <sstream>

using json = nlohmann::json;

BrowserClient::BrowserClient(CefRefPtr<CefRenderHandler> ptr,
			     CefRefPtr<CefLoadHandler> loadptr,
			     CefRefPtr<CefDisplayHandler> displayptr)
  : m_renderHandler(ptr),
    m_loadHandler(loadptr),
    m_displayHandler(displayptr)
{
    //m_openai = new OpenAI("http://192.168.8.37:8000/", "EMPTY", "Qwen-14B-Chat","v1");
    m_openai = new OpenAI("https://kingsware3.openai.azure.com/openai/deployments/",
			  "3d93f1f773414565813b79e1767000ef",
			  "kingsgpt35_16k",
			  "2023-07-01-preview");
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
    return m_displayHandler;
}
std::string BrowserClient::CalEleDesc(json &data){
    std::stringstream ssprompt;

    for(json::iterator it = data.begin(); it != data.end(); ++it)
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
    return ssprompt.str();
}
bool BrowserClient::OnProcessMessageReceived( CefRefPtr< CefBrowser > browser,
					      CefRefPtr< CefFrame > frame,
					      CefProcessId source_process,
					      CefRefPtr< CefProcessMessage > message )
{
    CefRefPtr<CefListValue> args = message->GetArgumentList();

    json eleArr = json::parse(args->GetString(0).ToString());

    json dict;
    dict["context"] = CalEleDesc(eleArr);

    std::string prompt = m_openai->prompt_template("script.prom",dict);
    
    m_openai->setFrame(frame);
    //m_openai->chatCompletion(prompt);
    m_openai->chatCompletionAzure(prompt);
    return true;
}
