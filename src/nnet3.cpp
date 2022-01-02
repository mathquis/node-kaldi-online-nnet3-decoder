#include "online2/online-nnet3-decoding.h"
#include "online2/online-nnet2-feature-pipeline.h"
#include "online2/onlinebin-util.h"
#include "online2/online-endpoint.h"
#include "fstext/fstext-lib.h"
#include "lat/lattice-functions.h"
#include "util/kaldi-thread.h"
#include "nnet3/nnet-utils.h"
#include "decoder/grammar-fst.h"
#include "util/kaldi-io.h"
#include "util/kaldi-table.h"
#include "lat/sausages.h"
#include "nnet3.h"

#include <napi.h>
#include <sys/types.h>
#include <signal.h>

// POSIX only:
// #include <unistd.h>
// Window only:
#include <stdint.h>

#include <math.h>
#include <time.h>
#include <string>
#include <numeric>

/* ============================================
 Online2 NNet3 acoustic model
============================================ */

using namespace kaldi;
using namespace fst;

void silent_log_handler (const kaldi::LogMessageEnvelope &envelope, const char *message) {
		// nothing - this handler simply keeps silent
}

Napi::FunctionReference OnlineNNet3Model::constructor;

Napi::Object OnlineNNet3Model::Init(Napi::Env env, Napi::Object exports) {

	Napi::HandleScope scope(env);

	// Model
	Napi::Function func = DefineClass(env, "OnlineNNet3Model", {
		InstanceMethod("loadLanguageModel", &OnlineNNet3Model::LoadLM),
	});

	constructor = Napi::Persistent(func);
	constructor.SuppressDestruct();

	exports.Set("OnlineNNet3Model", func);

	return exports;
}

OnlineNNet3Model::OnlineNNet3Model(const Napi::CallbackInfo& info) : Napi::ObjectWrap<OnlineNNet3Model>(info) {
	Napi::Env env = info.Env();
	Napi::HandleScope scope(env);

	// Configuration
	const Napi::Object config = info[0].As<Napi::Object>();

	if (!config.Get("debug").ToBoolean()) {
		SetLogHandler(silent_log_handler);
	}

	if (!config.Has("model")) throw Napi::Error::New(env, "Missing model path");
	std::string model_file = config.Get("model").ToString();

	// Options and configuration
	KALDI_LOG << "Loading feature configuration...";

	// Apply options
	if (config.Has("feature_type")) {
		feature_config.feature_type = config.Get("feature_type").ToString();
	}

	// Apply options
	if (config.Has("mfcc_config")) {
		feature_config.mfcc_config = config.Get("mfcc_config").ToString();
	}

	if (config.Has("ivector_extraction_config")) {
		feature_config.ivector_extraction_config = config.Get("ivector_extraction_config").ToString();
	}

	if (config.Has("global_cmvn_stats")) {
		feature_config.global_cmvn_stats_rxfilename = config.Get("global_cmvn_stats").ToString();
	}

	KALDI_LOG << "Loading decodable configuration...";

	if (config.Has("frame_subsampling_factor")) {
		decodable_opts.frame_subsampling_factor = config.Get("frame_subsampling_factor").ToNumber().Uint32Value();
	}

	if (config.Has("acoustic_scale")) {
		decodable_opts.acoustic_scale = config.Get("acoustic_scale").ToNumber().FloatValue();
	}

	if (config.Has("frames_per_chunk")) {
		decodable_opts.frames_per_chunk = config.Get("frames_per_chunk").ToNumber().Uint32Value();
	}

	// Create acoustic model
	KALDI_LOG << "Loading acoustic model: " << model_file;

	{
		bool binary;
		Input ki(model_file, &binary);
		trans_model.Read(ki.Stream(), binary);
		am_nnet.Read(ki.Stream(), binary);
		SetBatchnormTestMode(true, &(am_nnet.GetNnet()));
		SetDropoutTestMode(true, &(am_nnet.GetNnet()));
		nnet3::CollapseModel(nnet3::CollapseModelConfig(), &(am_nnet.GetNnet()));
	}

	// Create feature pipeline
	KALDI_LOG << "Loading feature pipeline info...";

	feature_info = new OnlineNnet2FeaturePipelineInfo(feature_config);

	KALDI_LOG << "Model loaded";
}

