SGDK ?= C:/sgdk190
GDK := $(subst \,/,$(SGDK))
.DEFAULT_GOAL := release
include $(GDK)/makefile.gen

# SGDK 1.90 emits dependency files but leaves their inclusion disabled.
-include $(DEPS)
