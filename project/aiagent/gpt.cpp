#include "gpt.hpp"
#include <stdint.h>
#include "include/base/cef_logging.h"
#include <sstream>
#include "util.h"

namespace {
    GPT* s_instance = NULL;
};
GPT* GPT::GetInstance()
{
    if (s_instance == NULL)
    {
        s_instance = new GPT();
    }

    return s_instance;
}

void GPT::ReleaseInstance()
{
    if (s_instance != NULL)
    {
        delete s_instance;
    }

    s_instance = NULL;
}

GPT::GPT()
{
    std::string path;
    if (GetResourceDir(path)){
      
        llama_backend_init();
	llama_numa_init(m_params.numa);

	// initialize the model

	path.append("models/qwen1_5-1_8b-chat-q4_0.gguf");
	auto mparams = llama_model_params_from_gpt_params(m_params);

	m_model  = llama_load_model_from_file(path.c_str(), mparams);
	
	if (m_model != NULL) {
	    // initialize the context
	    m_params.embedding = false;
	    auto cparams = llama_context_params_from_gpt_params(m_params);

	    m_ctx = llama_new_context_with_model(m_model, cparams);
	    

	    m_params.embedding = true;
	    auto ecparams = llama_context_params_from_gpt_params(m_params);

	    m_emb_ctx = llama_new_context_with_model(m_model, ecparams);

	}
    }
}

GPT::~GPT()
{
    if (m_ctx != NULL) {
        llama_free(m_ctx);
    }
    if (m_model != NULL) {
        llama_free_model(m_model);
    }
    llama_backend_free();
}

std::string GPT::generate(std::string prompt){
    const int n_len = 32;
    std::vector<llama_token> tokens_list;
    tokens_list = ::llama_tokenize(m_ctx, prompt, true);
    std::stringstream outstream;
    const int n_ctx    = llama_n_ctx(m_ctx);
    const int n_kv_req = tokens_list.size() + (n_len - tokens_list.size());

    LOG(INFO)<<"n_len = "<<n_len<<", n_ctx = "<<n_ctx<<", n_kv_req = "<<n_kv_req<<std::endl;

    // make sure the KV cache is big enough to hold all the prompt and generated tokens
    if (n_kv_req < n_ctx) {
        // print the prompt token-by-token

        llama_batch batch = llama_batch_init(512, 0, 1);

	// evaluate the initial prompt
	for (size_t i = 0; i < tokens_list.size(); i++) {
	    llama_batch_add(batch, tokens_list[i], i, { 0 }, false);
	}

	// llama_decode will output logits only for the last token of the prompt
	batch.logits[batch.n_tokens - 1] = true;

	if (llama_decode(m_ctx, batch) == 0) {
	    // main loop

	    int n_cur    = batch.n_tokens;
	    int n_decode = 0;

	    while (n_cur <= n_len) {
	      // sample the next token
	      {
		auto   n_vocab = llama_n_vocab(m_model);
		auto * logits  = llama_get_logits_ith(m_ctx, batch.n_tokens - 1);

		std::vector<llama_token_data> candidates;
		candidates.reserve(n_vocab);

		for (llama_token token_id = 0; token_id < n_vocab; token_id++) {
		  candidates.emplace_back(llama_token_data{ token_id, logits[token_id], 0.0f });
		}

		llama_token_data_array candidates_p = { candidates.data(), candidates.size(), false };

		// sample the most likely token
		const llama_token new_token_id = llama_sample_token_greedy(m_ctx, &candidates_p);

		// is it an end of stream?
		if (new_token_id == llama_token_eos(m_model) || n_cur == n_len) {
		  outstream << std::endl;
		  break;
		}

		outstream << llama_token_to_piece(m_ctx, new_token_id);
		// prepare the next batch
		llama_batch_clear(batch);

		// push this new token for next evaluation
		llama_batch_add(batch, new_token_id, n_cur, { 0 }, true);
		
		n_decode += 1;
	      }

	      n_cur += 1;
	      
	      // evaluate the current batch with the transformer model
	      if (llama_decode(m_ctx, batch)) {
		break;
	      }
	    }
	    outstream << std::endl;
	}
    }
    return outstream.str();
	
}
static std::vector<std::string> split_lines(const std::string & s) {
    std::string line;
    std::vector<std::string> lines;
    std::stringstream ss(s);
    while (std::getline(ss, line)) {
        lines.push_back(line);
    }
    return lines;
}

