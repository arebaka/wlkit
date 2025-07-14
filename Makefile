CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++20 -O2 -g
LDFLAGS = -lwayland-server -lwlroots-0.19 -lxkbcommon -lpixman-1 -lm

SRCDIR = src
INCDIR = inc
OBJDIR = obj
BUILDDIR = build

SOURCES = $(wildcard $(SRCDIR)/*.cpp)
OBJECTS = $(SOURCES:$(SRCDIR)/%.cpp=$(OBJDIR)/%.o)
HEADERS = $(wildcard $(INCDIR)/*.hpp $(INCDIR)/device/*.hpp)

TARGET = $(BUILDDIR)/libwlkit.so
STATIC_TARGET = $(BUILDDIR)/libwlkit.a

EXAMPLE_SRC = test/compositor.cpp
EXAMPLE_TARGET = $(BUILDDIR)/test-compositor

PKGS = wayland-server wlr-protocols wlroots-0.19 xkbcommon pixman-1

CXXFLAGS += $(shell pkg-config --cflags $(PKGS))
LDFLAGS += $(shell pkg-config --libs $(PKGS))
CXXFLAGS += -I$(INCDIR)
CXXFLAGS += -fPIC
CXXFLAGS += -DWLR_USE_UNSTABLE
CXXFLAGS += -D_POSIX_C_SOURCE=200809L

.PHONY: all clean install uninstall example debug valgrind format check

all: $(TARGET) $(STATIC_TARGET)

$(TARGET): $(OBJECTS) | $(BUILDDIR)
	$(CXX) -shared -o $@ $^ $(LDFLAGS)

$(STATIC_TARGET): $(OBJECTS) | $(BUILDDIR)
	ar rcs $@ $^

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp $(HEADERS) | $(OBJDIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJDIR):
	mkdir -p $(OBJDIR)

$(BUILDDIR):
	mkdir -p $(BUILDDIR)

example: $(EXAMPLE_TARGET)

$(EXAMPLE_TARGET): $(EXAMPLE_SRC) $(TARGET) | $(BUILDDIR)
	$(CXX) $(CXXFLAGS) -o $@ $< -L$(BUILDDIR) -lwlkit $(LDFLAGS)

clean:
	rm -rf $(OBJDIR) $(BUILDDIR)

install: $(TARGET) $(STATIC_TARGET)
	install -d $(DESTDIR)/usr/lib
	install -d $(DESTDIR)/usr/include/wlkit
	install -d $(DESTDIR)/usr/include/wlkit/device
	install -m 644 $(TARGET) $(DESTDIR)/usr/lib/
	install -m 644 $(STATIC_TARGET) $(DESTDIR)/usr/lib/
	install -m 644 $(INCDIR)/*.hpp $(DESTDIR)/usr/include/wlkit/
	install -m 644 $(INCDIR)/device/*.hpp $(DESTDIR)/usr/include/wlkit/device/

uninstall:
	rm -f $(DESTDIR)/usr/lib/libwlkit.so
	rm -f $(DESTDIR)/usr/lib/libwlkit.a
	rm -rf $(DESTDIR)/usr/include/wlkit

debug: CXXFLAGS += -DDEBUG -g3
debug: $(TARGET)

valgrind: debug
	valgrind --leak-check=full --show-leak-kinds=all $(EXAMPLE_TARGET)

format:
	find $(SRCDIR) $(INCDIR) -name "*.cpp" -o -name "*.hpp" | xargs clang-format -i

check:
	find $(SRCDIR) $(INCDIR) -name "*.cpp" -o -name "*.hpp" | xargs clang-tidy
