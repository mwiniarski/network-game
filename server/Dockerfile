FROM ubuntu:latest

RUN apt-get update
RUN apt-get install -y libfltk1.3-dev make g++
WORKDIR /app
COPY Makefile server.cpp /app/
RUN make

CMD /app/server.app

