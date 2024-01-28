#include "BrowserClient.hpp"
#include "json.hpp"
#include <string>
#include <sstream>

using json = nlohmann::json;

BrowserClient::BrowserClient(CefRefPtr<OpenAI> openai,
			     CefRefPtr<CefRenderHandler> ptr,
			     CefRefPtr<CefLoadHandler> loadptr,
			     CefRefPtr<CefDisplayHandler> displayptr,
			     CefRefPtr<CefLifeSpanHandler> lifespanptr)
  : m_openai(openai),
    m_renderHandler(ptr),
    m_loadHandler(loadptr),
    m_displayHandler(displayptr),
    m_lifeSpanHandler(lifespanptr)
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
    return m_displayHandler;
}
CefRefPtr<CefLifeSpanHandler> BrowserClient::GetLifeSpanHandler()
{
    return m_lifeSpanHandler;
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
bool BrowserClient::OnProcessMessageReceived( CefRefPtr<CefBrowser> browser,
					      CefRefPtr<CefFrame> frame,
					      CefProcessId source_process,
					      CefRefPtr<CefProcessMessage> message )
{
    LOG(INFO)<<"BrowserClient handle message:"<<message->GetName()<<std::endl;
    CefRefPtr<CefListValue> args = message->GetArgumentList();
    if(message->GetName() == "HtmlContent"){

	json eleArr = json::parse(args->GetString(0).ToString());
	
	json dict;
	dict["context"] = CalEleDesc(eleArr);
	
	std::string prompt = m_openai->prompt_template("script.prom",dict);
	//m_openai->chatCompletion(prompt, base::BindOnce(&BrowserView::UserCompletionCallback, browser));
	m_openai->chatCompletionAzure(prompt, base::BindOnce(&BrowserClient::ScriptCompletionCallback, this, browser));
    }
    return true;
}
void BrowserClient::ScriptCompletionCallback(CefRefPtr<CefBrowser> browser, const json& msg)
{
    std::string jscode = msg["choices"][0]["message"]["content"].get<std::string>();
		
    CefRefPtr<CefProcessMessage> jsmsg = CefProcessMessage::Create("JSCode");
    CefRefPtr<CefListValue> args = jsmsg->GetArgumentList();
    args->SetString(0, jscode);
    browser->GetMainFrame()->SendProcessMessage(PID_RENDERER, jsmsg);
}
