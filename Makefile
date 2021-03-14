OBJS := $(addprefix bin/objs/,$(notdir $(patsubst %.c,%.o,$(wildcard src/*.c))))
CFLAGS = -Wall -Wextra --pedantic -g
LDFLAGS = `pkg-config x11 --libs` -lGL -lGLU

.PHONY: all clean

all: bin bin/objs bin/from_scratch

bin/from_scratch: $(OBJS)
	pushd bin/objs; $(CC) $(LDFLAGS) $(^F) -o ../from_scratch; popd

bin/objs/%.o: src/%.c
	$(CC) $(CFLAGS) -c $^ -o $@

bin:
	mkdir bin

bin/objs:
	mkdir bin/objs

clean:
	rm -rvf bin
