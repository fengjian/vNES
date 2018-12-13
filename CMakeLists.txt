include(CheckCXXCompilerFlag)

project(vNES)

cmake_minimum_required(VERSION 2.8.6)
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}")
if(MSVC)
#TODO

else(MSVC)
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=gnu99")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
  if(APPLE)
    set(CMAKE_XCODE_ATTRIBUTE_GCC_C_LANGUAGE_STANDARD "gnu99")
    set(CMAKE_XCODE_ATTRIBUTE_CLANG_CXX_LANGUAGE_STANDARD "c++11")
    set(CMAKE_XCODE_ATTRIBUTE_CLANG_CXX_LIBRARY "libc++")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
  endif()
  set(CMAKE_CXX_FLAGS_DEBUG          "-g -DDEBUG")
  set(CMAKE_CXX_FLAGS_MINSIZEREL     "-Os -DNDEBUG")
  set(CMAKE_CXX_FLAGS_RELEASE        "-O4 -DNDEBUG")
  set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O2 -g")
endif(MSVC)

if(APPLE)
  set(CMAKE_C_COMPILER clang)
  set(CMAKE_CXX_COMPILER clang++)
endif()


file(GLOB_RECURSE nes_src src/*.cpp)
add_executable(vNES ${nes_src})


CHECK_CXX_COMPILER_FLAG("-std=c++11" HAS_CXX11)
if(NOT HAS_CXX11)
  message("cxx11 no find")
endif()


file(GLOB SourceIgnoreFiles "${CMAKE_SOURCE_DIR}/*")
set(SourceKeepFiles "${CMAKE_SOURCE_DIR}/src"
                    "${CMAKE_SOURCE_DIR}/CMakeLists.txt")
list(REMOVE_ITEM SourceIgnoreFiles ${SourceKeepFiles})
# Escape any '.' characters
string(REPLACE "." "\\\\." SourceIgnoreFiles "${SourceIgnoreFiles}")

include(InstallRequiredSystemLibraries)
set(CPACK_SOURCE_IGNORE_FILES "${SourceIgnoreFiles}")
set(FMESH_VERSION_MAJOR 1)
set(FMESH_VERSION_MINOR 0)
set(CPACK_PACKAGE_VERSION_MAJOR "${FMESH_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${FMESH_VERSION_MINOR}")
if(APPLE)
  set(CPACK_BUNDLE_NAME "vNES")
  #todo
endif()

if(WIN32)
  #todo
endif()

set(CPACK_PACKAGE_NAME "vNES")
set(CPACK_SOURCE_GENERATOR "TGZ")
set(CPACK_GENERATOR "TGZ")
include(cpack)


set(CMAKE_INSTALL_PREFIX ${CMAKE_CURRENT_SOURCE_DIR})
install(TARGETS vNES RUNTIME DESTINATION bin)
