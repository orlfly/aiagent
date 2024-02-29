#ifndef GPT_HPP
#define GPT_HPP
#include <string>
#include "common.h"
#include <stdio.h>
#include "include/base/cef_callback.h"
#include "json.hpp"
#include "gpt.hpp"

using json = nlohmann::json;

class GPT
{
public:
    using CompleteCallback = base::OnceCallback<void(const json&)>;
    static GPT* GetInstance();

    static void ReleaseInstance();
    std::vector<float> embedding(std::string doc);
    std::string generate(std::string prompt);
private:
    GPT();
    ~GPT();
    gpt_params m_params;
    llama_model *m_model;
    llama_context *m_ctx;
    llama_context *m_emb_ctx;
};

#endif
