LOCAL_MODULE := test-runner
LOCAL_CPPFLAGS := -I.
LOCAL_LIBRARIES := allocator util

$(call use-catch)
$(call use-fmt)

$(call add-executable-module,$(call get-path))
