FROM ubuntu:20.04

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && \
    apt-get install -y build-essential python cmake git pkg-config nodejs npm wget xvfb sudo

RUN wget -q -O - https://dl-ssl.google.com/linux/linux_signing_key.pub | apt-key add - && \
    echo "deb http://dl.google.com/linux/chrome/deb/ stable main" > /etc/apt/sources.list.d/google.list && \
    apt-get update && \
    apt-get install -y google-chrome-stable

RUN apt -y autoclean && \
    apt -y autoremove && \
    apt clean && \
    rm -rf /var/lib/apt/lists/*

WORKDIR /root

RUN npm update -g && npm install --save @angular/cli

ENV PATH="${PATH}:/root/node_modules/.bin"
