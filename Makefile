# Copyright (c) 2010 Florent Monnier
PKG_CONFIG = pkg-config
FFI_LIBS = $(shell $(PKG_CONFIG) --libs libffi)
FFI_CFLAGS = $(shell $(PKG_CONFIG) --cflags libffi)

CC = gcc
OCAMLC = ocamlc
OCAMLOPT = ocamlopt
OCAMLMKLIB = ocamlmklib
EDITOR = vim

all: test_opt

dlffi_stubs.o: dlffi_stubs.c
	$(OCAMLC) -g -c -ccopt $(FFI_CFLAGS) $<

dlldlffi_stubs.so: dlffi_stubs.o
	$(OCAMLMKLIB) -o dlffi_stubs $< -ldl $(FFI_LIBS)

dlffi.cmo: dlffi.ml
	$(OCAMLC) -c $<

dlffi.cmx: dlffi.ml
	$(OCAMLOPT) -c $<

dlffi.cma: dlffi.cmo  dlldlffi_stubs.so
	$(OCAMLC) -a -o $@ $< -dllib -ldlffi_stubs -cclib -ldl -cclib $(FFI_LIBS)

dlffi.cmxa:  dlffi.cmx  dlldlffi_stubs.so
	$(OCAMLOPT) -a -o $@ $< -cclib -ldlffi_stubs -cclib -ldl -cclib $(FFI_LIBS)

testlib.so: testlib.c
	$(CC) -g -shared -nostartfiles $< -o $@

.PHONY: edit
edit:
	$(EDITOR) dlffi_stubs.c dlffi.ml Makefile test.ml testlib.c

.PHONY: test
test: dlffi.cma testlib.so
	ocaml dlffi.cma test.ml

test.opt: test.ml dlffi.cmxa testlib.so
	ocamlopt -g -o $@ -I . dlffi.cmxa $<

.PHONY: test_opt
test_opt: test.opt
	./$<

.PHONY: clean
clean:
	$(RM) *.[oa] *.so *.cm[ixoa] *.cmxa *.opt

