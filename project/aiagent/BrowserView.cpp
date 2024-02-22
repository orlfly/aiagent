// This code is a modification of the original projects that can be found at
// https://github.com/if1live/cef-gl-example
// https://github.com/andmcgregor/cefgui

#include "BrowserView.hpp"
#include "GLCore.hpp"
#include "include/wrapper/cef_helpers.h"
#include "util.h"
#include "include/base/cef_logging.h"
#include "json.hpp"
#include <fstream>
using json = nlohmann::json;

std::string FileToString(std::string wp, std::string path) {
    std::ifstream inputFile(wp+path);
    if (!inputFile.is_open()) {
        LOG(ERROR) << "Error opening the file!" << std::endl;
        return ""; // Exit with an error code
    }
    
    std::ostringstream ss;
    std::string fileContent;
    std::string line;

    while (std::getline(inputFile, line)) {
        ss << line << "\n";
    }

    std::string file_contents = ss.str();
    inputFile.close();

    return file_contents;
}


bool BrowserView::viewport(float x, float y, float w, float h)
{
    if (!(x >= 0.0f) && (x < 1.0f))
        return false;

    if (!(x >= 0.0f) && (y < 1.0f))
        return false;

    if (!(w > 0.0f) && (w <= 1.0f))
        return false;

    if (!(h > 0.0f) && (h <= 1.0f))
        return false;

    if (x + w > 1.0f)
        return false;

    if (y + h > 1.0f)
        return false;

    m_viewport[0] = x;
    m_viewport[1] = y;
    m_viewport[2] = w;
    m_viewport[3] = h;

    return true;
}


//------------------------------------------------------------------------------
BrowserView::BrowserView(float scale)
  : m_viewport(0.0f, 0.0f, 1.0f, 1.0f), m_scale(scale), m_extensionCode("")
{
    std::string epath;
    bool sucess = GetResourceDir(epath);
    if(sucess){
      m_extensionCode = FileToString(epath, "script/aiagent.js");
    }
    //m_openai = new OpenAI("http://192.168.8.37:8000/", "EMPTY", "Qwen-14B-Chat","v1");
    m_openai = new OpenAI("https://kingsware3.openai.azure.com/openai/deployments/",
     			  "3d93f1f773414565813b79e1767000ef",
     			  "kingsgpt35_16k",
     			  "2023-07-01-preview");

    m_taskQue = std::shared_ptr<std::queue<std::shared_ptr<BrowserTask>>>(new std::queue<std::shared_ptr<BrowserTask>>());

    m_load_handler = new LoadHandler();
}

//------------------------------------------------------------------------------
BrowserView::~BrowserView()
{
    CefDoMessageLoopWork();
    m_browser->GetHost()->CloseBrowser(true);

    m_browser = nullptr;
    m_client = nullptr;
}

//------------------------------------------------------------------------------
void BrowserView::load(const std::string &url)
{
    assert(m_initialized);
    m_browser->GetMainFrame()->LoadURL(url);
}

//------------------------------------------------------------------------------
void BrowserView::draw()
{
    CefDoMessageLoopWork();
    m_render_handler->draw(m_viewport);
}

//------------------------------------------------------------------------------
void BrowserView::reshape(int w, int h)
{
    m_render_handler->reshape(w, h);
    GLCHECK(glViewport(m_viewport[0],
                       m_viewport[1],
                       GLsizei(m_viewport[2] * w),
                       GLsizei(m_viewport[3] * h)));
    m_browser->GetHost()->WasResized();
}

