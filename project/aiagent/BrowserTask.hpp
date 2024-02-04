#ifndef BROWSERTASK_HPP
#define BROWSERTASK_HPP
#include "include/base/cef_callback.h"
#include <string>
#include "json.hpp"

using json = nlohmann::json;

class BrowserTask{
public:
  using BrowserCallback = base::OnceCallback<void(const json&)>;
  int m_tid;
  std::string m_taskDesc;
  BrowserCallback m_callback;
  BrowserTask(int tid, std::string taskDesc,BrowserCallback callback):m_tid(tid), m_taskDesc(taskDesc){
      m_callback=std::move(callback);
  }
};


#endif
