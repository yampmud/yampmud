CC      = gcc
PROF    = -ggdb3
WARN    = -Wall -pipe 
C_FLAGS =  $(WARN) $(PROF) $(FLAGS)
LIBS    = -lcrypt
L_FLAGS =  $(PROF) $(LIBS)

C_FILES := $(wildcard *.c)
O_FILES := $(patsubst %.c, %.o, $(C_FILES))
H_FILES := $(wildcard *.h)	  

all:: rot

rot: $(O_FILES)
	@rm -f rot
	@echo "Finishing..."
	@$(CC) $(L_FLAGS) -o rot $(O_FILES) $(LIBS)
	@echo "Done."
.c.o: merc.h
	@echo "Compiling $<..."
	@$(CC) -c $(C_FLAGS) $<

depend:
	@$(CC) -E -MM $(C_FILES) -I. > .depend

indent:
	@indent -bad -bap $(C_FILES) $(H_FILES)

backup:
	@tar -cf backup.tar $(C_FILES) $(H_FILES) Makefile

clean:
	@rm -f $(O_FILES) *~
	@echo "Done Cleaning."
