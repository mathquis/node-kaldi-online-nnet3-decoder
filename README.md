# node-kaldi-online-nnet3-decoder

Be aware that the decoder is designed for Kaldi "Grammar FST with on the fly parts"
https://kaldi-asr.org/doc/grammar.html

This module was put together for one of my side project. I'm not a C++ dev so it might not be perfectly functional.

Pull requests are welcome :)

## Compiling from source

A compiled version of Kaldi, OpenBLAS and OpenFST.

Set environment variable `KALDI_PATH` to your Kaldi root directory

### Windows

Requires CMake and Visual Studio 2017+ for Windows installed

- Download Kaldi source code from
- Extract in `${KALDI_PATH}`

#### Compiling OpenBlas with LAPACK (static)

See: https://github.com/xianyi/OpenBLAS/wiki/How-to-use-OpenBLAS-in-Microsoft-Visual-Studio

- In WSL 2 (debian)
- Download OpenBlas source code from https://github.com/xianyi/OpenBLAS
- Extract in `${KALDI_PATH}/tool/openblas`
- `cd ${KALDI_PATH}/tool/openblas`
- `make BINARY=64 HOSTCC=gcc CC=x86_64-w64-mingw32-gcc FC=x86_64-w64-mingw32-gfortran CFLAGS='-static-libgcc -static-libstdc++ -static -ggdb' FFLAGS='-static' && mv -f libopenblas.dll.a openblas.lib`

<!--
#### Compiling OpenFST (static)

- Download OpenFST for windows source code from https://github.com/kkm000/openfst.git
- Extract in `${KALDI_PATH}/tool/openfst`
- `CPPFLAGS="-DWINDOWS" CXXFLAGS="-static -static-libgcc -static-libstdc++ -fexceptions -O2 -Wa,-mbig-obj" ./configure --enable-static --enable-shared --enable-ngram-fsts -enable-far`
- `make`
-->

#### Compiling Kaldi (static AND shared)

./configure --static --shared --use-cuda=no --mathlib=OPENBLAS
make -j <NUM_CPU> clean depend ; make biglib -j <NUM_CPU>