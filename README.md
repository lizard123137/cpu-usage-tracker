# *cpu-usage-tracker*

This is a Linux console application used for determining the CPU usage.

It is composed of:
- *Reader* - reads the /proc/stat directory
- *Analazer* - processes the data and sends it to Printer
- *Printer* - prints the formated data to the console
- *Watchdog* - makes sure that everything is running correctly
- *Logger* - logs the data to a file
It uses the pthread library to run these as seperate threads