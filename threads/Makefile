CXX      = $(GLOBAL_CXX)
CXXFLAGS = $(GLOBAL_CXXFLAGS)
LDFLAGS  = $(GLOBAL_LDFLAGS)

TARGET      = libthreads$(strip $(DYLIB_EXT))
sources     = $(wildcard *.cpp)
objects     = $(sources:.cpp=.o)
dependence := $(sources:.cpp=.d)

SHARED_LIBS    =
STATIC_LIBS    =
INCLUDE_MODULE = $(GLOBAL_SHARED_LIBS) $(GLOBAL_STATIC_LIBS) libcore

include $(MAKE_RULE)/find.library.make.rule 

-include $(dependence)

include $(MAKE_RULE)/dependency.make.rule


all: compile link build release

compile: $(objects)

link: $(objects)
	$(CXX) $^ $(LDFLAGS) -o $(TARGET)

build:

clean:
	rm -f $(TARGET) $(objects) $(dependence) $(CACHEFILE) *.d*


include $(MAKE_RULE)/general.release.make.rule

release: general_release

.PHONY: clean
