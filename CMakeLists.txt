cmake_minimum_required(VERSION 3.29)

set(PROJECT_VERSION 1.0.${PATCH_VERSION})
project(helloworld VERSION 0.1.0 LANGUAGES C CXX)

cmake_policy(SET CMP0167 OLD)

set(SOURCE_FILES main.cpp lib.cpp)
add_executable(${PROJECT_NAME} ${SOURCE_FILES})

# set_target_properties(ProgramLab4 PROPERTIES
#     CXX_STANDARD 14
#     CXX_STANDARD_REQUIRED ON
# )