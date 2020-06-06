LOCAL_SOURCE_FILES := $(wildcard *.cpp)

$(call use-fmt)

$(call add-static-library-module,$(call get-path))
