OCAML = ocaml
LIBDIR = ../src/

run:
	$(OCAML) -I $(LIBDIR) dlffi.cma example_X11.ml

.PHONY: run
