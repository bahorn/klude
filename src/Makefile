
build:
	mkdir -p ../artifacts
	make -C ./stage1 build
	make -C ./payload/ build
	make -C ./stage2 build
	python3 ./combine/build.py > ../artifacts/loader.py
	pyminify ../artifacts/loader.py > ../artifacts/loader-min.py
