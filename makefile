PROJECT = tracker

CC = gcc
CFLAGS = -g -Wall -Wextra
#CC = clang
#CFLAGS = -Weverything

LIBS = -pthread
CFILES = reader.o analyzer.o printer.o cpu-tracker.o

tracker: $(CFILES)
	$(CC) $(LIBS) $(CFLAGS) -o $@ $(CFILES)

$(CFILES): %.o: %.c %.h
cpu-tracker.o: cpu-tracker.c reader.h cpu-tracker.h

.PHONY: clean
clean:
	rm -f *.o $(PROJECT)

.PHONY: install
install:
	@echo no install tasks required

.PHONY: uninstall
uninstall:
	@echo no uninstall tasks required

.PHONY: help
help:
	@echo available make targets: clean