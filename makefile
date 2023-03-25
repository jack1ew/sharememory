# Copyright 2023 CSCE 311

cc = g++  # use g++ compiler
path = ..
standard = c++17

flags = -std=$(standard)  # compile with standard
flags += -Wall  # compile with all warnings
flags += -g  # include gdb instrumentation
flags += -I $(path)  # add parent to preprocessor include path

link = $(cc) $(flags) -o  # final linked build to binary executable

compile = $(cc) $(flags) -c  # compilation to intermediary .o files


# executables
#

all: text-server text-client

unix-domain-style: src/text-client.cc src/text-server.cc src/client.cc src/server.cc inc/client.h inc/server.h inc/unix_domain_sockets.h
	cpplint --root=$(path) $^

text-client: text-client.cc
	$(link) $@ $^

text-server: text-server.cc
	$(link) $@ $^

style: inc/*.h src/*.cpp
	cpplint --root=$(path) $^

clean:
	$(RM) bin/*.o text-client text-server
