const Kaldi	= require('../lib/')
const File	= require('fs')
const Path	= require('path')
const Wav	= require('wavefile').WaveFile;
const Glob	= require('glob')

const testsGlob = Path.resolve(process.cwd(), process.argv[2])

async function main() {

	Glob(testsGlob, async (err, files) => {

		if ( err ) {
			console.error(err)
			process.exit(1)
		}

		if ( files.length === 0 ) {
			console.log('No file to test')
			process.exit(0)
		}

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
console.log(Kaldi.OnlineNNet3Model)
		const m = new Kaldi.OnlineNNet3Model({
			model 						: './resources/model/final.mdl',
			graph 						: './resources/HCLG_grammar.fst',
			words 						: './resources/words_grammar.txt',
			feature_type 				: 'mfcc',
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

		for ( let i = 0 ; i < files.length ; i++ ) {

			const file = files[i]
			const intentName = file.split('_')[0].split('/').pop()

			console.log('=====================================================')
			console.log(intentName)
			console.log(file)

			const wavFile = new Wav(File.readFileSync(file))
			wavFile.toSampleRate(16000, {method: "sinc"});
			// wavFile.toBitDepth("16");
			const samples = wavFile.getSamples(false, Float32Array)

			let start = 0
			const step = 256
			const num = samples.length

			while ( start < num ) {
				const s = samples.slice(start, start + step)
				start += step
				const endpointDetected = d.pushChunk(16000.0, s.length, s)

				if ( endpointDetected ) {
					console.log('>>>>>>>>>>>>>>>>>>>> ENDPOINT')
					break
				}

			}

			const res = d.getResult()

			console.dir(res, { depth: null })
		}

	})
}

main()