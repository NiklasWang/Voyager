CXX      = $(GLOBAL_CXX)
CXXFLAGS = $(GLOBAL_CXXFLAGS)
AR       = $(GLOBAL_AR)
ARFLAGS  = $(GLOBAL_ARFLAGS)

TARGET      = libcommon$(strip $(STLIB_EXT))
sources     = $(wildcard *.cpp)
objects     = $(sources:.cpp=.o)
dependence := $(sources:.cpp=.d)

SHARED_LIBS     =
STATIC_LIBS     =
INCLUDE_MODULE  = liblog libutils


include $(MAKE_RULE)/find.library.make.rule

-include $(dependence)

include $(MAKE_RULE)/dependency.make.rule

all: compile link build release

compile: $(objects)

link: $(objects)
	$(AR) $(ARFLAGS) $(TARGET) $^

build:

clean:
	rm -f $(TARGET) $(objects) $(dependence) $(CACHEFILE) *.d*

release:

.PHONY: clean
