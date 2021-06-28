TARGET	= client
SRCS	= client.c connection.c select_cards.c search_cards.c evaluate_cards.c util.c
OBJS	= client.o connection.o select_cards.o search_cards.o evaluate_cards.o util.o

CC = gcc
LIBS = -lnsl   
CFLAGS = -g -O2

all: $(TARGET)

$(TARGET): $(OBJS) $(SRCS)
	$(CC) $(CFLAGS) -o $@ $(OBJS) $(LIBS)


Makefile: Makefile.in
	./configure

.c.o:
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f $(OBJS) $(TARGET)
