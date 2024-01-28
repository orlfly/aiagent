// The MIT License (MIT)
// 
// Copyright (c) 2023 Olrea, Florian Dang
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#ifndef OPENAI_HPP_
#define OPENAI_HPP_

#include <include/cef_urlrequest.h>
#include "include/base/cef_callback.h"
#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <mutex>
#include <cstdlib>
#include <map>

#include "json.hpp"

using json = nlohmann::json;

class OpenAI :public CefURLRequestClient
{
public:
    using CompleteCallback = base::OnceCallback<void(const json&)>;
    OpenAI(std::string baseUrl,std::string token, std::string model, std::string version);
    void setFrame(CefRefPtr<CefFrame> frame){ mFrame=frame; }
    //openai completion api
    void completion(std::string prompt, CompleteCallback callback);
    void completionAzure(std::string prompt, CompleteCallback callback);
    void chatCompletion(std::string prompt,CompleteCallback callback);
    void chatCompletionAzure(std::string prompt,CompleteCallback callback);
    void request(std::string function, const json &data,CompleteCallback callback);
    void requestAzure(std::string function, const json &data,CompleteCallback callback);
    //prompt template api
    std::string prompt_template(std::string temp, const json &dict);
    
    void OnRequestComplete(CefRefPtr<CefURLRequest> request) override;
    void OnUploadProgress(CefRefPtr<CefURLRequest> request,
			  int64_t current,
			  int64_t total) override;

    void OnDownloadProgress(CefRefPtr<CefURLRequest> request,
			    int64_t current,
			    int64_t total) override;

    void OnDownloadData(CefRefPtr<CefURLRequest> request,
			const void* data,
			size_t data_length) override;

    bool GetAuthCredentials(bool isProxy,
			    const CefString& host,
			    int port,
			    const CefString& realm,
			    const CefString& scheme,
			    CefRefPtr<CefAuthCallback> callback) override;
    ~OpenAI();
private:
    CefRefPtr<CefFrame> mFrame=nullptr;
    std::string mBaseUrl;
    std::string mToken;
    std::string mModel;
    std::string mVersion;
    std::map<uint64_t,std::shared_ptr<std::stringstream>> m_dataMap;
    std::map<uint64_t,CompleteCallback> m_interMap;
private:
  IMPLEMENT_REFCOUNTING(OpenAI);
};

#endif // OPENAI_HPP_
