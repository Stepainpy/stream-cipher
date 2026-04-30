.PHONY: all clean

CC = cc
CFLAGS += -O2 -std=c89
CFLAGS += -Wall -Wextra -pedantic
CFLAGS += -I. -Icipher

CCFILES = $(wildcard cipher/*/*.c)

NAME = tp
ifeq ($(OS),Windows_NT)
EXE = $(NAME).exe
else
EXE = $(NAME)
endif

OBJDIR = bin
OBJCDIR = $(OBJDIR)/cipher

OBJCDIRS = $(addprefix $(OBJCDIR)/,$(notdir $(basename $(CCFILES))))

OBJS += $(OBJDIR)/test.o
OBJS += $(patsubst %.c,$(OBJDIR)/%.o,$(CCFILES))

ifeq ($(CC),clang)
CFLAGS += -Wno-newline-eof
endif

all: $(EXE)

clean:
ifneq ($(wildcard $(OBJDIR)/.*),)
	rm -fr $(EXE) $(OBJDIR)
else
	@echo "Already cleaned"
endif

$(EXE): $(OBJS)
	$(CC) -o $@ $^

$(OBJS): | $(OBJDIR) $(OBJCDIRS)

$(OBJDIR):
	mkdir $(OBJDIR)
$(OBJCDIRS):
	mkdir -p $(OBJCDIRS)

$(OBJDIR)/%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

$(OBJCDIR)/%/%.o: cipher/%/%.c
	$(CC) $(CFLAGS) -c -o $@ $<