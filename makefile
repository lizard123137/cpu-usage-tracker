PROJECT := tracker

CC := gcc
CFLAGS := -g -Wall -Wextra -pthread
#CC = clang
#CFLAGS = -Weverything

SRC := src

SRCS = $(wildcard $(SRC)/*.c)
HDRS = $(wildcard $(SRC)/*.h)
OBJS = reader.o analyzer.o printer.o cpu-tracker.o

$(PROJECT): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

$(OBJS): %.o: $(SRC)/%.c $(SRC)/%.h
	$(CC) $(CFLAGS) -c $<

.PHONY: clean
clean:
	rm -r *.o 
	rm tracker

.PHONY: help
help:
	@echo available make targets: clean