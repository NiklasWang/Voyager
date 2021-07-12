CXX      = $(GLOBAL_CXX)
CXXFLAGS = $(GLOBAL_CXXFLAGS)
LDFLAGS  = $(GLOBAL_LDFLAGS)

TARGET      = libserver$(strip $(DYLIB_EXT))
sources     = $(wildcard *.cpp)
objects     = $(sources:.cpp=.o)
dependence := $(sources:.cpp=.d)

SHARED_LIBS     =
STATIC_LIBS     =
INCLUDE_MODULE  = $(GLOBAL_SHARED_LIBS) $(GLOBAL_STATIC_LIBS)

include $(MAKE_RULE)/find.library.make.rule 

-include $(dependence)

include $(MAKE_RULE)/dependency.make.rule

include $(MAKE_RULE)/submodule.make.rule


all: compile link build release

compile: $(objects) $(COMPILE_SUB_MODULES)

link: $(objects) $(LINK_SUB_MODULES)
	$(CXX) $(objects) $(LDFLAGS) -o $(TARGET)

build: $(BUILD_SUB_MODULES)

clean: $(CLEAN_SUB_MODULES)
	rm -f $(TARGET) $(objects) $(dependence) $(CACHEFILE) *.d*


include $(MAKE_RULE)/general.release.make.rule

release:  general_release $(RELEASE_SUB_MODULES)

.PHONY: clean $(MAKE_SUB_MODULES) $(COMPILE_SUB_MODULES) $(LINK_SUB_MODULES) $(CLEAN_SUB_MODULES)
