#-*- Makefile -*-
# Toplevel makefile for support library.

include $(HOME)/devel/mk/bootstrap.mk

$(call require,support)

TARGETS:=libsupport.a libsupport_debug.a

CFLAGS_ADD:=$(SUPPORT_CFLAGS)

libsupport_a_SOURCES:=ref.cc dllist.c hash_table.c meta.c mlog.c strutils.c	\
		      vector.c ini.c matrix.c

$(call finalize)

