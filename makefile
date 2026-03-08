.PHONY: all clean

CC = cc
CFLAGS += -O2 -std=c89
CFLAGS += -Wall -Wextra -pedantic
CFLAGS += -I. -Icipher

CCFILES = $(wildcard cipher/*/*.c)

OBJDIR = bin
OBJCDIR = $(OBJDIR)/cipher

OBJCDIRS = $(addprefix $(OBJCDIR)/,$(notdir $(basename $(CCFILES))))

OBJS += $(OBJDIR)/test.o
OBJS += $(patsubst %.c,$(OBJDIR)/%.o,$(CCFILES))

ifeq ($(CC),clang)
CFLAGS += -Wno-newline-eof
endif

all: $(OBJS)
	$(CC) -o tp $^

clean:
ifneq ($(wildcard $(OBJDIR)/.*),)
	rm -fr *.exe $(OBJDIR)/*
	rmdir $(OBJDIR)
else
	@echo "Already cleaned"
endif

$(OBJS): | $(OBJDIR) $(OBJCDIRS)

$(OBJDIR):
	mkdir $(OBJDIR)
$(OBJCDIRS):
	mkdir -p $(OBJCDIRS)

$(OBJDIR)/%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

$(OBJCDIR)/%/%.o: cipher/%/%.c
	$(CC) $(CFLAGS) -c -o $@ $<