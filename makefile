CC=gcc
CFLAGS=-g -c -Wall -Werrpr
LDFLAGS=-g
SOURCES= wavesynth.c dds.c USB.c audio.c libs/json.c config/config.c
OBJECTS=$(SOURCES:.cpp=.0)
EXECUTABLE=wavesynth

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) -o $@ $(OBJECTS) -lpthread -lasound -lm
	
.c.o:
	$(CC) $(CFLAGS) $< -o $@
	
clean:
	rm *.o synth
