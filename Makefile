OBJS := $(addprefix bin/objs/,$(notdir $(patsubst %.c,%.o,$(wildcard src/*.c))))
CC=gcc
CFLAGS = -Wall -Wextra --pedantic -g -std=c11 -fsanitize=undefined,address -fanalyzer -D_USE_MATH_DEFINES \
        -Wstack-protector -fstack-protector -Wmissing-prototypes -Wmissing-declarations -Winline -Wpedantic -Wvla \
        -D_DEFAULT_SOURCE -Werror=incompatible-pointer-types
    
LDFLAGS = `pkg-config x11 --libs` -lGL -lGLU -lm

.PHONY: all clean

all: bin bin/objs bin/from_scratch

bin/from_scratch: $(OBJS)
	pushd bin/objs; $(CC) $(CFLAGS) $(LDFLAGS) $(^F) -o ../from_scratch; popd

bin/objs/%.o: src/%.c
	$(CC) $(CFLAGS) -c $^ -o $@

bin:
	mkdir bin

bin/objs:
	mkdir bin/objs

clean:
	rm -rvf bin
