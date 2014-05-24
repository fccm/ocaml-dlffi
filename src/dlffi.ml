(* Copyright (c) 2010 Florent Monnier *)
(* Here is the file "dlffi.ml" which provides the interface to OCaml *)

type lib_handle
type func_handle

type pointer

type arg_type =
  | Int of int
  | Float of float
  | Double of float
  | String of string
  | Ptr of pointer
  | Void

type return_type =
  | Return_int
  | Return_float
  | Return_double
  | Return_string
  | Return_ptr
  | Return_void

type rtld_flags =
  | RTLD_LAZY
  | RTLD_NOW
  | RTLD_GLOBAL
  | RTLD_LOCAL
  | RTLD_NODELETE
  | RTLD_NOLOAD
  | RTLD_DEEPBIND

external dlopen: libname:string -> flags:rtld_flags list -> lib_handle
  = "ml_dlopen"

external dlsym: lib:lib_handle -> func_name:string -> func_handle
  = "ml_dlsym"

external dlclose: lib:lib_handle -> unit
  = "ml_dlclose"

external fficall: func:func_handle -> args:arg_type array -> return:return_type -> arg_type
  = "ml_fficall"

