############## Variables ###############

CC = gcc

IFLAGS   = -I/comp/40/build/include -I/usr/sup/cii40/include/cii
CFLAGS   = -g -std=gnu99 -O2 -Wall -Wextra -Werror -pedantic $(IFLAGS)
LDFLAGS  = -g -L/comp/40/build/lib -L/usr/sup/cii40/lib64
LDLIBS   = -lcii40-O2 -lm -lum-dis -lcii

INCLUDES = $(shell echo *.h)

EXECS    = um

############### Rules ###############

all: $(EXECS)

## Compile step (.c files -> .o files)

%.o: %.c $(INCLUDES)
	$(CC) $(CFLAGS) -c $< -o $@

## Linking step (.o -> executable program)

um: um.o
	$(CC) $(LDFLAGS) $^ -o $@ $(LDLIBS)

clean:
	rm -f $(EXECS)  *.o