static void batch_add_seq(llama_batch & batch, const std::vector<int32_t> & tokens, int seq_id) {
    for (size_t i = 0; i < tokens.size(); i++) {
        llama_batch_add(batch, tokens[i], i, { seq_id }, false);
    }
}

static void normalize(float * vec, float * out, int n) {
    float norm = 0;
    for (int i = 0; i < n; i++) {
        norm += vec[i] * vec[i];
    }
    norm = sqrt(norm);
    for (int i = 0; i < n; i++) {
        out[i] = vec[i] / norm;
    }
}

static void batch_decode(llama_context * ctx, llama_batch & batch, float * output, int n_seq, int n_embd) {
    // clear previous kv_cache values (irrelevant for embeddings)
    llama_kv_cache_clear(ctx);

    // run model
    fprintf(stderr, "%s: n_tokens = %d, n_seq = %d\n", __func__, batch.n_tokens, n_seq);
    if (llama_decode(ctx, batch) < 0) {
        fprintf(stderr, "%s : failed to decode\n", __func__);
    }

    // normalize on copy
    for (int k = 0; k < n_seq; k++) {
        float * emb = llama_get_embeddings_ith(ctx, k);
        float * out = output + k * n_embd;
        normalize(emb, out, n_embd);
    }
}
std::vector<float> GPT::embedding(std::string doc){
    const int n_ctx_train = llama_n_ctx_train(m_model);
    const int n_ctx = llama_n_ctx(m_emb_ctx);

    if (n_ctx < n_ctx_train) {
        // split the prompt into lines
        std::vector<std::string> prompts = split_lines(doc);

	// max batch size
	const uint64_t n_batch = m_params.n_batch;
	GGML_ASSERT(m_params.n_batch == m_params.n_ctx);

	// tokenize the prompts and trim
	std::vector<std::vector<int32_t>> inputs;
	for (const auto & prompt : prompts) {
	    auto inp = ::llama_tokenize(m_emb_ctx, prompt, true);
	    if (inp.size() > n_batch) {
	        inp.resize(n_batch);
	    }
	    inputs.push_back(inp);
	}

	// initialize batch
	const int n_prompts = prompts.size();
	struct llama_batch batch = llama_batch_init(n_batch, 0, n_prompts);

	// allocate output
	const int n_embd = llama_n_embd(m_model);
	std::vector<float> embeddings(n_prompts * n_embd, 0);
	float * emb = embeddings.data();

	// break into batches
	int p = 0; // number of prompts processed already
	int s = 0; // number of prompts in current batch
	for (int k = 0; k < n_prompts; k++) {
	    // clamp to n_batch tokens
	    auto & inp = inputs[k];
	    const uint64_t n_toks = inp.size();

	    // encode if at capacity
	    if (batch.n_tokens + n_toks > n_batch) {
	        float * out = emb + p * n_embd;
		batch_decode(m_emb_ctx, batch, out, s, n_embd);
		llama_batch_clear(batch);
		p += s;
		s = 0;
	    }

	    // add to batch
	    batch_add_seq(batch, inp, s);
	    s += 1;
	}

	// final batch
	float * out = emb + p * n_embd;
	batch_decode(m_emb_ctx, batch, out, s, n_embd);
	return embeddings;
    }else{
        std::vector<float> embeddings(0, 0);
	return embeddings;
    }
    
}
