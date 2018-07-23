TARGET = lib

INCLUDE_DIRS = -I../../../include -I../../../../Framework/include -I/usr/local/include/opencv2

CXX = g++
CXXFLAGS += -O2 -DLINUX -Wall $(INCLUDE_DIRS) `pkg-config --cflags opencv`
# CXXFLAGS += -O2 -DLINUX -Wall $(INCLUDE_DIRS) `pkg-config --cflags opencv`
LFLAGS += -lpthread -ljpeg -lrt
LDFLAGS = `pkg-config --libs opencv`

OBJECTS =   lib.o

all: $(TARGET)

clean:
	rm -f *.a *.o $(TARGET) core *~ *.so *.lo
