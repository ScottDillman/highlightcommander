cmake_minimum_required(VERSION 3.20 FATAL_ERROR)

IF (WIN32)

set(VCPKG_DIR "C:/vcpkg/installed/x64-windows")
set(VCPKG_TARGET_TRIPLET "x64-windows" CACHE STRING "")
set(CMAKE_TOOLCHAIN_FILE "C:/vcpkg/scripts/buildsystems/vcpkg.cmake" CACHE STRING "")
## set(CMAKE_GENERATOR_PLATFORM "x64")      

ENDIF()

project (highlightcommander)

## defaults
set(CMAKE_GENERATOR "Ninja")
set(CMAKE_BUILD_TYPE "MinSizeRel")

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    add_compile_options (-fdiagnostics-color=always)
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    add_compile_options (-fcolor-diagnostics)
endif ()

## package settings
IF (WIN32)
 set(CPACK_SOURCE_GENERATOR "ZIP")
 set(CPACK_GENERATOR "ZIP")
 set(CPACK_SYSTEM_NAME "Windows-amd64")
 set(CPACK_TOPLEVEL_TAG "Windows-amd64")
ELSE()
  set(CPACK_SOURCE_GENERATOR "TGZ")
  set(CPACK_GENERATOR "TGZ")
  set(CPACK_SYSTEM_NAME "Linux-amd64")
  set(CPACK_TOPLEVEL_TAG "Linux-amd64")
ENDIF()

set(CPACK_PACKAGE_DIRECTORY "highlightcommander")
set(CPACK_INCLUDE_TOPLEVEL_DIRECTORY OFF)

set(CPACK_PACKAGE_NAME "highlightcommander")
SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Higlight Commander Lister Plugin")
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Scott Dillman")
SET(CPACK_PACKAGE_VENDOR "scott@bitwise.ninja")
SET(CPACK_PACKAGE_VERSION_MAJOR "1")
SET(CPACK_PACKAGE_VERSION_MINOR "0")
SET(CPACK_PACKAGE_VERSION_PATCH "2")
set(CPACK_PACKAGE_VERSION "1.0.2")
set(CPACK_SOURCE_PACKAGE_FILE_NAME "highlightcommander")
set(CPACK_PACKAGE_INSTALL_DIRECTORY "highlightcommander")
set(CPACK_RESOURCE_FILE_README "${CMAKE_SOURCE_DIR}/README.md")
SET(CPACK_STRIP_FILES TRUE)
set(CPACK_SOURCE_IGNORE_FILES "${CMAKE_SOURCE_DIR}/build/;${CMAKE_SOURCE_DIR}/.git/;")
set(CPACK_IGNORE_FILES "${CMAKE_SOURCE_DIR}/build/;${CMAKE_SOURCE_DIR}/.git/;")

## packaging
INCLUDE(CPack)

set(CMAKE_VERBOSE_MAKEFILE OFF)
set(CMAKE_COLOR_MAKEFILE   ON)

## yaml support
IF (WIN32)

ELSE()
add_subdirectory (yaml-cpp)
include_directories("./yaml-cpp/include")
include_directories("/usr/include/qt/QtWidgets")
include_directories("/usr/include/qt/")
include_directories("/usr/include/qt/QtCore")
include_directories("/usr/include/qt/QtGui")
include_directories("./highlight/src/include")
# yaml support
add_library(yaml OBJECT ./yaml-cpp/src/binary.cpp ./yaml-cpp/src/convert.cpp ./yaml-cpp/src/depthguard.cpp ./yaml-cpp/src/directives.cpp ./yaml-cpp/src/emit.cpp ./yaml-cpp/src/emitfromevents.cpp ./yaml-cpp/src/emitter.cpp ./yaml-cpp/src/emitterstate.cpp ./yaml-cpp/src/emitterutils.cpp ./yaml-cpp/src/exceptions.cpp ./yaml-cpp/src/exp.cpp ./yaml-cpp/src/memory.cpp ./yaml-cpp/src/nodebuilder.cpp ./yaml-cpp/src/node.cpp ./yaml-cpp/src/node_data.cpp ./yaml-cpp/src/nodeevents.cpp ./yaml-cpp/src/null.cpp ./yaml-cpp/src/ostream_wrapper.cpp ./yaml-cpp/src/parse.cpp ./yaml-cpp/src/parser.cpp ./yaml-cpp/src/regex_yaml.cpp ./yaml-cpp/src/scanner.cpp ./yaml-cpp/src/scanscalar.cpp ./yaml-cpp/src/scantag.cpp ./yaml-cpp/src/scantoken.cpp ./yaml-cpp/src/simplekey.cpp ./yaml-cpp/src/singledocparser.cpp ./yaml-cpp/src/stream.cpp ./yaml-cpp/src/tag.cpp)

ENDIF()

## directories
include_directories(".")
include_directories("./highlight/src/include/")
include_directories("./sdk")


