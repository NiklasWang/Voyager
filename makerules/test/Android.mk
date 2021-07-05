LOCAL_PATH  := $(call my-dir)
MODULE_PATH := $(abspath $(LOCAL_PATH))

compile_order :=
exclude_dirs  :=
include $(MAKE_RULES)/module.android.make.rule
include $(MAKE_RULES)/submodule.android.make.rule