void BrowserView::OnContextInitialized() {
    CEF_REQUIRE_UI_THREAD();
    CefWindowInfo window_info;
    window_info.SetAsWindowless(0);

    m_render_handler = new RenderHandler(m_viewport,m_scale);
    m_initialized = m_render_handler->init();
    m_render_handler->reshape(128, 128); // initial size

    m_lifespan_handler = new LifeSpanHandler();
    
    CefBrowserSettings browserSettings;
    browserSettings.windowless_frame_rate = 60; // 30 is default

    m_client = new BrowserClient(m_openai, this, m_render_handler, m_load_handler, m_lifespan_handler);
        
    std::string url = "about:blank";
    //std::string url = "http://www.baidu.com";
    
    m_browser = CefBrowserHost::CreateBrowserSync(window_info, m_client.get(),
                                                  url, browserSettings,
                                                  nullptr, nullptr);
    LOG(INFO) << "open browser finish!" << std::endl;
}

void BrowserView::OnContextCreated(CefRefPtr<CefBrowser> browser,
				   CefRefPtr<CefFrame> frame,
				   CefRefPtr<CefV8Context> context) {

  
    CefRefPtr<CefV8Value> object = context->GetGlobal();

    m_jsHandler = new JSV8Handler(m_browser, m_load_handler);
    
    CefRefPtr<CefV8Value> executefunc = CefV8Value::CreateFunction("ExecuteTask", m_jsHandler);
    CefRefPtr<CefV8Value> evaluatefunc = CefV8Value::CreateFunction("EvaluateResult", m_jsHandler);
    CefRefPtr<CefV8Value> taskfunc = CefV8Value::CreateFunction("TaskTimeout", m_jsHandler);
    CefRefPtr<CefV8Value> summaryfunc = CefV8Value::CreateFunction("LLMSummary", m_jsHandler);

    object->SetValue("ExecuteTask", executefunc, V8_PROPERTY_ATTRIBUTE_NONE);
    object->SetValue("EvaluateResult", evaluatefunc, V8_PROPERTY_ATTRIBUTE_NONE);
    object->SetValue("TaskTimeout", taskfunc, V8_PROPERTY_ATTRIBUTE_NONE);
    object->SetValue("LLMSummary", summaryfunc, V8_PROPERTY_ATTRIBUTE_NONE);
    LOG(INFO) << "Regist  handler finish!" << std::endl;
}

void BrowserView::OnWebKitInitialized() {
    // Register the extension.
    if(m_extensionCode.length()>0){
        LOG(INFO) << "Regist aiagent object..." << std::endl;
        CefRegisterExtension("v8/ai", m_extensionCode, nullptr);
    }
}

bool BrowserView::OnProcessMessageReceived( CefRefPtr< CefBrowser > browser,
					    CefRefPtr< CefFrame > frame,
					    CefProcessId source_process,
					    CefRefPtr< CefProcessMessage > message )
{
    LOG(INFO)<<"BrowserView hangdle message:"<< message->GetName() <<std::endl;
    CefRefPtr<CefListValue> args = message->GetArgumentList();
    if(message->GetName() == "JSCode"){
        //std::stringstream cmd;
	//cmd<<"aiagent.taskTimer("<<args->GetInt(1)<<");"<<std::endl;
	//frame->ExecuteJavaScript(cmd.str(), frame->GetURL(), 0);
	frame->ExecuteJavaScript(args->GetString(0), frame->GetURL(), 0);
    }else if(message->GetName() == "ContentLoaded"){
	browser->GetMainFrame()->SendProcessMessage(PID_BROWSER, message);
    }
    return true;
}

void BrowserView::Execute(std::string task, CefRefPtr<CefServer> server, int connection_id){
    std::stringstream cmd;
    cmd<<"aiagent.Execute('"<<task<<"');"<<std::endl;
    m_browser->GetMainFrame()->ExecuteJavaScript(cmd.str(), m_browser->GetMainFrame()->GetURL(), 0);
    m_ApiServer = server;
    m_ConnectionId = connection_id;
}

void BrowserView::OnExecutFinish(std::string msg){
    if(m_ApiServer != nullptr && m_ConnectionId >0){
        json jresponse;
	jresponse["msg"] = msg;
	std::string response = jresponse.dump();
	m_ApiServer ->SendWebSocketMessage(m_ConnectionId, response.c_str(), response.size());
    }
}
