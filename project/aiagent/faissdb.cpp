#include "faissdb.hpp"
#include <stdint.h>
#include <fstream>
#include <iostream>
#include "include/base/cef_logging.h"
#include <sstream>
#include "util.h"

namespace {
    FaissDB* s_instance = NULL;
};
FaissDB* FaissDB::GetInstance()
{
    if (s_instance == NULL)
    {
        s_instance = new FaissDB();
    }

    return s_instance;
}

void FaissDB::ReleaseInstance()
{
    if (s_instance != NULL)
    {
        delete s_instance;
    }

    s_instance = NULL;
}

FaissDB::FaissDB():
  m_dim(512),
  m_dbsize(10000),
  m_qsize(1000),
  m_index(m_dim)
{
   std::string path;
   if(GetResourceDir(path)){
       path.append("db/db.json");
       std::ifstream dbf(path.c_str());
       m_dbdata = json::parse(dbf);
       for(json::iterator it = m_dbdata.begin(); it != m_dbdata.end(); ++it)
       {
	  index(*it);
       }
   }
   m_gpt=GPT::GetInstance();
}

FaissDB::~FaissDB()
{

}
void FaissDB::index(json& doc){
    std::vector<float> emb = m_gpt->embedding(doc["content"]);
    m_index.add(1, emb.data());
}
json FaissDB::query(std::string query){
    json resp;
    return resp;
}
