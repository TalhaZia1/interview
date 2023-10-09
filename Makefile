# Define the compiler and compiler flags
CC = gcc
CFLAGS = -Wall -O2 -Wunused-variable
LDFLAGS = -lsocketcan  # Add the linker flag for socketcan library

# Define the source file and the output executable name
SOURCE = sensorMock.c
OUTPUT = sensor.exe

# The default target is to build and run the program
all: $(OUTPUT)

# Build the executable
$(OUTPUT): $(SOURCE)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $(OUTPUT) $(SOURCE)  # Include LDFLAGS here

# Run the executable
run: all
	./$(OUTPUT)

# Clean up the generated files
clean:
	rm -f $(OUTPUT)

# PHONY targets don't represent files
.PHONY: all run clean
