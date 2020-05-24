FROM ubuntu:20.04

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && \
    apt-get -y install nodejs npm && \
    apt -y autoclean && \
    apt -y autoremove && \
    apt clean

RUN groupadd --gid 1000 user && \
    useradd --uid 1000 --gid user --groups sudo --shell /bin/bash --create-home user && \
    echo "user:user" | chpasswd

ENV HOME /home/user

USER user

WORKDIR /home/user

RUN npm update -g && npm install --save @angular/cli
