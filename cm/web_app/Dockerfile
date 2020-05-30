FROM ubuntu:20.04

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && \
    apt-get -y install nodejs npm wget xvfb

RUN wget -q -O - https://dl-ssl.google.com/linux/linux_signing_key.pub | apt-key add - && \
    echo "deb http://dl.google.com/linux/chrome/deb/ stable main" > /etc/apt/sources.list.d/google.list && \
    apt-get update && \
    apt-get install -y google-chrome-stable

RUN apt -y autoclean && \
    apt -y autoremove && \
    apt clean && \
    rm -rf /var/lib/apt/lists/*

RUN groupadd --gid 1000 user && \
    useradd --uid 1000 --gid user --groups sudo --shell /bin/bash --create-home user && \
    echo "user:user" | chpasswd

ENV HOME /home/user

USER user

WORKDIR /home/user

RUN npm update -g && npm install --save @angular/cli

ENV PATH="${PATH}:/home/user/node_modules/.bin"
