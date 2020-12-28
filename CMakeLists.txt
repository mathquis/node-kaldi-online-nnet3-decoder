project(kaldi)
cmake_minimum_required(VERSION 3.9)
cmake_policy(SET CMP0042 NEW)
set (CMAKE_CXX_STANDARD 11)

# -----------------------
# KALDI_PATH: Path to Kaldi
# OPENFST_PATH: Path to OpenFst
# OPENBLAS_PATH: Path to OpenBlas
# -----------------------
message("Kaldi: $ENV{KALDI_PATH}")
message("OpenFST: $ENV{OPENFST_PATH}")
message("OpenBLAS: $ENV{OPENBLAS_PATH}")

# Kaldi sources
set (KALDI_SRC_PATH $ENV{KALDI_PATH}/src)
message("Kaldi sources: ${KALDI_SRC_PATH}")

# Kaldi includes
set (KALDI_INCLUDE ${KALDI_SRC_PATH})
message("Kaldi include: ${KALDI_INCLUDE}")

# OpenFST includes
set (OPENFST_INCLUDE $ENV{OPENFST_PATH}/include)
message("OpenFST include: ${OPENFST_INCLUDE}")

# OpenBlas includes
set (OPENBLAS_INCLUDE $ENV{OPENBLAS_PATH}/include)
message("OpenBLAS include: ${OPENBLAS_INCLUDE}")

# define NPI_VERSION
add_definitions(-DNAPI_VERSION=3)
add_definitions(-DHAVE_OPENBLAS)
add_definitions(-DHAVE_LAPACK_CONFIG_H)
add_definitions(-DLAPACK_COMPLEX_STRUCTURE)
add_definitions(-DUSE_ONLY_PRERECORDED)

execute_process(COMMAND node -p "require('path').dirname(require('node-pre-gyp').find(path.resolve(path.join(__dirname,'./package.json'))))"
		WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
		OUTPUT_VARIABLE OUTPUT_DIRECTORY
		)
string(REPLACE "\n" "" OUTPUT_DIRECTORY ${OUTPUT_DIRECTORY})
string(REPLACE "\"" "" OUTPUT_DIRECTORY ${OUTPUT_DIRECTORY})

message("Output path: ${OUTPUT_DIRECTORY}")

file(GLOB SOURCE_FILES
	"src/nnet3.cpp"
)

if (WIN32)
	add_compile_options(-bigobj -wd4351 -wd4355 -wd4800 -wd4251 -wd4275 -wd4244 -wd4291 -wd4018 -wd4541 -wd4305 -wd4267 -wd4146)
endif (WIN32)

### INCLUDE

include_directories(
	${CMAKE_JS_INC}
	${KALDI_INCLUDE}
	${OPENFST_INCLUDE}
	${OPENBLAS_INCLUDE}
)

### Libraries

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
add_library(openblas			STATIC IMPORTED)
add_library(openfst				STATIC IMPORTED)

