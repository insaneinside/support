# -*- Makefile -*-
# devel://collin/support/Makefile

include $(HOME)/devel/mk/bootstrap.mk

$(call require,support)

SUBDIRS:=src tests

$(call finalize)
