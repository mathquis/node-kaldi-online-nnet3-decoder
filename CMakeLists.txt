cmake_minimum_required(VERSION 3.9)
cmake_policy(SET CMP0042 NEW)
set (CMAKE_CXX_STANDARD 11)

project(kaldi)

include_directories(
	${CMAKE_JS_INC}
	$ENV{KALDI_PATH}/src
	$ENV{OPENBLAS_PATH}/include
	$ENV{OPENFST_PATH}/src/include
)

link_directories(
	$ENV{OPENFSTLIB_PATH}/
	$ENV{OPENBLAS_PATH}/bin/
)

add_compile_options(-bigobj -wd4351 -wd4355 -wd4800 -wd4251 -wd4275 -wd4244 -wd4291 -wd4018 -wd4541 -wd4305 -wd4267 -wd4146)

file(GLOB SOURCE_FILES
	"src/nnet3.cpp"
)

add_library(${PROJECT_NAME} SHARED ${SOURCE_FILES} ${CMAKE_JS_SRC})

execute_process(COMMAND node -p "require('path').dirname(require('node-pre-gyp').find('D:/Projects/Assistant/node-kaldi-online-nnet3-decoder/package.json'))"
		WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
		OUTPUT_VARIABLE OUTPUT_DIRECTORY
		)
string(REPLACE "\n" "" OUTPUT_DIRECTORY ${OUTPUT_DIRECTORY})
string(REPLACE "\"" "" OUTPUT_DIRECTORY ${OUTPUT_DIRECTORY})

set_target_properties(${PROJECT_NAME} PROPERTIES
	ARCHIVE_OUTPUT_DIRECTORY_RELEASE "${OUTPUT_DIRECTORY}"
	LIBRARY_OUTPUT_DIRECTORY_RELEASE "${OUTPUT_DIRECTORY}"
	RUNTIME_OUTPUT_DIRECTORY_RELEASE "${OUTPUT_DIRECTORY}"
)

target_link_libraries(${PROJECT_NAME} libopenblas)
target_link_libraries(${PROJECT_NAME} fst)

add_library(kaldi_base STATIC IMPORTED)
set_target_properties(kaldi_base PROPERTIES IMPORTED_LOCATION "$ENV{KALDILIB_PATH}/kaldi-base.lib")
add_library(kaldi_chain STATIC IMPORTED)
set_target_properties(kaldi_chain PROPERTIES IMPORTED_LOCATION "$ENV{KALDILIB_PATH}/kaldi-chain.lib")
add_library(kaldi_cudamatrix STATIC IMPORTED)
set_target_properties(kaldi_cudamatrix PROPERTIES IMPORTED_LOCATION "$ENV{KALDILIB_PATH}/kaldi-cudamatrix.lib")
add_library(kaldi_decoder STATIC IMPORTED)
set_target_properties(kaldi_decoder PROPERTIES IMPORTED_LOCATION "$ENV{KALDILIB_PATH}/kaldi-decoder.lib")
add_library(kaldi_feat STATIC IMPORTED)
set_target_properties(kaldi_feat PROPERTIES IMPORTED_LOCATION "$ENV{KALDILIB_PATH}/kaldi-feat.lib")
add_library(kaldi_fstext STATIC IMPORTED)
set_target_properties(kaldi_fstext PROPERTIES IMPORTED_LOCATION "$ENV{KALDILIB_PATH}/kaldi-fstext.lib")
add_library(kaldi_gmm STATIC IMPORTED)
set_target_properties(kaldi_gmm PROPERTIES IMPORTED_LOCATION "$ENV{KALDILIB_PATH}/kaldi-gmm.lib")
add_library(kaldi_hmm STATIC IMPORTED)
set_target_properties(kaldi_hmm PROPERTIES IMPORTED_LOCATION "$ENV{KALDILIB_PATH}/kaldi-hmm.lib")
add_library(kaldi_ivector STATIC IMPORTED)
set_target_properties(kaldi_ivector PROPERTIES IMPORTED_LOCATION "$ENV{KALDILIB_PATH}/kaldi-ivector.lib")
add_library(kaldi_lat STATIC IMPORTED)
set_target_properties(kaldi_lat PROPERTIES IMPORTED_LOCATION "$ENV{KALDILIB_PATH}/kaldi-lat.lib")
add_library(kaldi_matrix STATIC IMPORTED)
set_target_properties(kaldi_matrix PROPERTIES IMPORTED_LOCATION "$ENV{KALDILIB_PATH}/kaldi-matrix.lib")
add_library(kaldi_nnet2 STATIC IMPORTED)
set_target_properties(kaldi_nnet2 PROPERTIES IMPORTED_LOCATION "$ENV{KALDILIB_PATH}/kaldi-nnet2.lib")
add_library(kaldi_nnet3 STATIC IMPORTED)
set_target_properties(kaldi_nnet3 PROPERTIES IMPORTED_LOCATION "$ENV{KALDILIB_PATH}/kaldi-nnet3.lib")
add_library(kaldi_online2 STATIC IMPORTED)
set_target_properties(kaldi_online2 PROPERTIES IMPORTED_LOCATION "$ENV{KALDILIB_PATH}/kaldi-online2.lib")
add_library(kaldi_transform STATIC IMPORTED)
set_target_properties(kaldi_transform PROPERTIES IMPORTED_LOCATION "$ENV{KALDILIB_PATH}/kaldi-transform.lib")
add_library(kaldi_tree STATIC IMPORTED)
set_target_properties(kaldi_tree PROPERTIES IMPORTED_LOCATION "$ENV{KALDILIB_PATH}/kaldi-tree.lib")
add_library(kaldi_util STATIC IMPORTED)
set_target_properties(kaldi_util PROPERTIES IMPORTED_LOCATION "$ENV{KALDILIB_PATH}/kaldi-util.lib")

