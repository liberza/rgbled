CC = gcc
CFLAGS = -Wall -g -I ../
TARGET1 = threadtest
OBJS1 = threadtest.o
TARGET2 = autotest
OBJS2 = autotest.o
all: threadtest autotest

$(TARGET1): $(OBJS1)
	$(CC) -o $(TARGET1) $(CFLAGS) $(OBJS1) 

$(TARGET2): $(OBJS2)
	$(CC) -o $(TARGET2) $(CFLAGS) $(OBJS2)

clean:
	$(RM) $(TARGET1) $(TARGET2) $(OBJS1) $(OBJS2)
