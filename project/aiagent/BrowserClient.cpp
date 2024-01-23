#include "Browserclient.hpp"

BrowserClient::BrowserClient(CefRefPtr<CefRenderHandler> ptr,
	      CefRefPtr<CefLoadHandler> loadptr)
    : m_renderHandler(ptr),
    m_loadHandler(loadptr)
{
    openai::start("http://192.168.8.37:8000/v1/", "EMPTY");
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
    std::cout<< args->GetString(0) <<std::endl;
    auto completion = openai::completion().create(R"({
            "model": "Qwen-14B-Chat",
            "prompt": "Say this is a test",
            "max_tokens": 7,
            "temperature": 0
        })"_json);
    std::cout << "Response is:\n" << completion.dump(2) << '\n';
    return true;
}
