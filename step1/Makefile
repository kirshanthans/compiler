LEX = flex
CC  = g++

CFLAGS = -c 

SOURCEDIR    = src
BUILDDIR     = build
GENERATEDDIR = generated

CSOURCE    = scanner.c
CPPSOURCE  = compiler.cpp logger.cpp

HEADER     = $(join $(addsuffix $(SOURCEDIR)/, $(dir $(CSOURCE))), $(notdir $(CSOURCE:.c=.h)))
HEADER     += $(join $(addsuffix $(SOURCEDIR)/, $(dir $(CPPSOURCE))), $(notdir $(CPPSOURCE:.cpp=.h)))

OBJECT     = $(join $(addsuffix $(BUILDDIR)/, $(dir $(CSOURCE))), $(notdir $(CSOURCE:.c=.o)))
OBJECT     += $(join $(addsuffix $(BUILDDIR)/, $(dir $(CPPSOURCE))), $(notdir $(CPPSOURCE:.cpp=.o)))

EXECUTABLE = Micro

all: compiler

compiler: directory $(OBJECT) 
	$(CC) -o $(EXECUTABLE) $(OBJECT)

directory:
	mkdir -p $(BUILDDIR) $(GENERATEDDIR)

$(BUILDDIR)/%.o: $(SOURCEDIR)/%.cpp $(HEADER)
	$(CC) -o $@ $(CFLAGS) $<

$(BUILDDIR)/%.o: $(GENERATEDDIR)/%.c $(HEADER)
	$(CC) -o $@ $(CFLAGS) $<

$(GENERATEDDIR)/scanner.c: $(SOURCEDIR)/scanner.l $(SOURCEDIR)/scanner.h
	$(LEX) -o $@ $<

clean:
	rm -rf Micro $(BUILDDIR) $(GENERATEDDIR)
