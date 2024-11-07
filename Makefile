CFLAGS += -Wall

.PHONY: all clean

all: dummy-server.bin

dummy-server.bin: main.c mongoose.c
	$(CC) $(CFLAGS) $^ -o $@ 

clean:
	$(RM) *.o *.bin

