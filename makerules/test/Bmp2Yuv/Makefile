CC      = $(GLOBAL_CC)
CFLAGS  = $(GLOBAL_CFLAGS)
LDFLAGS = $(GLOBAL_LDFLAGS) -lm

TARGET      = bmp2yuv$(strip $(EXE_EXT))
sources     = Bmp2Yuv.c Bmp2YuvTester.c
objects     = $(sources:.c=.o)
dependence := $(sources:.c=.d)

-include $(dependence)

include $(MAKE_RULE)/dependency.make.rule

all: compile link build release

compile: $(objects)

link:

build: $(objects)
	$(CC) $^ $(LDFLAGS) -o $(TARGET)

clean:
	rm -f $(TARGET) $(objects) $(dependence) *.d*
	rm -f *.nv21


include $(MAKE_RULE)/general.release.make.rule

release:

.PHONY: clean