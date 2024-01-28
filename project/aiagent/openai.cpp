#include "openai.hpp"
#include <ctemplate/template.h>
#include "include/base/cef_logging.h"
#include "util.h"

OpenAI::OpenAI(std::string baseUrl, std::string token, std::string model, std::string version)
  :mBaseUrl(baseUrl),mToken(token),mModel(model),mVersion(version)
{
}

OpenAI::~OpenAI()
{
}
void OpenAI::OnRequestComplete(CefRefPtr<CefURLRequest> request){
    CefURLRequest::Status status = request->GetRequestStatus();
    CefURLRequest::ErrorCode error_code = request->GetRequestError();
    CefRefPtr<CefResponse> response = request->GetResponse();
    if(status != UR_SUCCESS)
    {
        std::cout<< "Request Error:" << error_code<< std::endl;
    }else{
        if(response->GetStatus() == 200)
	{
	    LOG(INFO)<< response->GetStatusText() << ":" << response->GetStatus() << std::endl;

	    CompleteCallback callback = std::move(m_interMap[request->GetRequest()->GetIdentifier()]);
	    std::shared_ptr<std::stringstream> datastream = m_dataMap[request->GetRequest()->GetIdentifier()];
	    const json resData = json::parse((*datastream).str());
	    std::move(callback).Run(resData);
	    m_interMap.erase(request->GetRequest()->GetIdentifier());
	    m_dataMap.erase(request->GetRequest()->GetIdentifier());
	}
    }   
}
void OpenAI::OnUploadProgress(CefRefPtr<CefURLRequest> request,
			      int64_t current,
			      int64_t total) {
}

void OpenAI::OnDownloadProgress(CefRefPtr<CefURLRequest> request,
			int64_t current,
			int64_t total) {
}

void OpenAI::OnDownloadData(CefRefPtr<CefURLRequest> request,
		    const void* data,
		    size_t data_length) {
    std::shared_ptr<std::stringstream> datastream = m_dataMap[request->GetRequest()->GetIdentifier()]; 
    (*datastream) <<std::string(static_cast<const char*>(data), data_length);
}
bool OpenAI::GetAuthCredentials(bool isProxy,
			const CefString& host,
			int port,
			const CefString& realm,
			const CefString& scheme,
			CefRefPtr<CefAuthCallback> callback) {
    return false;  
}

void OpenAI::completionAzure(std::string prompt, CompleteCallback callback){
    json data;
    data["prompt"] = prompt;
    data["max_tokens"] = 2000;
    data["temperature"] = 0;
    requestAzure("/completions", data, std::move(callback));
}
void OpenAI::completion(std::string prompt,CompleteCallback callback){
    json data;
    data["model"] = mModel;
    data["prompt"] = prompt;
    data["max_tokens"] = 2000;
    data["temperature"] = 0;
    request("/completions", data, std::move(callback));
}
void OpenAI::chatCompletionAzure(std::string prompt, CompleteCallback callback){
    json data;
    data["messages"][0]["role"] = "user";
    data["messages"][0]["content"] = prompt;
    data["max_tokens"] = 2000;
    data["temperature"] = 0;
    requestAzure("/chat/completions", data, std::move(callback));
}
void OpenAI::chatCompletion(std::string prompt, CompleteCallback callback){
    json data;
    data["model"] = mModel;
    data["messages"][0]["role"] = "user";
    data["messages"][0]["content"] = prompt;
    data["max_tokens"] = 2000;
    data["temperature"] = 0;
    request("/chat/completions", data, std::move(callback));
}
void OpenAI::requestAzure(std::string function, const json &data, CompleteCallback callback){
  
    std::string url = mBaseUrl+mModel+function+"?api-version="+mVersion;
    
    // Create a CefRequest object.
    CefRefPtr<CefRequest> request = CefRequest::Create();

    request->SetURL(url);

    request->SetMethod("POST");

    // Optionally specify custom headers.
    CefRequest::HeaderMap headerMap;
    headerMap.insert(std::make_pair("Content-Type", "application/json"));
    headerMap.insert(std::make_pair("api-key", mToken));
    request->SetHeaderMap(headerMap);
    std::string requestData = data.dump();
    CefRefPtr<CefPostData> postData = CefPostData::Create();
    CefRefPtr<CefPostDataElement> element = CefPostDataElement::Create();
    element->SetToBytes(requestData.size(), requestData.c_str());
    postData->AddElement(element);
    request->SetPostData(postData);
    std::shared_ptr<std::stringstream> sharedStream(new std::stringstream(""));
    m_dataMap.insert(std::make_pair(request->GetIdentifier(),sharedStream));
    m_interMap.insert(std::make_pair(request->GetIdentifier(),std::move(callback)));

    CefRefPtr<CefURLRequest> url_request = CefURLRequest::Create(request, this, nullptr);
}
void  OpenAI::request(std::string function, const json &data, CompleteCallback callback){
    std::string url = mBaseUrl+mVersion+function;
    std::cout << url << std::endl;
    // Create a CefRequest object.
    CefRefPtr<CefRequest> request = CefRequest::Create();

    request->SetURL(url);

    request->SetMethod("POST");

    // Optionally specify custom headers.
    CefRequest::HeaderMap headerMap;
    headerMap.insert(std::make_pair("Content-Type", "application/json"));
    headerMap.insert(std::make_pair("Authorization", ("Bearer " + mToken)));
    request->SetHeaderMap(headerMap);
    std::string requestData = data.dump();
    CefRefPtr<CefPostData> postData = CefPostData::Create();
    CefRefPtr<CefPostDataElement> element = CefPostDataElement::Create();
    element->SetToBytes(requestData.size(), requestData.c_str());
    postData->AddElement(element);
    request->SetPostData(postData);
    std::shared_ptr<std::stringstream> sharedStream(new std::stringstream(""));
    m_dataMap.insert(std::make_pair(request->GetIdentifier(),sharedStream));
    m_interMap.insert(std::make_pair(request->GetIdentifier(),std::move(callback)));
    
    CefRefPtr<CefURLRequest> url_request = CefURLRequest::Create(request, this, nullptr);
}
std::string OpenAI::prompt_template(std::string temp, const json &dict)
{
    std::string epath;
    bool success = GetResourceDir(epath);
    std::string prompt;
    if(success){
        ctemplate::TemplateDictionary params("params");
        params.SetValue("context", dict["context"].get<std::string>());
        ctemplate::ExpandTemplate(epath+"template/"+temp, ctemplate::DO_NOT_STRIP, &params, &prompt);
    }
    return prompt;
}
