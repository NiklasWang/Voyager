CXX      = $(GLOBAL_CXX)
CXXFLAGS = $(GLOBAL_CXXFLAGS)
LDFLAGS  = $(GLOBAL_LDFLAGS)

TARGET      = AdaptiveProgressBar$(strip $(EXE_EXT))
sources     = AdaptiveProgressBar.cpp
objects     = $(sources:.cpp=.o)
dependence := $(sources:.cpp=.d)

-include $(dependence)

include $(MAKE_RULE)/dependency.make.rule

all: compile link build release

compile: $(objects)

link:

build: $(objects)
	$(CXX) $^ $(LDFLAGS) -o $(TARGET)

clean:
	rm -f $(TARGET) $(objects) $(dependence) *.d*


include $(MAKE_RULE)/general.release.make.rule

release:

.PHONY: clean