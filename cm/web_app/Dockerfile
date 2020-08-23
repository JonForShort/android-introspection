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

RUN groupadd --gid 1000 user && \
    useradd --uid 1000 --gid user --groups sudo --shell /bin/bash --create-home user && \
    echo "user:user" | chpasswd

RUN echo "user ALL=(root) NOPASSWD:ALL" > /etc/sudoers.d/user && \
    chmod 0440 /etc/sudoers.d/user

#
# Work-around for CM machine since it requires users to be added without root or sudo.
#
COPY cm/scripts /usr/local/bin
RUN chmod +x /usr/local/bin/*

ENV HOME /home/user

USER user

WORKDIR /home/user
