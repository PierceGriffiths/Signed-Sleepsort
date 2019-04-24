CC := gcc
SDIR := ./src
IDIR := $(SDIR)/include
CFLAGS := -march='native' -std=gnu11 -pedantic -I$(IDIR) -Wall
LIBS := -lpthread
ODIR := $(SDIR)/obj
ifeq ($(MAKECMDGOALS),$(filter $(MAKECMDGOALS),release sleepsort))
    CFLAGS := $(CFLAGS) -O3
else ifeq ($(MAKECMDGOALS),$(filter $(MAKECMDGOALS),debug sleepsort-debug))
    CFLAGS := $(CFLAGS) -Og -g -pg
    ODIR := $(SDIR)/obj/debug
endif

DEPS := $(wildcard $(IDIR)/*.h)

_OBJ := $(patsubst %.c, %.o, $(notdir $(wildcard $(SDIR)/*.c)))
OBJ := $(patsubst %, $(ODIR)/%, $(_OBJ))

$(ODIR)/%.o: $(SDIR)/%.c $(DEPS)
	@if [ ! -d "$(ODIR)" ]; then	\
	    mkdir -p $(ODIR);		\
	fi;				
	$(CC) $(CFLAGS) -c -o $@ $<

.PHONY: release debug cleanobj cleanall

release: sleepsort

debug: sleepsort-debug

sleepsort: $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

sleepsort-debug: $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

cleanobj:
	rm -f $(ODIR)/*.o $(ODIR)/debug/*.o *~ core $(IDIR)/*~

cleanall: cleanobj
	rm -f sleepsort sleepsort-debug