target_link_libraries(${PROJECT_NAME} ${CMAKE_JS_LIB})
target_link_libraries(${PROJECT_NAME} kaldi_base)
target_link_libraries(${PROJECT_NAME} kaldi_chain)
target_link_libraries(${PROJECT_NAME} kaldi_cudamatrix)
target_link_libraries(${PROJECT_NAME} kaldi_decoder)
target_link_libraries(${PROJECT_NAME} kaldi_feat)
target_link_libraries(${PROJECT_NAME} kaldi_fstext)
target_link_libraries(${PROJECT_NAME} kaldi_gmm)
target_link_libraries(${PROJECT_NAME} kaldi_hmm)
target_link_libraries(${PROJECT_NAME} kaldi_ivector)
target_link_libraries(${PROJECT_NAME} kaldi_lat)
target_link_libraries(${PROJECT_NAME} kaldi_matrix)
target_link_libraries(${PROJECT_NAME} kaldi_nnet2)
target_link_libraries(${PROJECT_NAME} kaldi_nnet3)
target_link_libraries(${PROJECT_NAME} kaldi_online2)
target_link_libraries(${PROJECT_NAME} kaldi_transform)
target_link_libraries(${PROJECT_NAME} kaldi_tree)
target_link_libraries(${PROJECT_NAME} kaldi_util)
set_target_properties(${PROJECT_NAME} PROPERTIES PREFIX "" SUFFIX ".node")

# Include N-API wrappers
execute_process(COMMAND node -p "require('node-addon-api').include"
		WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
		OUTPUT_VARIABLE NODE_ADDON_API_DIR
		)
string(REPLACE "\n" "" NODE_ADDON_API_DIR ${NODE_ADDON_API_DIR})
string(REPLACE "\"" "" NODE_ADDON_API_DIR ${NODE_ADDON_API_DIR})
target_include_directories(${PROJECT_NAME} PRIVATE ${NODE_ADDON_API_DIR})

# define NPI_VERSION
add_definitions(-DNAPI_VERSION=3)
add_definitions(-DHAVE_OPENBLAS)
add_definitions(-DHAVE_OPENBLAS)
add_definitions(-DHAVE_LAPACK_CONFIG_H)
add_definitions(-DLAPACK_COMPLEX_STRUCTURE)
add_definitions(-DUSE_ONLY_PRERECORDED)
