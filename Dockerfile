# Base image of 42 OS
FROM    ubuntu:20.04

# Update system
RUN     apt update && apt upgrade -y

# Time Zone Configuration (avoiding valgrind prompt)
ENV DEBIAN_FRONTEND=noninteractive
RUN apt-get update && apt-get install -y tzdata \
    && ln -fs /usr/share/zoneinfo/Etc/UTC /etc/localtime \
    && dpkg-reconfigure --frontend noninteractive tzdata


# Install necessary packages
RUN     apt install -y \
        build-essential \
        clang \
        valgrind \
        gdb \
        git

# User configuration
ARG USERNAME=devuser
ARG USER_UID=1000
ARG USER_GID=$USER_UID
RUN groupadd --gid $USER_GID $USERNAME \
    && useradd -s /bin/bash --uid $USER_UID --gid $USER_GID -m $USERNAME

# Set user as default user
USER $USERNAME

# Set working directory
WORKDIR /workspace

# Expose port to host
EXPOSE 5051

# Command to run
CMD [ "bash" ]