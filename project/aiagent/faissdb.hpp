#ifndef FAISSDB_HPP
#define FAISSDB_HPP
#include <string>
#include "common.h"
#include <stdio.h>
#include "json.hpp"
#include <faiss/IndexFlat.h>
#include "gpt.hpp"

using json = nlohmann::json;

class FaissDB
{
public:
    static FaissDB* GetInstance();

    static void ReleaseInstance();
    void index(json& doc);
    json query(std::string query);
private:
    FaissDB();
    ~FaissDB();
    int m_dim;
    int m_dbsize;
    int m_qsize;
    faiss::IndexFlatL2 m_index;
    json m_dbdata;
    GPT *m_gpt;
};

#endif
