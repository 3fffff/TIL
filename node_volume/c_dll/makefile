#
# A simple makefile for compiling a c++ project
#
.DEFAULT_GOAL := cat

TITLE_NAME := TestProj

MD := mkdir
RM := rm
CC := g++

OUTPUT_ROOT := ./target/
SOURCES := \
        main.cpp \

all: clean test

##
cat:
	$(MD) -p $(OUTPUT_ROOT)
	g++ -O2 $(SOURCES) -o $(OUTPUT_ROOT)$(TITLE_NAME).dll -s -shared  -lole32 -lwinmm
###
run: dll.cpp
	$(OUTPUT_ROOT)$(TITLE_NAME).dll 10
###
test: dll.cpp
	$(OUTPUT_ROOT)$(TITLE_NAME).dll 10000
###
clean:
	rm -rf $(OUTPUT_ROOT)*.dll