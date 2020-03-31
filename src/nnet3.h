#ifndef INC_DECODER
#define INC_DECODER

#include <napi.h>
#include "online2/online-nnet3-decoding.h"
#include "online2/online-nnet2-feature-pipeline.h"
#include "online2/onlinebin-util.h"
#include "online2/online-endpoint.h"
#include "fstext/fstext-lib.h"
#include "lat/lattice-functions.h"
#include "util/kaldi-thread.h"
#include "nnet3/nnet-utils.h"
#include "decoder/grammar-fst.h"
#include "util/kaldi-table.h"

class OnlineNNet3Model : public Napi::ObjectWrap<OnlineNNet3Model> {
    friend class OnlineNNet3GrammarDecoder;
	public:
		static Napi::Object Init(Napi::Env env, Napi::Object exports);
		OnlineNNet3Model(const Napi::CallbackInfo& info);
		~OnlineNNet3Model();

	private:
		static Napi::FunctionReference constructor;

        fst::SymbolTable									*word_syms;

        kaldi::OnlineNnet2FeaturePipelineConfig				feature_config;
        kaldi::OnlineNnet2FeaturePipelineInfo				*feature_info;

        kaldi::nnet3::AmNnetSimple							am_nnet;
        kaldi::nnet3::NnetSimpleLoopedComputationOptions	decodable_opts;

        kaldi::TransitionModel								trans_model;
        fst::GrammarFst                           			decode_fst;
};

class OnlineNNet3GrammarDecoder : public Napi::ObjectWrap<OnlineNNet3GrammarDecoder> {
	public:
		static Napi::Object Init(Napi::Env env, Napi::Object exports);
		OnlineNNet3GrammarDecoder(const Napi::CallbackInfo& info);
		~OnlineNNet3GrammarDecoder();
		Napi::Value Start(const Napi::CallbackInfo& info);
		Napi::Value PushChunk(const Napi::CallbackInfo& info);
		Napi::Value GetResult(const Napi::CallbackInfo& info);

	private:
		static Napi::FunctionReference constructor;

        void StartDecoding(void);
        void FreeDecoder(void);
        double RoundFloat(double number, const int decimals);

		OnlineNNet3Model											*aModel;

        clock_t t;
        double audio_duration;

		kaldi::LatticeFasterDecoderConfig							decoder_opts;
		kaldi::MinimumBayesRiskOptions 								mbr_opts;
		kaldi::nnet3::DecodableNnetSimpleLoopedInfo 				*decodable_info;

		kaldi::OnlineEndpointConfig									endpoint_opts;
		kaldi::OnlineIvectorExtractorAdaptationState				*adaptation_state;
		kaldi::OnlineNnet2FeaturePipeline 							*feature_pipeline;
		kaldi::OnlineSilenceWeighting								*silence_weighting;

		kaldi::SingleUtteranceNnet3DecoderTpl<fst::GrammarFst>		*decoder_;

		std::vector<std::pair<int32, kaldi::BaseFloat> > 			delta_weights;
};

class EndpointRule : public Napi::ObjectWrap<EndpointRule> {
    friend class OnlineNNet3GrammarDecoder;
	public:
		static Napi::Object Init(Napi::Env env, Napi::Object exports);
		EndpointRule(const Napi::CallbackInfo& info);
		~EndpointRule();
		kaldi::OnlineEndpointRule getRule();

	private:
		static Napi::FunctionReference constructor;
		kaldi::OnlineEndpointRule 		*rule;
};

#endif /* INC_DECODER */