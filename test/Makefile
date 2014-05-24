# Copyright (c) 2010 Florent Monnier
PKG_CONFIG = pkg-config
FFI_LIBS = $(shell $(PKG_CONFIG) --libs libffi)
FFI_CFLAGS = $(shell $(PKG_CONFIG) --cflags libffi)

CC = gcc
OCAML = ocaml
OCAMLC = ocamlc
OCAMLOPT = ocamlopt
LIBDIR = ../src
EDITOR = vim

all: test_opt

testlib.so: testlib.c
	$(CC) -g -shared -nostartfiles $< -o $@

.PHONY: edit
edit:
	$(EDITOR) test.ml testlib.c

.PHONY: test
test: testlib.so
	$(OCAML) -I $(LIBDIR) dlffi.cma test.ml

test.opt: test.ml testlib.so
	$(OCAMLOPT) -g -o $@ -I $(LIBDIR) dlffi.cmxa $<

.PHONY: test_opt
test_opt: test.opt
	./$<

.PHONY: clean
clean:
	$(RM) *.[oa] *.so *.cm[ixoa] *.cmxa *.opt

