MODULE_NAME := test-runner
MODULE_CPPFLAGS := -I.
MODULE_LIBRARIES := allocator util

$(call use-catch)
$(call use-fmt)

$(call add-executable-module,$(call get-path))
