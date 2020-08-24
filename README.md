# node-kaldi-online-nnet3-decoder

Be aware that the decoder is designed for Kaldi "Grammar FST with on the fly parts"
https://kaldi-asr.org/doc/grammar.html

This module was put together for one of my side project. I'm not a C++ dev so it might not be perfectly functional.

Pull requests are welcome :)

## Requirements

A compiled version of Kaldi in `${KALDI_PATH}`.

## Building on Windows

Requires: cmake (windows), Visual Studio 2017

```bash
mkdir kaldi-tools
```

### OpenFST

```bash
cd kaldi-tools
git clone https://github.com/kkm000/openfst.git
cd openfst
mkdir build
cd build
cmake -G "Visual Studio 15 2017 Win64" ../
```

Open `openfst.sln` in Visual Studio 2017 and generate solution

Define the path to OpenFST includes and libraries

```bash
set OPENFST_INCLUDE_PATH=%cd%\\src\\include
set OPENFST_LIB_PATH=%cd%\\build\\src\\lib\\Release
```

### OpenBLAS
See: https://benniesoft.wordpress.com/2017/08/07/building-openblas-for-windows-without-headache/

Requires: WSL 2 (debian)

```bash
sudo apt-get install -y build-essential gcc-mingw-w64-x86-64 gfortran-mingw-w64-x86-64
cd kaldi-tools
git clone https://github.com/xianyi/OpenBLAS
cd OpenBLAS
make BINARY=64 HOSTCC=gcc CC=x86_64-w64-mingw32-gcc FC=x86_64-w64-mingw32-gfortran CFLAGS='-static-libgcc -static-libstdc++ -static -ggdb' FFLAGS='-static' && mv -f libopenblas.dll.a libopenblas.lib
mkdir -p build/bin && mkdir -p build/include && mkdir -p build/lib
cp ./libopenblas.dll ./build/bin/ && cp ./libopenblas.lib ./build/bin
cp ./lapack-netlib/CBLAS/include/cblas.h ./build/include/cblas.h
cp ./lapack-netlib/CBLAS/include/cblas_mangling_with_flags.h.in ./build/include/cblas_mangling.h
cp ./lapack-netlib/CBLAS/include/cblas_f77.h ./build/include/f77cblas.h
cp ./lapack-netlib/LAPACKE/include/lapack.h ./build/include/lapack.h
cp ./lapack-netlib/LAPACKE/include/lapacke.h ./build/include/lapacke.h
cp ./lapack-netlib/LAPACKE/include/lapacke_config.h ./build/include/lapacke_config.h
cp ./lapack-netlib/LAPACKE/include/lapacke_mangling.h ./build/include/lapacke_mangling.h
cp ./lapack-netlib/LAPACKE/include/lapacke_utils.h ./build/include/lapacke_utils.h
cp ./config.h ./build/include/openblas_config.h
cp ./libopenblas.lib ./build/lib/libopenblas.dll.a
```

Define the path to OpenBLAS includes and libraries

```bash
set OPENBLAS_INCLUDE_PATH=%cd%\\build\\include
set OPENBLAS_LIB_PATH=%cd%\\build\\lib
```

**NOTE:** Add `libopenblas.dll` to `PATH`.

### Kaldi

Requires: perl, Visual Studio 2017

Follow https://github.com/kaldi-asr/kaldi/blob/master/windows/INSTALL.md from #4

```bash
set KALDI_PATH=...
cd %KALDI_PATH%
cd windows
generate_solution.pl --vsver vs2017 --enable-openblas
get_version.pl
```

Open `kaldiwin_vs2017.sln` in Visual Studio 2017 and generate `kaldi-*` projects to create:

```markdown
- kaldi-base.lib
- kaldi-chain.lib
- kaldi-cudamatrix.lib
- kaldi-decoder.lib
- kaldi-feat.lib
- kaldi-fstext.lib
- kaldi-gmm.lib
- kaldi-hmm.lib
- kaldi-ivector.lib
- kaldi-lat.lib
- kaldi-matrix.lib
- kaldi-nnet2.lib
- kaldi-nnet3.lib
- kaldi-online2.lib
- kaldi-transform.lib
- kaldi-tree.lib
- kaldi-util.lib
```

Define the path to Kaldi static libraries

```bash
set KALDI_LIB_PATH=%KALDI_PATH%\\kaldiwin_vs2017_OPENBLAS\\x64\Release
```

