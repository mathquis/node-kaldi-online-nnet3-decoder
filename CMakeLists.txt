cmake_minimum_required(VERSION 3.9)
cmake_policy(SET CMP0042 NEW)
set (CMAKE_CXX_STANDARD 11)

# Requires:
# - KALDI_PATH: Path to Kaldi Git checkout
# - KALDI_LIB_PATH: Path to Kaldi compiled static libraries (LINUX: .a, WIN32: .lib)
# - OPENFST_LIB_PATH: Path to OpenFST static library (LINUX: .a, WIN32: .lib)
# - OPENFST_INCLUDE_PATH: Path to OpenFST includes (.h)
# - OPENBLAS_INCLUDE_PATH: Path to OpenBLAS includes (.h)
# - OPENBLAS_LIB_PATH: Path to OpenBLAS shared library (LINUX: .so, WIN32: .dll)

# set (KALDI_OPENFST_PATH $ENV{KALDI_PATH}/tools/openfst)
# set (KALDI_OPENBLAS_PATH $ENV{KALDI_PATH}/tools/OpenBlas)

# define NPI_VERSION
add_definitions(-DNAPI_VERSION=3)
add_definitions(-DHAVE_OPENBLAS)
add_definitions(-DHAVE_LAPACK_CONFIG_H)
add_definitions(-DLAPACK_COMPLEX_STRUCTURE)
add_definitions(-DUSE_ONLY_PRERECORDED)

execute_process(
	COMMAND node -p "require('path').dirname(require('node-pre-gyp').find(path.resolve(path.join(__dirname,'./package.json'))))"
	WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
	OUTPUT_VARIABLE OUTPUT_DIRECTORY
	)
string(REPLACE "\n" "" OUTPUT_DIRECTORY ${OUTPUT_DIRECTORY})
string(REPLACE "\"" "" OUTPUT_DIRECTORY ${OUTPUT_DIRECTORY})

message("KALDI_PATH: $ENV{KALDI_PATH}")
message("KALDI_LIB_PATH: $ENV{KALDI_LIB_PATH}")
message("OPENFST_LIB_PATH: $ENV{OPENFST_LIB_PATH}")
message("OPENFST_INCLUDE_PATH: $ENV{OPENFST_INCLUDE_PATH}")
message("OPENBLAS_INCLUDE_PATH: $ENV{OPENBLAS_INCLUDE_PATH}")
message("OPENBLAS_LIB_PATH: $ENV{OPENBLAS_LIB_PATH}")

project(kaldi)

file(
	GLOB
	SOURCE_FILES
	"src/nnet3.cpp"
)

### INCLUDE

include_directories(
	${CMAKE_JS_INC}
	$ENV{KALDI_PATH}/src
	$ENV{OPENFST_INCLUDE_PATH}
	$ENV{OPENBLAS_INCLUDE_PATH}
)

### LINK

link_directories(
	# $ENV{OPENFST_LIB_PATH}
	# $ENV{OPENBLAS_LIB_PATH}
)

if (WIN32)
	# Specific Windows compile options
	add_compile_options(-bigobj -wd4351 -wd4355 -wd4800 -wd4251 -wd4275 -wd4244 -wd4291 -wd4018 -wd4541 -wd4305 -wd4267 -wd4146)
endif (WIN32)

add_library(openblas			STATIC IMPORTED)
add_library(fst					STATIC IMPORTED)
add_library(kaldi_base			STATIC IMPORTED)
add_library(kaldi_chain			STATIC IMPORTED)
add_library(kaldi_cudamatrix	STATIC IMPORTED)
add_library(kaldi_decoder		STATIC IMPORTED)
add_library(kaldi_feat			STATIC IMPORTED)
add_library(kaldi_fstext		STATIC IMPORTED)
add_library(kaldi_gmm			STATIC IMPORTED)
add_library(kaldi_hmm			STATIC IMPORTED)
add_library(kaldi_ivector		STATIC IMPORTED)
add_library(kaldi_lat			STATIC IMPORTED)
add_library(kaldi_matrix		STATIC IMPORTED)
add_library(kaldi_nnet2			STATIC IMPORTED)
add_library(kaldi_nnet3			STATIC IMPORTED)
add_library(kaldi_online2		STATIC IMPORTED)
add_library(kaldi_transform		STATIC IMPORTED)
add_library(kaldi_tree			STATIC IMPORTED)
add_library(kaldi_util			STATIC IMPORTED)

