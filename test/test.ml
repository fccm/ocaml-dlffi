(* Copyright (c) 2010 Florent Monnier *)
open Dlffi

let handle_return = function
  | Int v    -> Printf.printf "ocaml: got (Int %d)\n%!" v
  | Float v  -> Printf.printf "ocaml: got (Float %g)\n%!" v
  | Double v -> Printf.printf "ocaml: got (Double %g)\n%!" v
  | String v -> Printf.printf "ocaml: got (String \"%s\")\n%!" v
  | Ptr _    -> Printf.printf "ocaml: got (Ptr)\n%!"
  | Void     -> Printf.printf "ocaml: got (Void)\n%!"

let test1 () =
  let lib = dlopen "./testlib.so" [RTLD_LAZY] in
  let func = dlsym ~lib ~func_name:"func_static_handle" in
  handle_return(fficall ~func ~args:[| Float 1.1 |] ~return:Return_int);
  handle_return(fficall ~func ~args:[| Float 2.2 |] ~return:Return_int);
  handle_return(fficall ~func ~args:[| Float 3.3 |] ~return:Return_int);
  dlclose ~lib;
;;

let test2 () =
  let lib = dlopen "./testlib.so" [RTLD_LAZY] in
  let func = dlsym ~lib ~func_name:"func_string_param" in
  handle_return(fficall ~func ~args:[| String "one" |] ~return:Return_string);
  dlclose ~lib;
;;

let test3 () =
  let lib = dlopen "./testlib.so" [RTLD_LAZY] in
  let func = dlsym ~lib ~func_name:"func_int_param" in
  handle_return(fficall ~func ~args:[| Int 244 |] ~return:Return_int);
  dlclose ~lib;
;;

let test4 () =
  let lib = dlopen "./testlib.so" [RTLD_LAZY] in
  let func = dlsym ~lib ~func_name:"func_double_param" in
  handle_return(fficall ~func ~args:[| Double 211.6 |] ~return:Return_double);
  dlclose ~lib;
;;

let test5 () =
  let lib = dlopen "./testlib.so" [RTLD_LAZY] in
  let func = dlsym ~lib ~func_name:"func_float_param" in
  handle_return(fficall ~func ~args:[| Float 211.7 |] ~return:Return_float);
  dlclose ~lib;
;;

let test6 () =
  let lib = dlopen "./testlib.so" [RTLD_LAZY] in
  let func = dlsym ~lib ~func_name:"func_void_param" in
  handle_return(fficall ~func ~args:[| |] ~return:Return_void);
  dlclose ~lib;
;;

let () =
  print_newline ();  test1 ();
  print_newline ();  test2 ();
  print_newline ();  test3 ();
  print_newline ();  test4 ();
  print_newline ();  test5 ();
  print_newline ();  test6 ();
;;

