
cmake_minimum_required(VERSION 2.8 FATAL_ERROR)


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
include_directories("./highlight/include")
include_directories("./yaml-cpp/include")

add_library(highlightcommander MODULE listplug.cpp hc_wlx.cpp hc_wlx.h listplug.h resource.h cmdlineoptions.h cmdlineoptions.cpp help.h help.cpp arg_parser.cc arg_parser.h ./highlight/core/ansigenerator.cpp ./highlight/core/bbcodegenerator.cpp ./highlight/core/codegenerator.cpp ./highlight/core/datadir.cpp ./highlight/core/elementstyle.cpp ./highlight/core/htmlgenerator.cpp ./highlight/core/latexgenerator.cpp ./highlight/core/odtgenerator.cpp ./highlight/core/pangogenerator.cpp ./highlight/core/platform_fs.cpp ./highlight/core/preformatter.cpp ./highlight/core/rtfgenerator.cpp ./highlight/core/stringtools.cpp ./highlight/core/stylecolour.cpp ./highlight/core/svggenerator.cpp ./highlight/core/syntaxreader.cpp ./highlight/core/texgenerator.cpp ./highlight/core/themereader.cpp ./highlight/core/xhtmlgenerator.cpp ./highlight/core/xterm256generator.cpp ./highlight/core/Diluculum/InternalUtils.cpp ./highlight/core/Diluculum/LuaExceptions.cpp ./highlight/core/Diluculum/LuaFunction.cpp ./highlight/core/Diluculum/LuaState.cpp ./highlight/core/Diluculum/LuaUserData.cpp ./highlight/core/Diluculum/LuaUtils.cpp ./highlight/core/Diluculum/LuaValue.cpp ./highlight/core/Diluculum/LuaVariable.cpp ./highlight/core/Diluculum/LuaWrappers.cpp ./highlight/core/astyle/ASBeautifier.cpp ./highlight/core/astyle/ASEnhancer.cpp ./highlight/core/astyle/ASFormatter.cpp ./highlight/core/astyle/ASResource.cpp ./highlight/core/astyle/ASStreamIterator.cpp ./highlight/include/ansigenerator.h ./highlight/include/bbcodegenerator.h ./highlight/include/charcodes.h ./highlight/include/codegenerator.h ./highlight/include/datadir.h ./highlight/include/elementstyle.h ./highlight/include/enums.h ./highlight/include/htmlgenerator.h ./highlight/include/latexgenerator.h ./highlight/include/odtgenerator.h ./highlight/include/pangogenerator.h ./highlight/include/platform_fs.h ./highlight/include/preformatter.h ./highlight/include/rtfgenerator.h ./highlight/include/stringtools.h ./highlight/include/stylecolour.h ./highlight/include/svggenerator.h ./highlight/include/syntaxreader.h ./highlight/include/texgenerator.h ./highlight/include/themereader.h ./highlight/include/version.h ./highlight/include/xhtmlgenerator.h ./highlight/include/xterm256generator.h ./highlight/include/astyle/ASStreamIterator.h ./highlight/include/astyle/astyle.h)

#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -optl-static -static -static-libgcc -static-libstdc++")

target_compile_options(highlightcommander PUBLIC -std=c++11 -Wall -O2 )
target_link_libraries (highlightcommander yaml-cpp comdlg32 gdi32 comdlg32 lua)
set_target_properties(highlightcommander PROPERTIES SUFFIX ".wlx64")
set_target_properties(highlightcommander PROPERTIES PREFIX "")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static-libgcc -static-libstdc++ -Wl,-Bstatic -lstdc++ -lpthread -Wl,-Bdynamic")

install(DIRECTORY "${PROJECT_SOURCE_DIR}/dist/" DESTINATION ".")
install(TARGETS highlightcommander DESTINATION ".")

INCLUDE(CPack)