OnlineNNet3Model::~OnlineNNet3Model() {
	delete feature_info;
	feature_info = NULL;
	delete word_syms;
	word_syms = NULL;
}

Napi::Value OnlineNNet3Model::LoadLM(const Napi::CallbackInfo& info) {
	Napi::Env env = info.Env();
	Napi::HandleScope scope(env);

	// Configuration
	const Napi::Object config = info[0].As<Napi::Object>();

	if (!config.Has("graph")) throw Napi::Error::New(env, "Missing graph path");
	std::string graph_file = config.Get("graph").ToString();

	if (!config.Has("words")) throw Napi::Error::New(env, "Missing words path");
	std::string words_file = config.Get("words").ToString();

	// Create GrammarFST
	KALDI_LOG << "Loading GrammarFST: " << graph_file;

	ReadKaldiObject(graph_file, &decode_fst);

	// Read words symbol table
	KALDI_LOG << "Loading word symbols table: " << words_file;

	// delete word_syms;
	if (!words_file.empty())
		if (!(word_syms = fst::SymbolTable::ReadText(words_file)))
			KALDI_ERR << "Could not read symbol table from file " << words_file;

	KALDI_LOG << "Loaded " << word_syms->NumSymbols();

	return Napi::Boolean::New(env, true);
}

/* ============================================
 Online2 NNet3 Grammar FST decoder
============================================ */

Napi::FunctionReference OnlineNNet3GrammarDecoder::constructor;

Napi::Object OnlineNNet3GrammarDecoder::Init(Napi::Env env, Napi::Object exports) {

	Napi::HandleScope scope(env);

	Napi::Function func = DefineClass(env, "OnlineNNet3GrammarDecoder", {
		InstanceMethod("start", &OnlineNNet3GrammarDecoder::Start),
		InstanceMethod("pushChunk", &OnlineNNet3GrammarDecoder::PushChunk),
		InstanceMethod("getResult", &OnlineNNet3GrammarDecoder::GetResult)
	});

	constructor = Napi::Persistent(func);
	constructor.SuppressDestruct();

	exports.Set("OnlineNNet3GrammarDecoder", func);

	return exports;
}

OnlineNNet3GrammarDecoder::OnlineNNet3GrammarDecoder(const Napi::CallbackInfo& info) : Napi::ObjectWrap<OnlineNNet3GrammarDecoder>(info) {
	Napi::Env env = info.Env();
	Napi::HandleScope scope(env);

	decoder_           = nullptr;
	silence_weighting  = nullptr;
	feature_pipeline   = nullptr;
	adaptation_state   = nullptr;

	// Acoustic model
	aModel = Napi::ObjectWrap<OnlineNNet3Model>::Unwrap( info[0].As<Napi::Object>() );

	decodable_info = new nnet3::DecodableNnetSimpleLoopedInfo(aModel->decodable_opts, &aModel->am_nnet);

	const Napi::Object config = info[1].As<Napi::Object>();

	// Decoding
	if (config.Has("beam")) {
		decoder_opts.beam = config.Get("beam").ToNumber().FloatValue();
	}

	if (config.Has("lattice_beam")) {
		decoder_opts.lattice_beam = config.Get("lattice_beam").ToNumber().FloatValue();
	}

	if (config.Has("max_active")) {
		decoder_opts.max_active = config.Get("max_active").ToNumber().Uint32Value();
	}

	if (config.Has("min_active")) {
		decoder_opts.min_active = config.Get("min_active").ToNumber().Uint32Value();
	}

	// Endpointing
	if (config.Has("endpointing")) {

		KALDI_LOG << "Using endpointing...";

		const Napi::Object endpoint_config = config.Get("endpointing").ToObject();

		if (endpoint_config.Has("silence_phones")) {
			endpoint_opts.silence_phones = endpoint_config.Get("silence_phones").ToString();

			KALDI_LOG << "Using silence phones: " << endpoint_opts.silence_phones;
		}

		if (endpoint_config.Has("rule1")) {
			KALDI_LOG << "Using endpointing rule1...";

			endpoint_opts.rule1 = Napi::ObjectWrap<EndpointRule>::Unwrap(endpoint_config.Get("rule1").ToObject())->getRule();
		}
		if (endpoint_config.Has("rule2")) {
			KALDI_LOG << "Using endpointing rule2...";

			endpoint_opts.rule2 = Napi::ObjectWrap<EndpointRule>::Unwrap(endpoint_config.Get("rule2").ToObject())->getRule();
		}
		if (endpoint_config.Has("rule3")) {
			KALDI_LOG << "Using endpointing rule3...";

			endpoint_opts.rule3 = Napi::ObjectWrap<EndpointRule>::Unwrap(endpoint_config.Get("rule3").ToObject())->getRule();
		}
		if (endpoint_config.Has("rule4")) {
			KALDI_LOG << "Using endpointing rule4...";

			endpoint_opts.rule4 = Napi::ObjectWrap<EndpointRule>::Unwrap(endpoint_config.Get("rule4").ToObject())->getRule();
		}
		if (endpoint_config.Has("rule5")) {
			KALDI_LOG << "Using endpointing rule5...";

			endpoint_opts.rule5 = Napi::ObjectWrap<EndpointRule>::Unwrap(endpoint_config.Get("rule5").ToObject())->getRule();
		}
	}

	// // Create MBR
	KALDI_LOG << "Loading MBR...";
	MinimumBayesRiskOptions mbr_opts;
	mbr_opts.decode_mbr = true;

	// KALDI_LOG << "alloc: OnlineIvectorExtractorAdaptationState";

	adaptation_state  = new OnlineIvectorExtractorAdaptationState (aModel->feature_info->ivector_extractor_info);

	// KALDI_LOG << "alloc: OnlineSilenceWeighting";

	silence_weighting = new OnlineSilenceWeighting (aModel->trans_model,
													aModel->feature_info->silence_weighting_config,
													aModel->decodable_opts.frame_subsampling_factor);
}

