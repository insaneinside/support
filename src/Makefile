#-*- Makefile -*-
# Toplevel makefile for support library.

include $(HOME)/devel/mk/bootstrap.mk

$(call require,support)

ALTERNATE_SUFFIXES:=pic debug
build: pic debug

TARGETS:=libsupport.a
CFLAGS_ADD:=$(SUPPORT_CFLAGS)

libsupport_a_SOURCES:=ref.cc dllist.c hash_table.c meta.c mlog.c strutils.c	\
		      vector.c ini.c matrix.c

$(call finalize)

