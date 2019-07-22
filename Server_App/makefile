# file makefile
CFLAGS = -g -Wall 
LDFLAGS = 
CC=gcc

SRCDIR = src
OBJDIR = obj
TESTDIR = test
EXEFILE = project

srcfiles = $(wildcard $(SRCDIR)/*.c)
objfiles = $(patsubst $(SRCDIR)/%,$(OBJDIR)/%,$(srcfiles:.c=.o))
testfiles = $(wildcard $(TESTDIR)/*)
deps = $(objfiles:.o=.d)


.PHONY: clean

all:$(TESTDIR)/$(EXEFILE)
-include $(deps)

# Dependencies der einzelnen C-Dateien herausfinden und in obj gespeichert
$(OBJDIR)/%.d: $(SRCDIR)/%.c
	$(CC) -MM -MT "$@ $(patsubst %.d,%.o,$@)" -MF $@ $<

# .o dateien der entsprechenden .c Dateien erstellen und in obj speichern
#$< gibt die erste Abhängigkeit an.
$(OBJDIR)/%.o: 
	$(CC) $(CFLAGS) -c $<
	mv *.o $(OBJDIR)

#Kopieren der Dateien von test nach bin 
install:
	mkdir -p $(BINDIR)
	cp -p $(TESTDIR)/* $(BINDIR)

#Programme werden gebunden und in test gespeichert
$(TESTDIR)/$(EXEFILE): $(objfiles)
	$(CC) -o $@ $^ $(LDFLAGS)


clean:
	rm -f $(objfiles) $(depfiles) $(deps) $(testfiles)
