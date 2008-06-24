# -*- Makefile -*-
# devel://collin/support/Makefile

include $(HOME)/devel/mk/bootstrap.mk

$(call require,support)

SUBDIRS:=src
export ALTERNATE_SUFFIXES:=pic
build: pic

$(call finalize)
