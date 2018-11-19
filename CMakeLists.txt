cmake_minimum_required(VERSION 3.8)

project (tenhard)
set(CMAKE_CXX_STANDARD 17)

#cmake_policy(SET CMP0063 NEW)
#set(CMAKE_CXX_VISIBILITY_PRESET hidden)
#set(CMAKE_C_VISIBILITY_PRESET hidden)
#set(CMAKE_VISIBILITY_INLINES_HIDDEN hidden)
#set(TENHARD_TARGET_TYPE SHARED)
#set(TENHARD_TARGET_TYPE STATIC)

#set(CMAKE_INSTALL_PREFIX  /Users/gggin/workspace/tenhard)
#set(CMAKE_PREFIX_PATH f:/Program\ Files/easy_profiler/lib/cmake/easy_profiler)
#find_package(easy_profiler REQUIRED)  

set(TARGET_NAME sha1)


add_executable(
        ${TARGET_NAME}
		sha1.h
        sha1.cc
		sse.cc
)

add_executable(
        aes
        aes/matrix.cc
        aes/matrix.h
)

if(MSVC)
  target_compile_options(${TARGET_NAME} PRIVATE /W4 /WX)
else(MSVC)
  target_compile_options(${TARGET_NAME} PRIVATE -Wall -Wextra -pedantic -Werror)
endif(MSVC)

#target_link_libraries(sha1 easy_profiler)
