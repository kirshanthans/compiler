LEX  = flex
YACC = bison
CC   = g++

CFLAGS = -c -Wno-write-strings -I./$(GENERATEDDIR) -I./$(SOURCEDIR)

SOURCEDIR    = src
BUILDDIR     = build
GENERATEDDIR = generated

SOURCE    = scanner parser compiler logger
HEAD      = parser compiler logger

HEADER = $(join $(addsuffix $(SOURCEDIR)/, $(dir $(HEAD))), $(notdir $(HEAD:=.h)))
OBJECT = $(join $(addsuffix $(BUILDDIR)/, $(dir $(SOURCE))), $(notdir $(SOURCE:=.o)))

EXECUTABLE = Micro

all:group compiler

group:
	@echo "Name: Kirshanthan Sundararajah ID: ksundar"
	@echo "Name: Laith Sakka ID:lsakka"

compiler: directory $(OBJECT) 
	$(CC) -o $(EXECUTABLE) $(OBJECT)

directory:
	mkdir -p $(BUILDDIR) $(GENERATEDDIR)

$(BUILDDIR)/%.o: $(SOURCEDIR)/%.cpp $(HEADER)
	$(CC) -o $@ $(CFLAGS) $<

$(BUILDDIR)/%.o: $(GENERATEDDIR)/%.c $(HEADER)
	$(CC) -o $@ $(CFLAGS) $<

$(GENERATEDDIR)/parser.c: $(SOURCEDIR)/parser.y $(SOURCEDIR)/parser.h 
	$(YACC) -v -o $@ $< --defines=$(GENERATEDDIR)/scanner.h

$(GENERATEDDIR)/scanner.c: $(SOURCEDIR)/scanner.l $(GENERATEDDIR)/parser.c
	$(LEX) -o $@ $<

clean:
	rm -rf Micro $(BUILDDIR) $(GENERATEDDIR)
