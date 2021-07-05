CXX      = $(GLOBAL_CXX)
CXXFLAGS = $(GLOBAL_CXXFLAGS)
LDFLAGS  = $(GLOBAL_LDFLAGS) -fPIC -shared

TARGET      = libTypeinfoInDlopen$(strip $(DYLIB_EXT))
sources     = TypeinfoInDlopen.cpp
objects     = $(sources:.cpp=.o)
dependence := $(sources:.cpp=.d)

-include $(dependence)

include $(MAKE_RULE)/dependency.make.rule

all: compile link build release

compile: $(objects)

link: $(objects)
	$(CXX) $^ $(LDFLAGS) -o $(TARGET)

build:

clean:
	rm -f $(TARGET) $(objects) $(dependence) *.d*


include $(MAKE_RULE)/general.release.make.rule

release:

.PHONY: clean