PROJECT = 01
SYSTEM = linux
SRCDIR = src
BINDIR = bin
CC = gcc
CXX = g++
CFLAGS = -O2 -pedantic -Wall
IGNORE_UNUSED = -Wno-unused-variable
all: libpalindrome.o c3 s3 server4 client4

clean:
	@rm -rf $(BINDIR)/*.o \
	$(BINDIR)/client1 $(BINDIR)/server1 \
	$(BINDIR)/client2 $(BINDIR)/server2 \
	$(BINDIR)/client3 $(BINDIR)/server3 \
	$(BINDIR)/server4 $(BINDIR)/server5

distrib: clean
	tar -cvf $(SYSTEM)-$(PROJECT).tar $(SRCDIR)/* $(BINDIR)/img Makefile

client1: client1.o
	$(CC) $(CFLAGS) $(BINDIR)/$@.o -o $(BINDIR)/$@

client1.o: $(SRCDIR)/client1.c
	$(CC) $(CFLAGS) -c $(SRCDIR)/$(basename $@).c -o $(BINDIR)/$(basename $@).o

client2: client2.o
	$(CC) $(CFLAGS) $(BINDIR)/$@.o -o $(BINDIR)/$@

client2.o: $(SRCDIR)/client2.c
	$(CC) $(CFLAGS) -c $(SRCDIR)/$(basename $@).c -o $(BINDIR)/$(basename $@).o

client3: client3.o
	$(CC) $(CFLAGS) $(BINDIR)/$@.o -o $(BINDIR)/$@

client3.o: $(SRCDIR)/client3.c
	$(CC) $(CFLAGS) -c $(SRCDIR)/$(basename $@).c -o $(BINDIR)/$(basename $@).o

server1: server1.o
	$(CC) $(CFLAGS) $(BINDIR)/$@.o -o $(BINDIR)/$@

server1.o: $(SRCDIR)/server1.c
	$(CC) $(CFLAGS) -c $(SRCDIR)/$(basename $@).c -o $(BINDIR)/$(basename $@).o

server2: server2.o
	$(CC) $(CFLAGS) $(BINDIR)/$@.o -o $(BINDIR)/$@

server2.o: $(SRCDIR)/server2.c
	$(CC) $(CFLAGS) -c $(SRCDIR)/$(basename $@).c -o $(BINDIR)/$(basename $@).o

server3: server3.o libpalindrome.o
	$(CC) $(CFLAGS) $(BINDIR)/$@.o $(BINDIR)/libpalindrome.o -o $(BINDIR)/$@

server3.o: $(SRCDIR)/server3.c
	$(CC) $(CFLAGS) -c $(SRCDIR)/$(basename $@).c -o $(BINDIR)/$(basename $@).o

c3: c3.o
	$(CC) $(CFLAGS) $(BINDIR)/$@.o -o $(BINDIR)/$@

c3.o: $(SRCDIR)/client3.c
	$(CC) $(CFLAGS) -c $(SRCDIR)/$(basename $@).c -o $(BINDIR)/$(basename $@).o

s3: s3.o libpalindrome.o
	$(CC) $(CFLAGS) $(BINDIR)/$@.o $(BINDIR)/libpalindrome.o -o $(BINDIR)/$@

s3.o: $(SRCDIR)/server3.c
	$(CC) $(CFLAGS) -c $(SRCDIR)/$(basename $@).c -o $(BINDIR)/$(basename $@).o

server4: server4.o
	$(CXX) $(BINDIR)/$@.o -o $(BINDIR)/$@

server4.o: $(SRCDIR)/server4.cpp
	$(CXX) $(SRCDIR)/$(basename $@).cpp -o $(BINDIR)/$(basename $@).o

#server5: server5.o
#	$(CC) $(CFLAGS) $(BINDIR)/$@.o -o $(BINDIR)/$@

#server5.o: $(SRCDIR)/server5.c
#	$(CC) $(IGNORE_UNUSED) $(CFLAGS) -c $(SRCDIR)/$(basename $@).c -o $(BINDIR)/$(basename $@).o

libpalindrome.o: $(SRCDIR)/libpalindrome.c
	$(CC) $(IGNORE_UNUSED) $(CFLAGS) -c $(SRCDIR)/$(basename $@).c -o $(BINDIR)/$(basename $@).o
