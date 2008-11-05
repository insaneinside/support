#-*- Makefile -*-
# Toplevel makefile for support library.

include $(HOME)/devel/mk/bootstrap.mk

$(call require,support)

TARGETS:=libsupport.a

libsupport_a_SUFFIXES:=debug pic pic_debug
libsupport_pic_a_SUFFIXES:=debug pic pic_debug
build: debug pic pic_debug

CFLAGS_ADD:=$(SUPPORT_CFLAGS)

libsupport_a_SOURCES:=ref.cc dllist.c hash_table.c mlog.c strutils.c	\
		      vector.c matrix.c #ini.c meta.c

$(call finalize)

