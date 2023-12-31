KVERSION=5.15.0-91-generic

build:
	docker build --build-arg="KVERSION=$(KVERSION)" -t bahorn/kernel-build:$(KVERSION) .