OnlineNNet3GrammarDecoder::~OnlineNNet3GrammarDecoder() {
	FreeDecoder();
	delete silence_weighting;
	delete adaptation_state;
	delete decodable_info;
}

Napi::Value OnlineNNet3GrammarDecoder::Start(const Napi::CallbackInfo& info) {
	Napi::Env env = info.Env();
	Napi::HandleScope scope(env);

	if (!decoder_) {
			StartDecoding();
	}

	return Napi::Boolean::New(env, true);
}

void OnlineNNet3GrammarDecoder::StartDecoding(void) {

	KALDI_LOG << "start_decoding..." ;
	KALDI_LOG << "max_active  :" << decoder_opts.max_active;
	KALDI_LOG << "min_active  :" << decoder_opts.min_active;
	KALDI_LOG << "beam        :" << decoder_opts.beam;
	KALDI_LOG << "lattice_beam:" << decoder_opts.lattice_beam;

	FreeDecoder();

	t = clock();
	audio_duration = 0;

	// KALDI_LOG << "alloc: OnlineNnet2FeaturePipeline";

	feature_pipeline = new OnlineNnet2FeaturePipeline(*aModel->feature_info);
	feature_pipeline->SetAdaptationState(*adaptation_state);

	// KALDI_LOG << "alloc: SingleUtteranceNnet3DecoderTpl";

	decoder_ = new SingleUtteranceNnet3DecoderTpl<fst::ConstGrammarFst>(
		decoder_opts,
		aModel->trans_model,
		*decodable_info,
		aModel->decode_fst,
		feature_pipeline
	);

	decoder_->InitDecoding(0);

	KALDI_LOG << "start_decoding...done" ;
}

Napi::Value OnlineNNet3GrammarDecoder::PushChunk(const Napi::CallbackInfo& info) {
	using fst::VectorFst;

	Napi::Env env = info.Env();
	Napi::HandleScope scope(env);

	if (!decoder_) {
		StartDecoding();
	}

	const BaseFloat sample_rate		= info[0].As<Napi::Number>().FloatValue();
	const int32 num_frames			= info[1].As<Napi::Number>().Uint32Value();
	const Napi::Float32Array frames	= info[2].As<Napi::Float32Array>();

	Vector<BaseFloat> wave_part;

	wave_part.Resize(static_cast<MatrixIndexT>(num_frames));

	for (int i = 0; i < num_frames; i++) {
		wave_part(i) = static_cast<BaseFloat>(frames[i]);
	}

	audio_duration += num_frames / sample_rate;

	KALDI_LOG << "AcceptWaveform [sample_rate: " << sample_rate << "]";

	feature_pipeline->AcceptWaveform(sample_rate, wave_part);

	KALDI_LOG << "Wave accepted";

	if (silence_weighting->Active() && feature_pipeline->IvectorFeature() != NULL) {
		silence_weighting->ComputeCurrentTraceback(decoder_->Decoder());
		silence_weighting->GetDeltaWeights(feature_pipeline->NumFramesReady(), &delta_weights);
		feature_pipeline->IvectorFeature()->UpdateFrameWeights(delta_weights);
	}

	KALDI_LOG << "Advancing decoding...";

	decoder_->AdvanceDecoding();

	KALDI_LOG << "Decoding advanced";

	KALDI_LOG << "Detecting endpoint";

	bool endpoint_detected = decoder_->EndpointDetected(endpoint_opts);

	KALDI_LOG << "Endpoint? " << ( endpoint_detected ? "Yes" : "No" );

	return Napi::Boolean::New(env, endpoint_detected);
}

