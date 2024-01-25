#include "openai.hpp"

OpenAI::OpenAI(std::string baseUrl,std::string token,std::string version)
  :mBaseUrl(baseUrl),mToken(token),mVersion(version)
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
	    std::cout<< response->GetStatusText() << ":" << response->GetStatus() << std::endl;
	    if(mFrame != nullptr)
	    {
		json jresponse = json::parse(download_data_.str());
		std::string jscode = jresponse["choices"][0]["message"]["content"].get<std::string>();
		CefRefPtr<CefProcessMessage> msg = CefProcessMessage::Create("JSCode");
		CefRefPtr<CefListValue> args = msg->GetArgumentList();
		args->SetString(0, jscode);
		mFrame->SendProcessMessage(PID_RENDERER, msg);
	    }
	}
    }   
}
void OpenAI::OnUploadProgress(CefRefPtr<CefURLRequest> request,
			      int64_t current,
			      int64_t total) {
    upload_total_ = total;
}

void OpenAI::OnDownloadProgress(CefRefPtr<CefURLRequest> request,
			int64_t current,
			int64_t total) {
    download_total_ = total;
}

void OpenAI::OnDownloadData(CefRefPtr<CefURLRequest> request,
		    const void* data,
		    size_t data_length) {
    download_data_ <<std::string(static_cast<const char*>(data), data_length);
}
bool OpenAI::GetAuthCredentials(bool isProxy,
			const CefString& host,
			int port,
			const CefString& realm,
			const CefString& scheme,
			CefRefPtr<CefAuthCallback> callback) {
    return false;  
}

void OpenAI::completionAzure(const json &data){

    std::string model_name = data["model"].get<std::string>();
    std::string url = mBaseUrl+model_name+"/chat/completions?api-version="+mVersion;
    std::cout << url << std::endl;
    
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

    CefRefPtr<CefURLRequest> url_request = CefURLRequest::Create(request, this, nullptr);
}
void  OpenAI::completion(const json &data){

    
    std::string url = mBaseUrl+mVersion+"/"+"chat/completions";
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

    CefRefPtr<CefURLRequest> url_request = CefURLRequest::Create(request, this, nullptr);
}