if (WIN32)
	set (KALDI_BUILD_PATH $ENV{KALDI_PATH}/kaldiwin_vs2017_OPENBLAS/${NODE_ARCH}/${CMAKE_BUILD_TYPE})
	message("Kaldi build path: ${KALDI_BUILD_PATH}")

	# The rest as static
	set_target_properties(openblas			PROPERTIES IMPORTED_LOCATION	"$ENV{OPENBLAS_PATH}/bin/libopenblas${CMAKE_STATIC_LIBRARY_SUFFIX}")
	set_target_properties(openfst			PROPERTIES IMPORTED_LOCATION	"$ENV{OPENFST_PATH}/build64/src/lib/Release/fst${CMAKE_STATIC_LIBRARY_SUFFIX}")
	set_target_properties(kaldi_base		PROPERTIES IMPORTED_LOCATION	"${KALDI_BUILD_PATH}/kaldi-base${CMAKE_STATIC_LIBRARY_SUFFIX}")
	set_target_properties(kaldi_chain		PROPERTIES IMPORTED_LOCATION	"${KALDI_BUILD_PATH}/kaldi-chain${CMAKE_STATIC_LIBRARY_SUFFIX}")
	set_target_properties(kaldi_cudamatrix	PROPERTIES IMPORTED_LOCATION	"${KALDI_BUILD_PATH}/kaldi-cudamatrix${CMAKE_STATIC_LIBRARY_SUFFIX}")
	set_target_properties(kaldi_decoder		PROPERTIES IMPORTED_LOCATION	"${KALDI_BUILD_PATH}/kaldi-decoder${CMAKE_STATIC_LIBRARY_SUFFIX}")
	set_target_properties(kaldi_feat		PROPERTIES IMPORTED_LOCATION	"${KALDI_BUILD_PATH}/kaldi-feat${CMAKE_STATIC_LIBRARY_SUFFIX}")
	set_target_properties(kaldi_fstext		PROPERTIES IMPORTED_LOCATION	"${KALDI_BUILD_PATH}/kaldi-fstext${CMAKE_STATIC_LIBRARY_SUFFIX}")
	set_target_properties(kaldi_gmm			PROPERTIES IMPORTED_LOCATION	"${KALDI_BUILD_PATH}/kaldi-gmm${CMAKE_STATIC_LIBRARY_SUFFIX}")
	set_target_properties(kaldi_hmm			PROPERTIES IMPORTED_LOCATION	"${KALDI_BUILD_PATH}/kaldi-hmm${CMAKE_STATIC_LIBRARY_SUFFIX}")
	set_target_properties(kaldi_ivector		PROPERTIES IMPORTED_LOCATION	"${KALDI_BUILD_PATH}/kaldi-ivector${CMAKE_STATIC_LIBRARY_SUFFIX}")
	set_target_properties(kaldi_lat			PROPERTIES IMPORTED_LOCATION	"${KALDI_BUILD_PATH}/kaldi-lat${CMAKE_STATIC_LIBRARY_SUFFIX}")
	set_target_properties(kaldi_matrix		PROPERTIES IMPORTED_LOCATION	"${KALDI_BUILD_PATH}/kaldi-matrix${CMAKE_STATIC_LIBRARY_SUFFIX}")
	set_target_properties(kaldi_nnet2		PROPERTIES IMPORTED_LOCATION	"${KALDI_BUILD_PATH}/kaldi-nnet2${CMAKE_STATIC_LIBRARY_SUFFIX}")
	set_target_properties(kaldi_nnet3		PROPERTIES IMPORTED_LOCATION	"${KALDI_BUILD_PATH}/kaldi-nnet3${CMAKE_STATIC_LIBRARY_SUFFIX}")
	set_target_properties(kaldi_online2		PROPERTIES IMPORTED_LOCATION	"${KALDI_BUILD_PATH}/kaldi-online2${CMAKE_STATIC_LIBRARY_SUFFIX}")
	set_target_properties(kaldi_transform	PROPERTIES IMPORTED_LOCATION	"${KALDI_BUILD_PATH}/kaldi-transform${CMAKE_STATIC_LIBRARY_SUFFIX}")
	set_target_properties(kaldi_tree		PROPERTIES IMPORTED_LOCATION	"${KALDI_BUILD_PATH}/kaldi-tree${CMAKE_STATIC_LIBRARY_SUFFIX}")
	set_target_properties(kaldi_util		PROPERTIES IMPORTED_LOCATION	"${KALDI_BUILD_PATH}/kaldi-util${CMAKE_STATIC_LIBRARY_SUFFIX}")
endif (WIN32)

