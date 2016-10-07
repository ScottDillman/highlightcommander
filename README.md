HighlightCommander
=======

Syntax aware lister for double commander using André Simon's highlight library http://www.andre-simon.de/doku/highlight/en/highlight.php
Based on the original Highlight lister from Jens Theeß https://web.archive.org/web/20090508044550/http://www.theess.com/highlight

Highlight features can be found here: http://andre-simon.de/dokuwiki/doku.php?id=en:features

![Logo](doc/hilightcommander.png)

Features:
	- Lua based plugins
	- Syntax support for 200+ languages, add your own
	- Over 80+ themes, or make your own
	- Select and copy RTF to clipboard
	- Configure settings in yaml file
	
	example:
		
```
		theme: bitwiseninja.theme
		pagesize: letter
		rtfcharstyles: false
		wraplines: true
		includestyle: false
		rtfpagecolor: true
		printlinenumbers: true
		printzeros: true
		fragmentcode: false
		keepinjections: false
		linenumberwidth: 5
		linewidth: 80
		encoding: utf8
		basefont: FiraCode-Medium
		basefontsize: 8
		disabletrailingnl: false
		indentationscheme: allman
```
	
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
