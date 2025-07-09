CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -O2 -g -D_POSIX_C_SOURCE=200809L
LDFLAGS = -lwayland-server -lwlroots-0.19 -lxkbcommon -lpixman-1 -lm

SRCDIR = src
INCDIR = inc
OBJDIR = obj
BUILDDIR = build

SOURCES = $(wildcard $(SRCDIR)/*.c)
OBJECTS = $(SOURCES:$(SRCDIR)/%.c=$(OBJDIR)/%.o)
HEADERS = $(wildcard $(INCDIR)/*.h)

TARGET = $(BUILDDIR)/libwlcom.so
STATIC_TARGET = $(BUILDDIR)/libwlcom.a

EXAMPLE_SRC = example/compositor.c
EXAMPLE_TARGET = $(BUILDDIR)/example-compositor

PKGS = wayland-server wlr-protocols wlroots-0.19 xkbcommon pixman-1

CFLAGS += $(shell pkg-config --cflags $(PKGS))
LDFLAGS += $(shell pkg-config --libs $(PKGS))
CFLAGS += -I$(INCDIR)
CFLAGS += -fPIC
CFLAGS += -DWLR_USE_UNSTABLE

.PHONY: all clean install example

all: $(TARGET) $(STATIC_TARGET)

$(TARGET): $(OBJECTS) | $(BUILDDIR)
	$(CC) -shared -o $@ $^ $(LDFLAGS)

$(STATIC_TARGET): $(OBJECTS) | $(BUILDDIR)
	ar rcs $@ $^

$(OBJDIR)/%.o: $(SRCDIR)/%.c $(HEADERS) | $(OBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJDIR):
	mkdir -p $(OBJDIR)

$(BUILDDIR):
	mkdir -p $(BUILDDIR)

example: $(EXAMPLE_TARGET)

$(EXAMPLE_TARGET): $(EXAMPLE_SRC) $(TARGET) | $(BUILDDIR)
	$(CC) $(CFLAGS) -o $@ $< -L$(BUILDDIR) -lwlcom $(LDFLAGS)

clean:
	rm -rf $(OBJDIR) $(BUILDDIR)

install: $(TARGET) $(STATIC_TARGET)
	install -d $(DESTDIR)/usr/lib
	install -d $(DESTDIR)/usr/include/wlcom
	install -m 644 $(TARGET) $(DESTDIR)/usr/lib/
	install -m 644 $(STATIC_TARGET) $(DESTDIR)/usr/lib/
	install -m 644 $(INCDIR)/*.h $(DESTDIR)/usr/include/wlcom/

uninstall:
	rm -f $(DESTDIR)/usr/lib/libwlcom.so
	rm -f $(DESTDIR)/usr/lib/libwlcom.a
	rm -rf $(DESTDIR)/usr/include/wlcom

debug: CFLAGS += -DDEBUG -g3
debug: $(TARGET)

valgrind: debug
	valgrind --leak-check=full --show-leak-kinds=all $(EXAMPLE_TARGET)

format:
	find $(SRCDIR) $(INCDIR) -name "*.c" -o -name "*.h" | xargs clang-format -i

check:
	find $(SRCDIR) $(INCDIR) -name "*.c" -o -name "*.h" | xargs clang-tidy
