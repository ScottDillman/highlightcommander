HighlightCommander
=======

Syntax aware lister for double commander using André Simon's highlight library http://www.andre-simon.de/doku/highlight/en/highlight.php
Based on the original Highlight lister from Jens Theeß https://web.archive.org/web/20090508044550/http://www.theess.com/highlight

Highlight features can be found here: http://andre-simon.de/dokuwiki/doku.php?id=en:features

![Logo](doc/hilightcommander.png)


I have current releases for Windows 64.

Installing:
	unzip achive into wlx plugins directory
	
	See http://andre-simon.de/dokuwiki/doku.php for highlight usage

Build requirements

	-Windows
		- CMake
		- MSYS2 and Mingw-w64

Building

	1. clone repositiory
	2. cd into cloned repository root
	3. mkdir build
	4. cd build
	3. Generate project files using cmake
		- cmake -G "MinGW Makefiles"
	4. build package: make package
