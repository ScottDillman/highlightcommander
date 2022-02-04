win-pre:
	cmd /c "mkdir release"
	cd release & cmake -Ax64 -G "Ninja" ..

win-build:
	cd release & nmake

win-clean:
	rmdir /q/s release