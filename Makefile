win-pre:
	cmd /c "mkdir release"
	cd release & cmake -G "Ninja" ..

win-build:
	cd release & nmake

win-clean:
	rmdir /q/s release