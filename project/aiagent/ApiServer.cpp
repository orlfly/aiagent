#include "ApiServer.hpp"
#include "util.h"
#include "json.hpp"

using json = nlohmann::json;

void ServerHandler::OnServerCreated(CefRefPtr<CefServer> server){
    DCHECK(!m_server);
    m_server = server;
    RunCompleteCallback(server->IsRunning());
}
void ServerHandler::OnServerDestroyed(CefRefPtr<CefServer> server){
    DCHECK(m_server);
    m_server = nullptr;
    RunCompleteCallback(true);
}
void ServerHandler::OnHttpRequest(CefRefPtr<CefServer> server,
				  int connection_id,
				  const CefString& client_address,
				  CefRefPtr<CefRequest> request){
    // Parse the request URL and retrieve the path without leading slash.
    CefURLParts url_parts;
    CefParseURL(request->GetURL(), url_parts);
    std::string path = CefString(&url_parts.path);
    if (!path.empty() && path[0] == '/') {
      path = path.substr(1);
    }
    if (path.empty()) {
      path = "websocket.html";
    }
    std::string mime_type;
    const size_t sep = path.find_last_of(".");
    if (sep != std::string::npos) {
      // Determine the mime type based on the extension.
      mime_type = CefGetMimeType(path.substr(sep + 1));
    } else {
      // No extension. Assume html.
      path += ".html";
    }
    if (mime_type.empty()) {
      mime_type = "text/html";
    }
    CefRefPtr<CefStreamReader> stream;
    CefResponse::HeaderMap extra_headers;
    // Load any resource supported by cefclient.
    stream = GetBinaryResourceReader(path.c_str());

    if (stream) {
      SendHttpResponseStream(server, connection_id, mime_type, stream,
                             extra_headers);
    } else {
      server->SendHttp404Response(connection_id);
    }
}
void ServerHandler::OnWebSocketMessage(CefRefPtr<CefServer> server,
				       int connection_id,
				       const void* data,
				       size_t data_size){
    // Echo the reverse of the message.
    json msg = json::parse((char*)data);
    std::string message = msg.dump();

    m_browser->SengUserMessage(msg);
    
    server->SendWebSocketMessage(connection_id, message.c_str(), message.size());
}

void ServerHandler::RunCompleteCallback(bool success){

    if (!CefCurrentlyOn(TID_UI)) {
      CefPostTask(TID_UI, base::BindOnce(&ServerHandler::RunCompleteCallback,
                                         this, success));
      return;
    }

    if (!m_complete_callback.is_null()) {
      std::move(m_complete_callback).Run(success);
    }
}

void ServerHandler::SendHttpResponseStream(CefRefPtr<CefServer> server,
					   int connection_id,
					   const std::string& mime_type,
					   CefRefPtr<CefStreamReader> stream,
					   CefResponse::HeaderMap extra_headers){
    // Determine the stream size.
    stream->Seek(0, SEEK_END);
    int64_t content_length = stream->Tell();
    stream->Seek(0, SEEK_SET);

    // Send response headers.
    server->SendHttpResponse(connection_id, 200, mime_type, content_length,
                             extra_headers);

    // Send stream contents.
    char buffer[8192];
    size_t read;
    do {
      read = stream->Read(buffer, 1, sizeof(buffer));
      if (read > 0) {
        server->SendRawData(connection_id, buffer, read);
      }
    } while (!stream->Eof() && read != 0);

    // Close the connection.
    server->CloseConnection(connection_id);
}

