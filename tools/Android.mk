LOCAL_PATH  := $(call my-dir)
MODULE_PATH := $(abspath $(LOCAL_PATH))

compile_order :=
exclude_dirs  := Bmp2Yuv FontBitmap
include $(MAKE_RULES)/submodule.android.make.rule