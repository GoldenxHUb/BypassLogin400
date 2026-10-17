LOCAL_PATH := $(call my-dir)

# Dobby (prebuilt static)
include $(CLEAR_VARS)
LOCAL_MODULE    := libdobby
LOCAL_SRC_FILES := Dobby/$(TARGET_ARCH_ABI)/libdobby.a
include $(PREBUILT_STATIC_LIBRARY)

# xdl (source → static lib)
include $(CLEAR_VARS)
LOCAL_MODULE      := libxdl
LOCAL_SRC_FILES   := xdl/xdl.c xdl/xdl_iterate.c xdl/xdl_linker.c xdl/xdl_lzma.c xdl/xdl_util.c
LOCAL_C_INCLUDES  := $(LOCAL_PATH)/xdl/include
include $(BUILD_STATIC_LIBRARY)

# Zygisk module
include $(CLEAR_VARS)
LOCAL_MODULE           := zygisk
LOCAL_SRC_FILES        := zygisk.cpp bypass_login.cpp
LOCAL_LDLIBS           := -llog -ldl
LOCAL_CPPFLAGS         := -std=c++17 -fno-exceptions -fno-rtti -fvisibility=hidden
LOCAL_CFLAGS           := -fvisibility=hidden
LOCAL_C_INCLUDES       := $(LOCAL_PATH) \
                          $(LOCAL_PATH)/Dobby \
                          $(LOCAL_PATH)/xdl/include
LOCAL_STATIC_LIBRARIES := libdobby libxdl
include $(BUILD_SHARED_LIBRARY)
