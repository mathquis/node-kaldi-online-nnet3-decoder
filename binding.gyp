{
  "targets": [
    {
      "target_name": "kaldi",
      "cflags!": [ "-fno-exceptions" ],
      "cflags_cc!": [ "-fno-exceptions" ],
      "sources": [
        "./src/nnet3.cpp"
      ],
      "include_dirs": [
        "<!@(node -p \"require('node-addon-api').include\")",
        "/opt/kaldi/src",
        "/opt/kaldi/tools/openfst-1.6.7/include"
      ],
      "ldflags": [
        "-rdynamic",
        "-ldl",
        "-lm",
        "-lpthread"
      ],
      "cflags_cc": [
        "-std=c++11",
        "-Wall",
        "-Wno-sign-compare",
        "-Wno-unused-local-typedefs",
        "-Wno-deprecated-declarations",
        "-Winit-self",
        "-DKALDI_DOUBLEPRECISION=0",
        "-DHAVE_EXECINFO_H=1",
        "-DHAVE_CXXABI_H",
        "-DHAVE_ATLAS",
        "-m64",
        "-msse",
        "-msse2",
        "-pthread",
        "-g",
        "-fPIC",
        "-frtti"
      ],
      "library_dirs": [
        "/usr/lib",
        "/usr/local/lib",
        "/opt/kaldi/src/base",
        "/opt/kaldi/src/chain",
        "/opt/kaldi/src/cudamatrix",
        "/opt/kaldi/src/decoder",
        "/opt/kaldi/src/feat",
        "/opt/kaldi/src/fstext",
        "/opt/kaldi/src/gmm",
        "/opt/kaldi/src/hmm",
        "/opt/kaldi/src/ivector",
        "/opt/kaldi/src/lat",
        "/opt/kaldi/src/lm",
        "/opt/kaldi/src/matrix",
        "/opt/kaldi/src/nnet2",
        "/opt/kaldi/src/nnet3",
        "/opt/kaldi/src/online2",
        "/opt/kaldi/src/transform",
        "/opt/kaldi/src/tree",
        "/opt/kaldi/src/util"
      ],
      "libraries": [
        "-latlas",
        "-lfst",
        "-lkaldi-online2",
        "-lkaldi-ivector",
        "-lkaldi-nnet3",
        "-lkaldi-chain",
        "-lkaldi-nnet2",
        "-lkaldi-cudamatrix",
        "-lkaldi-decoder",
        "-lkaldi-lat",
        "-lkaldi-fstext",
        "-lkaldi-hmm",
        "-lkaldi-feat",
        "-lkaldi-transform",
        "-lkaldi-gmm",
        "-lkaldi-tree",
        "-lkaldi-util",
        "-lkaldi-matrix",
        "-lkaldi-base",
      ],
      'defines': [ 'NAPI_DISABLE_CPP_EXCEPTIONS' ],
    },
    {
      "target_name": "action_after_build",
      "type": "none",
      "dependencies": [ "<(module_name)" ],
      "copies": [
        {
          "files": [ "<(PRODUCT_DIR)/<(module_name).node" ],
          "destination": "<(module_path)"
        }
      ]
    }
  ]
}
