cmake_minimum_required(VERSION 3.2)
project(short_alloc)


###########
# Toolchain
###########
set(toolchain   "$ENV{HOME}/toolchain_desktop.cmake"      CACHE FILEPATH     "")  # override with -D toolchain=~/toolchain_desktop.cmake
                                                                              # e.g. cmake -D toolchain="" ..

if (toolchain AND (EXISTS ${toolchain}))
  message("==> Including toolchain_file ${toolchain}")
  include(${toolchain})
endif()


###########
# clean-all (removes cmake's cache)
###########
add_custom_target(clean-all
  COMMAND ${CMAKE_BUILD_TOOL} clean
  COMMAND ${CMAKE_COMMAND} -P ${CMAKE_SOURCE_DIR}/clean-all.cmake
  )


##############
# C++ Standard
##############
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_EXTENSIONS OFF)


##############
# Sources and Target Name
##############
set(src1    main.cpp)
set(target1 go)

##############
# Boost
##############
#set(Boost_USE_STATIC_LIBS        ON) # only find static libs
#set(Boost_USE_MULTITHREADED      ON)
#set(Boost_USE_STATIC_RUNTIME    OFF)
find_package(Boost COMPONENTS system )
if(Boost_FOUND)
  include_directories(${Boost_INCLUDE_DIRS})
  set(libs ${libs} ${Boost_LIBRARIES})
endif()


##############
# PThread
##############
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package (Threads)
set(libs ${libs} ${CMAKE_THREAD_LIBS_INIT})


##############
# Build and Link
##############
add_executable(${target1} ${src1})
target_link_libraries(${target1} ${libs})
