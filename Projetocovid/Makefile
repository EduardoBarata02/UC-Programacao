PROJECT = covid19 #file target's name

#Compiler to be used
CC = gcc
#Flags to compile with
CFLAGS = -Wall -std=c11 -O3

#OBJECTS
OBJECTS = commandline.o dataprint.o datares.o memoryclear.o read.o sort.o datasel.o

all: $(PROJECT)

$(PROJECT): $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -o $(PROJECT)

commandline.o: commandline.c header.h
dataprint.o: dataprint.c header.h
datares.o: datares.c header.h
datasel.o: datasel.c header.h
memoryclear.o: memoryclear.c header.h
read.o: read.c header.h
sort.o: sort.c header.h

#"Make clean"
clean:
	rm -f  *.o $(PROJECT)
