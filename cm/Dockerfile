FROM ubuntu:20.04

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && \
    apt-get install -y build-essential openjdk-8-jdk wget unzip python git pkg-config && \
    apt -y autoclean && apt -y autoremove && apt clean

RUN useradd -ms /bin/bash user

USER user

WORKDIR /home/user

ENV ANDROID_SDK_ROOT /home/user/android-sdk
ENV ANDROID_SDK_HOME ${ANDROID_SDK_ROOT}
ENV ANDROID_HOME     ${ANDROID_SDK_ROOT}
ENV ANDROID_SDK      ${ANDROID_SDK_ROOT}
ENV ANDROID_NDK_HOME ${ANDROID_SDK_ROOT}/ndk
ENV ANDROID_NDK_ROOT ${ANDROID_SDK_ROOT}/ndk

RUN wget https://dl.google.com/android/repository/sdk-tools-linux-4333796.zip -O ${ANDROID_SDK_ROOT}.zip && \
    unzip ${ANDROID_SDK_ROOT}.zip -d ${ANDROID_SDK_ROOT}

ENV PATH ${PATH}:${ANDROID_SDK_ROOT}/tools/bin:${ANDROID_SDK_ROOT}/cmake/3.10.2.4988404/bin

RUN yes | sdkmanager --update && \
    yes | sdkmanager "platforms;android-28" "platform-tools" "build-tools;28.0.3" "ndk-bundle" "cmake;3.10.2.4988404"
