CC      = cc
LDFLAGS = -lzstd
CFLAGS  =

all: zstd

zstd:
	$(CC) zstd.c -o $@ $(CFLAGS) $(LDFLAGS)

clean:
	rm zstd
