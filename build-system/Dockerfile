FROM ubuntu:22.04

ARG KVERSION=5.15.0-91-generic

RUN apt update && \
    apt -y upgrade && \
    apt install -y build-essential python3-pip linux-headers-$KVERSION xxd && \
    pip install jinja2 pyelftools python-minifier

RUN mkdir -p /build /workdir

CMD cp -r /build/src /workdir/src && cd /workdir && make -C src
