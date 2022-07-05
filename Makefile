SHELL=cmd.exe

TARGET=task.exe

RC_DIR = ./
OBJ_DIR = ./
INC_DIR = ./

CPP_SRC = $(wildcard *.cpp)
OBJECTS = $(CPP_SRC:.cpp=.o)

# expect gnu g++
CC=@c++
LD=@c++
RM=@-del /q 2>NUL
AS=@ml64.exe 2>NUL

CCFLAGS = -c
DBG_CCFLAGS = -DDEBUG -g
RLS_CCFLAGS = -s -fdata-sections -ffunction-sections -O3
RLS_LDFLAGS = -Wl,--gc-sections,-s

%.o: %.cpp
	@echo CC $<
	$(CC) $(CCFLAGS) -o $@ $<

all: CCFLAGS += $(DBG_CCFLAGS)
all: $(TARGET) 

release: CCFLAGS += RLS_CCFLAGS
release: LDFLAGS += RLS_LDFLAGS
release: $(TARGET)

$(TARGET): $(OBJECTS)
	@echo LD $@
	$(LD) $(LDFLAGS) -o $@ $(LDLIBS) $^

.PHONY: clean
clean:
	$(RM) $(OBJECTS)	

