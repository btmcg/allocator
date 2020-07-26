MODULE_NAME := benchmark-runner
MODULE_CPPFLAGS := -I.
MODULE_LIBRARIES := allocator util

$(use-fmt)
$(use-google-benchmark)

$(call add-executable-module,$(get-path))
