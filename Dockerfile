# Container to build the daemon on Docker with a GCC compiler for ARMv7.
# Use the docker_build.sh script for that.

FROM arm32v7/gcc:9.3
COPY . /usr/src
WORKDIR /usr/src
RUN bash ./build.sh
CMD ["sleep", "infinity"]