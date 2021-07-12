include $(MAKE_RULE)/submodule.make.rule

all: compile link build release

compile: $(COMPILE_SUB_MODULES)

link: $(LINK_SUB_MODULES)

build: $(BUILD_SUB_MODULES)

clean: $(CLEAN_SUB_MODULES)

release: $(RELEASE_SUB_MODULES)

.PHONY: all clean $(MAKE_SUB_MODULES) $(COMPILE_SUB_MODULES) $(LINK_SUB_MODULES) $(CLEAN_SUB_MODULES)
