CC      = $(GLOBAL_CC)
CFLAGS  = $(GLOBAL_CFLAGS)
LDFLAGS = $(GLOBAL_LDFLAGS)

TARGET      = FontBitmapTest$(strip $(EXE_EXT))
sources     = FontBitmapTest.c
objects     = $(sources:.c=.o)
dependence := $(sources:.c=.d)

SYSTEM_LIBS = m
STATIC_LIBS = $(GLOBAL_STATIC_LIBS)
SHARED_LIBS = $(GLOBAL_SHARED_LIBS) libfreetype libpng16 libz

include $(MAKE_RULE)/find.library.make.rule

-include $(dependence)

include $(MAKE_RULE)/dependency.make.rule

all: compile link build release

compile: $(objects)

link:

build: $(objects)
	$(CC) $^ $(LDFLAGS) -o $(TARGET)

clean:
	rm -f $(TARGET) $(objects) $(dependence) $(CACHEFILE) *.d*


include $(MAKE_RULE)/general.release.make.rule

release:

.PHONY: clean
