FROM ubuntu:20.04

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && \
    apt-get -y install nodejs npm && \
    apt -y autoclean && apt -y autoremove && apt clean

RUN useradd -ms /bin/bash user

USER user

WORKDIR /home/user

RUN npm update -g && npm install --save @angular/cli
