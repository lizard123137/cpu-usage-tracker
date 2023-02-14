# cpu-usage-tracker

This is a **Linux** console application used for determining the CPU usage for each CPU core.

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

# Technologies used

- C programming language
- GCC compiler
- pThreads library
- Makefile
- Valgrind