win-pre:
	cmd /c "mkdir release"
	cd release & cmake -G "ninja" ..

win-build:
	cd release & nmake

win-clean:
	rmdir /q/s release