CXX      = $(GLOBAL_CXX)
CXXFLAGS = $(GLOBAL_CXXFLAGS)
LDFLAGS  = $(GLOBAL_LDFLAGS)

TARGET      = SocketTesterServer$(strip $(EXE_EXT))
sources     = server_tester.cpp
objects     = $(sources:.cpp=.o)
dependence := $(sources:.cpp=.d)

STATIC_LIBS =
SHARED_LIBS =
INCLUDE_MODULE  = $(GLOBAL_STATIC_LIBS) $(GLOBAL_SHARED_LIBS)

include $(MAKE_RULE)/find.library.make.rule

-include $(dependence)

include $(MAKE_RULE)/dependency.make.rule


all: compile link build release

compile: $(objects)

link: $(objects)

build:
	$(CXX) $(objects) $(LDFLAGS) -o $(TARGET)

clean:
	rm -f $(TARGET) $(objects) $(dependence) $(CACHEFILE) *.d*


include $(MAKE_RULE)/general.release.make.rule

release:  general_release

.PHONY: clean