## lua support
add_library(Diluculum OBJECT ./highlight/src/core/Diluculum/InternalUtils.cpp ./highlight/src/core/Diluculum/LuaExceptions.cpp ./highlight/src/core/Diluculum/LuaFunction.cpp ./highlight/src/core/Diluculum/LuaState.cpp ./highlight/src/core/Diluculum/LuaUserData.cpp ./highlight/src/core/Diluculum/LuaUtils.cpp ./highlight/src/core/Diluculum/LuaValue.cpp ./highlight/src/core/Diluculum/LuaVariable.cpp ./highlight/src/core/Diluculum/LuaWrappers.cpp)


## astyle support
add_library(astyle OBJECT ./highlight/src/core/astyle/ASBeautifier.cpp ./highlight/src/core/astyle/ASEnhancer.cpp ./highlight/src/core/astyle/ASFormatter.cpp ./highlight/src/core/astyle/ASResource.cpp ./highlight/src/core/astyle/ASStreamIterator.cpp)

# highlight core
add_library(core OBJECT ./highlight/src/core/ansigenerator.cpp ./highlight/src/core/bbcodegenerator.cpp ./highlight/src/core/codegenerator.cpp ./highlight/src/core/datadir.cpp ./highlight/src/core/elementstyle.cpp ./highlight/src/core/htmlgenerator.cpp ./highlight/src/core/keystore.cpp ./highlight/src/core/latexgenerator.cpp ./highlight/src/core/lspclient.cpp ./highlight/src/core/odtgenerator.cpp ./highlight/src/core/pangogenerator.cpp ./highlight/src/core/platform_fs.cpp ./highlight/src/core/preformatter.cpp ./highlight/src/core/rtfgenerator.cpp ./highlight/src/core/stringtools.cpp ./highlight/src/core/stylecolour.cpp ./highlight/src/core/svggenerator.cpp ./highlight/src/core/syntaxreader.cpp ./highlight/src/core/texgenerator.cpp ./highlight/src/core/themereader.cpp ./highlight/src/core/xhtmlgenerator.cpp ./highlight/src/core/xterm256generator.cpp)

## highlight class
add_library(app OBJECT src/hc_wlx.cpp)

## main wlx plugin
add_library(highlightcommander MODULE src/plugin.cpp)

IF (WIN32)
find_package(Lua REQUIRED)


find_package(Qt5 COMPONENTS Core Widgets CONFIG REQUIRED)
if (${Qt5Core_FOUND})
    message("Found Qt " ${Qt5_VERSION})
else()
    message("Couldn't find Qt")
endif()

find_package(yaml-cpp 0.7 REQUIRED)

#find_package(Qt5Core REQUIRED)
#find_package(Qt5Gui REQUIRED)
#find_package(Qt5Widgets REQUIRED)

include_directories(${LUA_INCLUDE_DIR})
include_directories(${YAML_CPP_INCLUDE_DIRS})

# target_link_libraries(main PRIVATE yaml-cpp)

## target_include_directories(highlightcommander ${LUA_INCLUDE_DIR})
## target_link_libraries(highlightcommander PRIVATE ${LUA_LIBRARIES})
ENDIF()

## compiler options
IF (WIN32)
target_compile_options(highlightcommander PUBLIC /std:c++17 /Wall /O2)
ELSE()
target_compile_options(highlightcommander PUBLIC -std=c++17 -Wall -O2 -fdiagnostics-color=always)
ENDIF()

IF (WIN32)
 target_link_libraries (highlightcommander ${LUA_LIBRARIES} comdlg32 gdi32 comdlg32 ${YAML_CPP_LIBRARIES} Diluculum astyle core app)
ELSE()
  target_link_libraries (highlightcommander lua yaml Diluculum astyle core app)
ENDIF()

## target properties
set_target_properties(highlightcommander PROPERTIES SUFFIX ".wlx")
set_target_properties(highlightcommander PROPERTIES PREFIX "")

## linker options
IF (WIN32)
# find_package(yaml-cpp CONFIG REQUIRED)
ELSE()
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC -static-libgcc -static-libstdc++ -Wl,-Bstatic -lstdc++ -lpthread -Wl,-Bdynamic,--no-as-needed")
ENDIF()

## main files
install(DIRECTORY "${PROJECT_SOURCE_DIR}/dist/" DESTINATION ".")
## themes
install(DIRECTORY "${PROJECT_SOURCE_DIR}/highlight/themes" DESTINATION ".")
## syntax definitions
install(DIRECTORY "${PROJECT_SOURCE_DIR}/highlight/langDefs" DESTINATION ".")
## LUA plugin scripts
install(DIRECTORY "${PROJECT_SOURCE_DIR}/highlight/plugins" DESTINATION ".")
## filetype definitions
install(FILES "${PROJECT_SOURCE_DIR}/highlight/filetypes.conf" DESTINATION ".")
## target
install(TARGETS highlightcommander DESTINATION ".")


