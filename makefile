# Compiler
CC = gcc

# Compiler flags
CFLAGS = -Wall 

# Source files
SRCS = main.c commands.c

# Header files
HDRS = commands.h

# Object files
OBJS = $(SRCS:.c=.o)

# Target executable
TARGET = rush

# Default target
all: $(TARGET)

# Rule to build the target executable
$(TARGET):	$(OBJS)
	$(CC)	$(CFLAGS)	-o	$(TARGET)	$(OBJS)

# Rule to build object files
%.o: %.c $(HDRS)
	$(CC)	$(CFLAGS)	-c	$<	-o	$@

# Clean up build files
clean:
	rm	-f	$(OBJS)	$(TARGET)

# Phony targets
.PHONY: all clean