cmake_minimum_required(VERSION 3.25)
project(rift CXX C)

add_subdirectory(be-engine)

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS src/*.cpp src/*.h src/*.hpp)
file(GLOB_RECURSE HLSL_FILES CONFIGURE_DEPENDS shaders/*.hlsl shaders/*.hlsli)
file(GLOB_RECURSE ASSET_FILES CONFIGURE_DEPENDS assets/*)
set_source_files_properties(${HLSL_FILES} ${ASSET_FILES} PROPERTIES HEADER_FILE_ONLY TRUE)

add_executable(rift ${SOURCES} ${HLSL_FILES} ${ASSET_FILES})
target_include_directories(rift PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src")
target_link_libraries(rift PRIVATE core toolkit lua luabridge3)

be_apply_defaults(rift)
be_cook_app(rift)
be_copy_runtime_deps(rift)