set_target_properties(openblas			PROPERTIES IMPORTED_LOCATION	"$ENV{OPENBLAS_LIB_PATH}/libopenblas${CMAKE_STATIC_LIBRARY_SUFFIX}")
set_target_properties(fst				PROPERTIES IMPORTED_LOCATION	"$ENV{OPENFST_LIB_PATH}/fst${CMAKE_STATIC_LIBRARY_SUFFIX}")
set_target_properties(kaldi_base		PROPERTIES IMPORTED_LOCATION	"$ENV{KALDI_LIB_PATH}/kaldi-base${CMAKE_STATIC_LIBRARY_SUFFIX}")
set_target_properties(kaldi_chain		PROPERTIES IMPORTED_LOCATION	"$ENV{KALDI_LIB_PATH}/kaldi-chain${CMAKE_STATIC_LIBRARY_SUFFIX}")
set_target_properties(kaldi_cudamatrix	PROPERTIES IMPORTED_LOCATION	"$ENV{KALDI_LIB_PATH}/kaldi-cudamatrix${CMAKE_STATIC_LIBRARY_SUFFIX}")
set_target_properties(kaldi_decoder		PROPERTIES IMPORTED_LOCATION	"$ENV{KALDI_LIB_PATH}/kaldi-decoder${CMAKE_STATIC_LIBRARY_SUFFIX}")
set_target_properties(kaldi_feat		PROPERTIES IMPORTED_LOCATION	"$ENV{KALDI_LIB_PATH}/kaldi-feat${CMAKE_STATIC_LIBRARY_SUFFIX}")
set_target_properties(kaldi_fstext		PROPERTIES IMPORTED_LOCATION	"$ENV{KALDI_LIB_PATH}/kaldi-fstext${CMAKE_STATIC_LIBRARY_SUFFIX}")
set_target_properties(kaldi_gmm			PROPERTIES IMPORTED_LOCATION	"$ENV{KALDI_LIB_PATH}/kaldi-gmm${CMAKE_STATIC_LIBRARY_SUFFIX}")
set_target_properties(kaldi_hmm			PROPERTIES IMPORTED_LOCATION	"$ENV{KALDI_LIB_PATH}/kaldi-hmm${CMAKE_STATIC_LIBRARY_SUFFIX}")
set_target_properties(kaldi_ivector		PROPERTIES IMPORTED_LOCATION	"$ENV{KALDI_LIB_PATH}/kaldi-ivector${CMAKE_STATIC_LIBRARY_SUFFIX}")
set_target_properties(kaldi_lat			PROPERTIES IMPORTED_LOCATION	"$ENV{KALDI_LIB_PATH}/kaldi-lat${CMAKE_STATIC_LIBRARY_SUFFIX}")
set_target_properties(kaldi_matrix		PROPERTIES IMPORTED_LOCATION	"$ENV{KALDI_LIB_PATH}/kaldi-matrix${CMAKE_STATIC_LIBRARY_SUFFIX}")
set_target_properties(kaldi_nnet2		PROPERTIES IMPORTED_LOCATION	"$ENV{KALDI_LIB_PATH}/kaldi-nnet2${CMAKE_STATIC_LIBRARY_SUFFIX}")
set_target_properties(kaldi_nnet3		PROPERTIES IMPORTED_LOCATION	"$ENV{KALDI_LIB_PATH}/kaldi-nnet3${CMAKE_STATIC_LIBRARY_SUFFIX}")
set_target_properties(kaldi_online2		PROPERTIES IMPORTED_LOCATION	"$ENV{KALDI_LIB_PATH}/kaldi-online2${CMAKE_STATIC_LIBRARY_SUFFIX}")
set_target_properties(kaldi_transform	PROPERTIES IMPORTED_LOCATION	"$ENV{KALDI_LIB_PATH}/kaldi-transform${CMAKE_STATIC_LIBRARY_SUFFIX}")
set_target_properties(kaldi_tree		PROPERTIES IMPORTED_LOCATION	"$ENV{KALDI_LIB_PATH}/kaldi-tree${CMAKE_STATIC_LIBRARY_SUFFIX}")
set_target_properties(kaldi_util		PROPERTIES IMPORTED_LOCATION	"$ENV{KALDI_LIB_PATH}/kaldi-util${CMAKE_STATIC_LIBRARY_SUFFIX}")


add_library(${PROJECT_NAME} SHARED ${SOURCE_FILES} ${CMAKE_JS_SRC})

set_target_properties(${PROJECT_NAME} PROPERTIES
	ARCHIVE_OUTPUT_DIRECTORY_RELEASE "${OUTPUT_DIRECTORY}"
	LIBRARY_OUTPUT_DIRECTORY_RELEASE "${OUTPUT_DIRECTORY}"
	RUNTIME_OUTPUT_DIRECTORY_RELEASE "${OUTPUT_DIRECTORY}"
)

target_link_libraries(
	${PROJECT_NAME}
	${CMAKE_JS_LIB}
	openblas
	fst
	kaldi_online2
	kaldi_nnet3
	kaldi_nnet2
	kaldi_ivector
	kaldi_chain
	kaldi_decoder
	kaldi_lat
	kaldi_fstext
	kaldi_hmm
	kaldi_feat
	kaldi_transform
	kaldi_gmm
	kaldi_tree
	kaldi_cudamatrix
	kaldi_util
	kaldi_matrix
	kaldi_base
)

set_target_properties(${PROJECT_NAME} PROPERTIES PREFIX "" SUFFIX ".node")

# Include N-API wrappers
execute_process(COMMAND node -p "require('node-addon-api').include"
		WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
		OUTPUT_VARIABLE NODE_ADDON_API_DIR
		)
string(REPLACE "\n" "" NODE_ADDON_API_DIR ${NODE_ADDON_API_DIR})
string(REPLACE "\"" "" NODE_ADDON_API_DIR ${NODE_ADDON_API_DIR})
target_include_directories(${PROJECT_NAME} PRIVATE ${NODE_ADDON_API_DIR})

