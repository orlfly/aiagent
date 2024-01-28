#ifndef API_SERVER_HPP_
#define API_SERVER_HPP_

#include "BrowserView.hpp"
#include "include/base/cef_callback.h"
#include "include/base/cef_weak_ptr.h"
#include "include/cef_parser.h"
#include "include/cef_server.h"
#include "include/wrapper/cef_closure_task.h"
#include "include/wrapper/cef_helpers.h"
#include "include/cef_response.h"
#include "util.h"
#include <iostream>

// Handles the HTTP/WebSocket server.
class ServerHandler : public CefServerHandler {
public:
  using CompleteCallback = base::OnceCallback<void(bool /* success */)>;

  ServerHandler(CefRefPtr<BrowserView> browser):m_browser(browser) {}

  // |complete_callback| will be executed on the UI thread after completion.
  void StartServer(int port, CompleteCallback complete_callback){
      CEF_REQUIRE_UI_THREAD();
      DCHECK(!m_server);
      DCHECK(port >= 1025 && port <= 65535);
      m_port = port;
      m_complete_callback = std::move(complete_callback);
      CefServer::CreateServer("0.0.0.0", port, 10, this);
      std::cout<<"server start!"<<std::endl;
  }
  

  // |complete_callback| will be executed on the UI thread after completion.
  void StopServer(CompleteCallback complete_callback){
      CEF_REQUIRE_UI_THREAD();
      DCHECK(m_server);
      m_complete_callback = std::move(complete_callback);
      m_server->Shutdown();
  }
  
  // CefServerHandler methods are called on the server thread.

  void OnServerCreated(CefRefPtr<CefServer> server) override;
  
  void OnServerDestroyed(CefRefPtr<CefServer> server) override;
  
  void OnClientConnected(CefRefPtr<CefServer> server,
                         int connection_id) override {}

  void OnClientDisconnected(CefRefPtr<CefServer> server,
                            int connection_id) override {}

  void OnHttpRequest(CefRefPtr<CefServer> server,
                     int connection_id,
                     const CefString& client_address,
                     CefRefPtr<CefRequest> request) override;
  
  void OnWebSocketRequest(CefRefPtr<CefServer> server,
                          int connection_id,
                          const CefString& client_address,
                          CefRefPtr<CefRequest> request,
                          CefRefPtr<CefCallback> callback) override {
    // Always accept WebSocket connections.
    callback->Continue();
  }

  void OnWebSocketConnected(CefRefPtr<CefServer> server,
                            int connection_id) override {}

  void OnWebSocketMessage(CefRefPtr<CefServer> server,
                          int connection_id,
                          const void* data,
                          size_t data_size) override;
  
  int port() const { return m_port; }

 private:
  void RunCompleteCallback(bool success);
  
  static void SendHttpResponseStream(CefRefPtr<CefServer> server,
                                     int connection_id,
                                     const std::string& mime_type,
                                     CefRefPtr<CefStreamReader> stream,
                                     CefResponse::HeaderMap extra_headers);
  CefRefPtr<CefServer> m_server;

  // The below members are only accessed on the UI thread.
  int m_port;
  CompleteCallback m_complete_callback;
  CefRefPtr<BrowserView> m_browser;

  IMPLEMENT_REFCOUNTING(ServerHandler);
  DISALLOW_COPY_AND_ASSIGN(ServerHandler);
};

#endif
