FROM android-introspection_web_app:latest

RUN groupadd --gid 1000 user && \
    useradd --uid 1000 --gid user --groups sudo --shell /bin/bash --create-home user && \
    echo "user:user" | chpasswd

RUN echo "user ALL=(root) NOPASSWD:ALL" > /etc/sudoers.d/user && \
    chmod 0440 /etc/sudoers.d/user

ENV HOME /home/user

USER user

WORKDIR /home/user