Napi::Value OnlineNNet3GrammarDecoder::GetResult(const Napi::CallbackInfo& info) {
	Napi::Env env = info.Env();
	Napi::HandleScope scope(env);

	// TODO: if (!decoder_) return false;

	KALDI_LOG << "Input finished";

	feature_pipeline->InputFinished();

	KALDI_LOG << "Final advance decoding";

	decoder_->AdvanceDecoding();

	KALDI_LOG << "Finalize decoding";

	decoder_->FinalizeDecoding();

	KALDI_LOG << "Frames decoded: " << decoder_->NumFramesDecoded();
	KALDI_LOG << "Audio duration: " << audio_duration;

	Napi::Object response = Napi::Object::New(env);

	Napi::Array word_confidences = Napi::Array::New(env);

	std::string text		= "";
	double likelihood		= 0.0;
	double seconds_taken	= 0.0;

	const int32 num_frames = decoder_->NumFramesDecoded();
	double seconds_per_frame = audio_duration / num_frames;

	if (num_frames > 0) {

		// KALDI_LOG << "Getting lattice";

		CompactLattice clat;
		decoder_->GetLattice(true, &clat);

		if (clat.NumStates() > 0) {

			// Resolve lattices
			CompactLattice best_path_clat;
			CompactLatticeShortestPath(clat, &best_path_clat);

			Lattice best_path_lat;
			ConvertLattice(best_path_clat, &best_path_lat);

			LatticeWeight weight;
			std::vector<int32> alignment;
			std::vector<int32> wordIds;
			GetLinearSymbolSequence(best_path_lat, &alignment, &wordIds, &weight);

			// Text and words
			std::ostringstream msg;
			for (size_t i = 0; i < wordIds.size(); i++) {
				std::string s = aModel->word_syms->Find(wordIds[i]);
				if (s.empty()) {
					KALDI_WARN << "Word-id " << wordIds[i] << " not in symbol table.";
				} else
					KALDI_LOG << "LAT Word: " << s;
					msg << s << ( i == wordIds.size() - 1 ? "" : " " );
				}

			// KALDI_LOG << "Got wordIds " << wordIds.size();

			text = msg.str();

			// MBR
			MinimumBayesRisk *mbr = NULL;
			mbr = new MinimumBayesRisk(clat, wordIds, mbr_opts);

			const std::vector<BaseFloat> &conf                         = mbr->GetOneBestConfidences();
			const std::vector<int32> &words                            = mbr->GetOneBest();
			const std::vector<std::pair<BaseFloat, BaseFloat> > &times = mbr->GetOneBestTimes();

			for (size_t i = 0; i < words.size(); i++) {
				std::string s = aModel->word_syms->Find(wordIds[i]);
				if (s.empty()) {
					KALDI_WARN << "Word-id " << words[i] << " not in symbol table.";
				} else
					KALDI_LOG << "MBR Word: " << s;
				}

			// KALDI_LOG << "Got words " << words.size();
			// KALDI_LOG << "Got conf " << conf.size();

			KALDI_ASSERT(conf.size() == words.size() && words.size() == times.size());

			KALDI_LOG << "Text: " << text;

			// Time
			t = clock() - t;
			seconds_taken = RoundFloat(((double)t)/CLOCKS_PER_SEC, 3);

			// Words and confidences
			double totalConfidence = 0;
			int position = 0;

			for (size_t i = 0; i < wordIds.size(); i++) {
				// Get the word symbol (# -> word)
				const std::string &word = aModel->word_syms->Find(wordIds[i]);
				const std::string &m = aModel->word_syms->Find(words[i]);

				double confidence = conf[i];
				if (!words[i]) {
					confidence = 1.0;
				}
				if (confidence < 0.0) {
					confidence = 0.0;
				} else if (confidence > 1.0) {
					confidence = 1.0;
				}

				totalConfidence += confidence;

				Napi::Object item = Napi::Object::New(env);
				item.Set("value", word);
				item.Set("confidence", RoundFloat(confidence, 5));

				// Word time
				Napi::Array t = Napi::Array::New(env, 2);

				uint32_t idx = 0;
				t.Set<double>(idx++, RoundFloat(times[i].first * seconds_per_frame, 2));
				t.Set<double>(idx++, RoundFloat(times[i].second * seconds_per_frame, 2));

				item.Set("time", t);

				item.Set("rangeStart", position);
				item.Set("rangeEnd", position + word.size());

				position += word.size() + ( i == wordIds.size() - 1 ? 0 : 1 ); // Add space

				// Add word to array
				word_confidences[i] = item;
			}

			likelihood = RoundFloat(totalConfidence / wordIds.size(), 5);
		}

		KALDI_LOG << "Decoded utterance in " << seconds_taken << "s with confidence " << likelihood;
	}

	FreeDecoder();

	response.Set("frames", num_frames);
	response.Set("text", text);
	response.Set("likelihood", likelihood);
	response.Set("seconds", RoundFloat(seconds_taken, 3));
	response.Set("duration", RoundFloat(audio_duration, 3));
	response.Set("tokens", word_confidences);

	return response;
}

