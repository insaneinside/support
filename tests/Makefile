# -*- Makefile -*-
# devel://collin/support/Makefile

include $(HOME)/devel/mk/bootstrap.mk

$(call require,support)

TARGETS:=dllist-test meta-test matrix-test # meta-test_debug 
CFLAGS_ADD:=$(SUPPORT_CFLAGS)

meta_test_SOURCES:=meta-test.c
meta_test_ADD:=$(SUPPORT_LIBS)
meta_test_DEPENDS:=$(SUPPORT_LIBS)

dllist_test_SOURCES:=dllist-test.c
dllist_test_ADD:=$(SUPPORT_LIBS)


$(call finalize)
