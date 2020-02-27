const Kaldi = require('./lib/')

function main() {

	const r1 = new Kaldi.EndpointRule({
		must_contain_nonsilence: true,
		min_trailing_silence: 3.0,
		max_relative_cost: 5.5,
		min_utterance_length: 12.0
	})

	console.log(r1)

	const r2 = new Kaldi.EndpointRule({
		must_contain_nonsilence: true,
		min_trailing_silence: 3.0,
		max_relative_cost: 5.5,
		min_utterance_length: 12.0
	})

	console.log(r2)

	const r3 = new Kaldi.EndpointRule({
		must_contain_nonsilence: true,
		min_trailing_silence: 3.0,
		max_relative_cost: 5.5,
		min_utterance_length: 12.0
	})

	console.log(r3)

	const r4 = new Kaldi.EndpointRule({
		must_contain_nonsilence: true,
		min_trailing_silence: 3.0,
		max_relative_cost: 5.5,
		min_utterance_length: 12.0
	})

	console.log(r4)

	const r5 = new Kaldi.EndpointRule({
		must_contain_nonsilence: true,
		min_trailing_silence: 3.0,
		max_relative_cost: 5.5,
		min_utterance_length: 12.0
	})

	console.log(r5)

	const m = new Kaldi.OnlineNNet3Model({
		model 						: './resources/model/final.mdl',
		graph 						: './resources/HCLG.fst',
		words 						: './resources/words.txt',
		mfcc_config 				: './resources/online/conf/mfcc.conf',
		ivector_extraction_config 	: './resources/online/conf/ivector_extractor.conf',
		global_cmvn_stats 			: './resources/online/ivector_extractor/global_cmvn.stats',
		frame_subsampling_factor 	: 3,
		acoustic_scale 				: 1.0,
		frames_per_chunk 			: 20,
		debug 						: true
	})

	console.log(m)

	const d = new Kaldi.OnlineNNet3GrammarDecoder(m, {
		beam: 12.0,
		lattice_beam: 4.0,
		max_active: 2500,
		min_active: 200,
		endpointing: {
			silence_phones: '1:2:3:4:5:6:7:8:9:10:11:12:13:14:15',
			rule1: r1,
			rule2: r2,
			rule3: r3,
			rule4: r4,
			rule5: r5
		}
	})

	console.log(d)
}

main()