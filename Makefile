win-pre:
	cmd /c "mkdir release"
	cd release & cmake -Ax64 -G "Visual Studio 16 2019" ..

win-build:
	cd release & MSBuild /nologo /t:Build ALL_BUILD.vcxproj 

win-clean:
	rmdir /q/s release