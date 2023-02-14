# CPU usage tracker

This is a **Linux** console application used for determining the CPU usage for each CPU cores.

It is composed of several threads:
- **Reader** - reads the /proc/stat directory
- **Analazer** - processes the data and sends it to Printer
- **Printer** - prints the formated data to the console
- **Watchdog** - makes sure that everything is running correctly
- **Logger** - logs the data to a file

It uses the pthread library to run these as seperate threads

# Build

The build system is based on makefile. In order to build the program navigate to the project folder and type:

```Terminal
make
```

By default it compiles using the GCC compiler.

 In order to change that go into makefile, comment out the existing CC and CFLAGS variables and uncomment the other ones.

```Makefile
CC = gcc
CFLAGS = -g -Wall -Wextra
# CC = clang
# CFLAGS = -Weverything
```

# Run

In order to run the built application navigate to the project folder and type:

```Terminal
./tracker
```

To close the application press **Ctrl+C** or **Ctrl+Z**

# Tests

This app has been tested for memory leaks using valgrind.
```
valgrind --leak-check=full --track-origins=yes ./tracker 

CPU USAGE FOR EACH CORE 

CPU CORE 1| 3%
CPU CORE 2| 5%
CPU CORE 3| 4%
CPU CORE 4| 4%
^C
 CLOSING THREADS...
==5096== 
==5096== HEAP SUMMARY:
==5096==     in use at exit: 0 bytes in 0 blocks
==5096==   total heap usage: 45 allocs, 45 frees, 20,222 bytes allocated
==5096== 
==5096== All heap blocks were freed -- no leaks are possible
```

# Technologies used

- C programming language
- GCC compiler
- pThreads library
- Makefile
- Valgrind