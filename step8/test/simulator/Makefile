CC=g++
EXEC=tiny
SOURCE=tiny.c
OBJECT=tiny.o

all:$(OBJECT)
	@$(CC) -o $(EXEC) $<

%.o:%.c
	@$(CC) -c $<

clean:
	rm -rf $(OBJECT) $(EXEC)
