#include "util.h"
#include <iostream>
bool FileExists(const char* path) {
    FILE* f = fopen(path, "rb");
    if (f) {
      fclose(f);
      return true;
    }
    return false;
}

CefRefPtr<CefStreamReader> GetBinaryResourceReader(const char* resource_name) {
    std::string path;
    if (!GetResourceDir(path))
      return nullptr;

    path.append("html/");
    path.append(resource_name);
    std::cout<<path<<std::endl;
    if (!FileExists(path.c_str()))
      return nullptr;

    return CefStreamReader::CreateForFile(path);
}

bool GetResourceDir(std::string& dir) {
    char buff[1024];

    // Retrieve the executable path.
    ssize_t len = readlink("/proc/self/exe", buff, sizeof(buff) - 1);
    if (len == -1)
      return false;

    buff[len] = 0;

    // Remove the executable name from the path.
    char* pos = strrchr(buff, '/');
    if (!pos)
      return false;

    strcpy(pos + 1, "");
    dir = std::string(buff);
    return true;
}
