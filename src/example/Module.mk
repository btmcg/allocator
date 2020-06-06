LOCAL_SOURCE_FILES := $(wildcard *.cpp)
LOCAL_LIBRARIES := allocator

$(call add-executable-module,$(call get-path))
