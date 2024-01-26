#ifndef UTIL_H_
#define UTIL_H_

#include "include/cef_stream.h"

bool FileExists(const char* path);

CefRefPtr<CefStreamReader> GetBinaryResourceReader(const char* resource_name);

bool GetResourceDir(std::string& dir);

#endif
