#!/bin/sh
mkdir -p artifacts && \
    docker run \
        --rm \
        -v `pwd`/src:/build/src:ro \
        -v `pwd`/artifacts:/workdir/artifacts:rw \
        -e KVERSION=$1 \
        bahorn/kernel-build:$1 && \
    scp artifacts/loader-min.py a@$2:~/loader.py
