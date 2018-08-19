CC=g++
CFLAGS=-c -Wall
LDFLAGS= -lX11 -Wall
SOURCES=$(wildcard $(SRCDIR)/*.cpp)
INCLUDES=$(wildcard $(INCDIR)/*.h)
OBJECTS=$(SOURCES:$(SRCDIR)/%.cpp=$(OBJDIR)/%.o)
EXECUTABLE=shapes

SRCDIR = src
INCDIR = inc
OBJDIR = obj
BINDIR = bin

all: $(SOURCES) $(BINDIR)/$(EXECUTABLE) 

$(BINDIR)/$(EXECUTABLE): $(OBJECTS)
	$(CC) $(OBJECTS) $(LDFLAGS) -o $@
	@echo "Linking complete!"

$(OBJECTS): $(OBJDIR)/%.o : $(SRCDIR)/%.cpp $(INCDIR)
	$(CC) $(CFLAGS) $< -I $(word 2,$^) -o $@
	@echo "Compiled "$<" successfully!"

clean:
	rm -rf $(OBJECTS) $(BINDIR)/$(EXECUTABLE)