if (UNIX)
	set_target_properties(openblas			PROPERTIES IMPORTED_LOCATION	"$ENV{OPENBLAS_PATH}/lib/libopenblas${CMAKE_STATIC_LIBRARY_SUFFIX}")
	set_target_properties(openfst			PROPERTIES IMPORTED_LOCATION	"$ENV{OPENFST_PATH}/lib/libfst${CMAKE_STATIC_LIBRARY_SUFFIX}")
	set_target_properties(kaldi_base		PROPERTIES IMPORTED_LOCATION	"${KALDI_SRC_PATH}/base/kaldi-base${CMAKE_STATIC_LIBRARY_SUFFIX}")
	set_target_properties(kaldi_chain		PROPERTIES IMPORTED_LOCATION	"${KALDI_SRC_PATH}/chain/kaldi-chain${CMAKE_STATIC_LIBRARY_SUFFIX}")
	set_target_properties(kaldi_cudamatrix	PROPERTIES IMPORTED_LOCATION	"${KALDI_SRC_PATH}/cudamatrix/kaldi-cudamatrix${CMAKE_STATIC_LIBRARY_SUFFIX}")
	set_target_properties(kaldi_decoder		PROPERTIES IMPORTED_LOCATION	"${KALDI_SRC_PATH}/decoder/kaldi-decoder${CMAKE_STATIC_LIBRARY_SUFFIX}")
	set_target_properties(kaldi_feat		PROPERTIES IMPORTED_LOCATION	"${KALDI_SRC_PATH}/feat/kaldi-feat${CMAKE_STATIC_LIBRARY_SUFFIX}")
	set_target_properties(kaldi_fstext		PROPERTIES IMPORTED_LOCATION	"${KALDI_SRC_PATH}/fstext/kaldi-fstext${CMAKE_STATIC_LIBRARY_SUFFIX}")
	set_target_properties(kaldi_gmm			PROPERTIES IMPORTED_LOCATION	"${KALDI_SRC_PATH}/gmm/kaldi-gmm${CMAKE_STATIC_LIBRARY_SUFFIX}")
	set_target_properties(kaldi_hmm			PROPERTIES IMPORTED_LOCATION	"${KALDI_SRC_PATH}/hmm/kaldi-hmm${CMAKE_STATIC_LIBRARY_SUFFIX}")
	set_target_properties(kaldi_ivector		PROPERTIES IMPORTED_LOCATION	"${KALDI_SRC_PATH}/ivector/kaldi-ivector${CMAKE_STATIC_LIBRARY_SUFFIX}")
	set_target_properties(kaldi_lat			PROPERTIES IMPORTED_LOCATION	"${KALDI_SRC_PATH}/lat/kaldi-lat${CMAKE_STATIC_LIBRARY_SUFFIX}")
	set_target_properties(kaldi_matrix		PROPERTIES IMPORTED_LOCATION	"${KALDI_SRC_PATH}/matrix/kaldi-matrix${CMAKE_STATIC_LIBRARY_SUFFIX}")
	set_target_properties(kaldi_nnet2		PROPERTIES IMPORTED_LOCATION	"${KALDI_SRC_PATH}/nnet2/kaldi-nnet2${CMAKE_STATIC_LIBRARY_SUFFIX}")
	set_target_properties(kaldi_nnet3		PROPERTIES IMPORTED_LOCATION	"${KALDI_SRC_PATH}/nnet3/kaldi-nnet3${CMAKE_STATIC_LIBRARY_SUFFIX}")
	set_target_properties(kaldi_online2		PROPERTIES IMPORTED_LOCATION	"${KALDI_SRC_PATH}/online2/kaldi-online2${CMAKE_STATIC_LIBRARY_SUFFIX}")
	set_target_properties(kaldi_transform	PROPERTIES IMPORTED_LOCATION	"${KALDI_SRC_PATH}/transform/kaldi-transform${CMAKE_STATIC_LIBRARY_SUFFIX}")
	set_target_properties(kaldi_tree		PROPERTIES IMPORTED_LOCATION	"${KALDI_SRC_PATH}/tree/kaldi-tree${CMAKE_STATIC_LIBRARY_SUFFIX}")
	set_target_properties(kaldi_util		PROPERTIES IMPORTED_LOCATION	"${KALDI_SRC_PATH}/util/kaldi-util${CMAKE_STATIC_LIBRARY_SUFFIX}")
endif (UNIX)

add_library(${PROJECT_NAME} SHARED ${SOURCE_FILES} ${CMAKE_JS_SRC})

set_target_properties(${PROJECT_NAME} PROPERTIES
	ARCHIVE_OUTPUT_DIRECTORY_RELEASE "${OUTPUT_DIRECTORY}"
	LIBRARY_OUTPUT_DIRECTORY_RELEASE "${OUTPUT_DIRECTORY}"
	RUNTIME_OUTPUT_DIRECTORY_RELEASE "${OUTPUT_DIRECTORY}"
	LINK_FLAGS_RELEASE -s
)

target_link_libraries(
	${PROJECT_NAME}
	${CMAKE_JS_LIB}
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
	openblas
	openfst
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