double OnlineNNet3GrammarDecoder::RoundFloat(double number, const int decimals) {
	return round(number * pow(10, decimals)) / pow(10, decimals);
}

void OnlineNNet3GrammarDecoder::FreeDecoder(void) {
	KALDI_LOG << "free_decoder";
	delete decoder_;
	decoder_ = nullptr;
	delete feature_pipeline;
	feature_pipeline = nullptr;
}

/* ============================================
 Endpointing rule
============================================ */

Napi::FunctionReference EndpointRule::constructor;

Napi::Object EndpointRule::Init(Napi::Env env, Napi::Object exports) {

	Napi::HandleScope scope(env);

	// Model
	Napi::Function func = DefineClass(env, "EndpointRule", {});

	constructor = Napi::Persistent(func);
	constructor.SuppressDestruct();

	exports.Set("EndpointRule", func);

	return exports;
}

EndpointRule::EndpointRule(const Napi::CallbackInfo& info) : Napi::ObjectWrap<EndpointRule>(info) {
	Napi::Env env = info.Env();
	Napi::HandleScope scope(env);

	// Configuration
	const Napi::Object config = info[0].As<Napi::Object>();

	bool must_contain_nonsilence = true;
	if (config.Has("must_contain_nonsilence")) {
		must_contain_nonsilence = config.Get("must_contain_nonsilence").ToBoolean();
	}

	BaseFloat min_trailing_silence = 1.0;
	if (config.Has("min_trailing_silence")) {
		min_trailing_silence = config.Get("min_trailing_silence").ToNumber().FloatValue();
	}

	BaseFloat max_relative_cost = std::numeric_limits< BaseFloat >::infinity();
	if (config.Has("max_relative_cost")) {
		max_relative_cost = config.Get("max_relative_cost").ToNumber().FloatValue();
	}

	BaseFloat min_utterance_length = 0.0;
	if (config.Has("min_utterance_length")) {
		min_utterance_length = config.Get("min_utterance_length").ToNumber().FloatValue();
	}

	rule = new OnlineEndpointRule(must_contain_nonsilence, min_trailing_silence, max_relative_cost, min_utterance_length);
}

EndpointRule::~EndpointRule() {
	delete rule;
}

kaldi::OnlineEndpointRule EndpointRule::getRule() {
	return rule;
}

/* ============================================
 Native module initialization
============================================ */

Napi::Object Init(Napi::Env env, Napi::Object exports) {
	// Register the model
	OnlineNNet3Model::Init(env, exports);
	// Register the decoder
	OnlineNNet3GrammarDecoder::Init(env, exports);
	// Register the decoder
	EndpointRule::Init(env, exports);

	return exports;
};

NODE_API_MODULE(NODE_GYP_MODULE_NAME, Init)