OPT = -g0 -O3

LIB_SOURCES1 = microio.c
LIB_SOURCES = $(addprefix src/, $(LIB_SOURCES1))

CC = gcc
TARGET = libmicroio.so
LIB_OBJECTS =$(LIB_SOURCES:.c=.o)
LDFLAGS= -L. $(OPT) -Wall -Wextra  -shared
LIBS=-ldl
ALL= $(TARGET) libmicroio.a
CFLAGS = -Isrc/ -Iinclude/ -std=gnu11 -c $(OPT) -Wall  -Wextra -Werror=implicit-function-declaration -Wformat=0 -D_GNU_SOURCE -Wwrite-strings -Werror -Werror=maybe-uninitialized -fPIC

ifneq ($(BUILD),release)
    CFLAGS += -DDEBUG
    OPT = -g3 -O0
endif
all: $(ALL)

$(TARGET): $(LIB_OBJECTS)
	$(CC) $(LDFLAGS) $(LIB_OBJECTS) $(LIBS) -o $@

libmicroio.a: libmicroio.so
	ar rcs libmicroio.a $(LIB_OBJECTS)

release debug:
	$(MAKE) BUILD=$@



.c.o: $(HEADERS) $(LEVEL_CS)
	$(CC) $(CFLAGS) $< -o $@ -MMD -MF $@.depends

depend: h-depend
clean:
	rm -f $(LIB_OBJECTS) $(ALL) src/*.o.depends src/*.o

-include $(LIB_OBJECTS:.o=.o.depends)




