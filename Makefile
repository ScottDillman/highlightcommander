win-pre:
	mkdir release
	cd release; cmake -G "NMake Makefiles" ..

win-build:
	cd release; nmake

win-clean:
	rmdir /q/s release