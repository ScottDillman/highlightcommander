cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

set(CMAKE_GENERATOR "Ninja")

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    add_compile_options (-fdiagnostics-color=always)
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    add_compile_options (-fcolor-diagnostics)
endif ()

SET(CPACK_GENERATOR "ZIP")
SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Higlight Commander Lister Plugin")
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Scott Dillman")
SET(CPACK_PACKAGE_VENDOR "scott@bitwise.ninja")
SET(CPACK_PACKAGE_VERSION_MAJOR "1")
SET(CPACK_PACKAGE_VERSION_MINOR "0")
SET(CPACK_PACKAGE_VERSION_PATCH "1")
set(CPACK_PACKAGE_VERSION "1.0.1")
set(CPACK_RESOURCE_FILE_README "${CMAKE_SOURCE_DIR}/README.md")
SET(CPACK_STRIP_FILES TRUE)
set(CPACK_SOURCE_IGNORE_FILES "${CMAKE_SOURCE_DIR}/build/;${CMAKE_SOURCE_DIR}/.git/;")
set(CPACK_IGNORE_FILES "${CMAKE_SOURCE_DIR}/build/;${CMAKE_SOURCE_DIR}/.git/;")

set(CMAKE_VERBOSE_MAKEFILE OFF)
set(CMAKE_COLOR_MAKEFILE   ON)

project (highlightcommander)

add_subdirectory (yaml-cpp)

include_directories(".")

include_directories("./highlight/src/include/")
include_directories("./sdk")
include_directories("./yaml-cpp/include")

include_directories("/usr/include/qt/QtWidgets")
include_directories("/usr/include/qt/")
include_directories("/usr/include/qt/QtCore")
include_directories("/usr/include/qt/QtGui")
include_directories("./highlight/src/include")
include_directories("./yaml-cpp/include")

## lua support
add_library(Diluculum OBJECT ./highlight/src/core/Diluculum/InternalUtils.cpp ./highlight/src/core/Diluculum/LuaExceptions.cpp ./highlight/src/core/Diluculum/LuaFunction.cpp ./highlight/src/core/Diluculum/LuaState.cpp ./highlight/src/core/Diluculum/LuaUserData.cpp ./highlight/src/core/Diluculum/LuaUtils.cpp ./highlight/src/core/Diluculum/LuaValue.cpp ./highlight/src/core/Diluculum/LuaVariable.cpp ./highlight/src/core/Diluculum/LuaWrappers.cpp)

# yaml support
add_library(yaml OBJECT ./yaml-cpp/src/binary.cpp ./yaml-cpp/src/convert.cpp ./yaml-cpp/src/depthguard.cpp ./yaml-cpp/src/directives.cpp ./yaml-cpp/src/emit.cpp ./yaml-cpp/src/emitfromevents.cpp ./yaml-cpp/src/emitter.cpp ./yaml-cpp/src/emitterstate.cpp ./yaml-cpp/src/emitterutils.cpp ./yaml-cpp/src/exceptions.cpp ./yaml-cpp/src/exp.cpp ./yaml-cpp/src/memory.cpp ./yaml-cpp/src/nodebuilder.cpp ./yaml-cpp/src/node.cpp ./yaml-cpp/src/node_data.cpp ./yaml-cpp/src/nodeevents.cpp ./yaml-cpp/src/null.cpp ./yaml-cpp/src/ostream_wrapper.cpp ./yaml-cpp/src/parse.cpp ./yaml-cpp/src/parser.cpp ./yaml-cpp/src/regex_yaml.cpp ./yaml-cpp/src/scanner.cpp ./yaml-cpp/src/scanscalar.cpp ./yaml-cpp/src/scantag.cpp ./yaml-cpp/src/scantoken.cpp ./yaml-cpp/src/simplekey.cpp ./yaml-cpp/src/singledocparser.cpp ./yaml-cpp/src/stream.cpp ./yaml-cpp/src/tag.cpp)

## astyle support
add_library(astyle OBJECT ./highlight/src/core/astyle/ASBeautifier.cpp ./highlight/src/core/astyle/ASEnhancer.cpp ./highlight/src/core/astyle/ASFormatter.cpp ./highlight/src/core/astyle/ASResource.cpp ./highlight/src/core/astyle/ASStreamIterator.cpp)

# highlight core
add_library(core OBJECT ./highlight/src/core/ansigenerator.cpp ./highlight/src/core/bbcodegenerator.cpp ./highlight/src/core/codegenerator.cpp ./highlight/src/core/datadir.cpp ./highlight/src/core/elementstyle.cpp ./highlight/src/core/htmlgenerator.cpp ./highlight/src/core/keystore.cpp ./highlight/src/core/latexgenerator.cpp ./highlight/src/core/lspclient.cpp ./highlight/src/core/odtgenerator.cpp ./highlight/src/core/pangogenerator.cpp ./highlight/src/core/platform_fs.cpp ./highlight/src/core/preformatter.cpp ./highlight/src/core/rtfgenerator.cpp ./highlight/src/core/stringtools.cpp ./highlight/src/core/stylecolour.cpp ./highlight/src/core/svggenerator.cpp ./highlight/src/core/syntaxreader.cpp ./highlight/src/core/texgenerator.cpp ./highlight/src/core/themereader.cpp ./highlight/src/core/xhtmlgenerator.cpp ./highlight/src/core/xterm256generator.cpp)

add_library(app OBJECT src/hc_wlx.cpp)

## main wlx plugin
add_library(highlightcommander MODULE src/plugin.cpp)

target_compile_options(highlightcommander PUBLIC -std=c++17 -Wall -O2 -fdiagnostics-color=always)

IF (WIN32)
 target_link_libraries (highlightcommander comdlg32 gdi32 comdlg32 ryml Diluculum astyle core app)
ELSE()
  target_link_libraries (highlightcommander lua yaml Diluculum astyle core app)
ENDIF()

set_target_properties(highlightcommander PROPERTIES SUFFIX ".wlx")
set_target_properties(highlightcommander PROPERTIES PREFIX "")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC -static-libgcc -static-libstdc++ -Wl,-Bstatic -lstdc++ -lpthread -Wl,-Bdynamic,--no-as-needed")

install(DIRECTORY "${PROJECT_SOURCE_DIR}/dist/" DESTINATION ".")
install(TARGETS highlightcommander DESTINATION ".")

INCLUDE(CPack)
