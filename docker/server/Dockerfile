# Runtime docker image
FROM ubuntu:26.04

ARG HOST_UID=1001
ARG HOST_GID=1001

# Create user

RUN existing_user="$(getent passwd "${HOST_UID}" | cut -d: -f1)" && \
    if [ -n "$existing_user" ]; then userdel -r "$existing_user" || userdel "$existing_user"; fi && \
    existing_group="$(getent group "${HOST_GID}" | cut -d: -f1)" && \
    if [ -n "$existing_group" ]; then groupdel "$existing_group"; fi && \
    groupadd -g "${HOST_GID}" x2 && \
    useradd -u "${HOST_UID}" -g x2 -m x2

ARG DEBIAN_FRONTEND=noninteractive
RUN apt-get update \
 && apt-get install -y --no-install-recommends \
        libodbc2 \
        tdsodbc \
 && rm -rf /var/lib/apt/lists/*

# Define default user
USER x2:x2
WORKDIR /opt/x2
COPY --chown=x2:x2 CenterServer/ /opt/x2/CenterServer/
COPY --chown=x2:x2 ChannelServer/ /opt/x2/ChannelServer/
COPY --chown=x2:x2 EPublisherServer/ /opt/x2/EPublisherServer/
COPY --chown=x2:x2 GameServer/ /opt/x2/GameServer/
COPY --chown=x2:x2 GlobalServer/ /opt/x2/GlobalServer/
COPY --chown=x2:x2 LoginServer/ /opt/x2/LoginServer/
