/* Copyright (c) 2010 Florent Monnier */
#include <dlfcn.h>
#include <ffi.h>

#include <caml/mlvalues.h>
#include <caml/memory.h>
#include <caml/alloc.h>
#include <caml/fail.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static const int rtld_flags_table[] = {
  RTLD_LAZY,
  RTLD_NOW,
  RTLD_GLOBAL,
  RTLD_LOCAL,
  RTLD_NODELETE,
  RTLD_NOLOAD,
  RTLD_DEEPBIND,
};

static inline int
rtld_flags_val( value mask_list )
{
  int c_mask = 0; 
  while (mask_list != Val_emptylist) {
    value head = Field(mask_list, 0);
    c_mask |= rtld_flags_table[Long_val(head)];
    mask_list = Field(mask_list, 1);
  }
  return c_mask;
}

CAMLprim value
ml_dlopen( value libname, value ml_flags )
{
  void *lib_handle = dlopen(String_val(libname), rtld_flags_val(ml_flags));
  if (lib_handle == NULL) failwith(dlerror());
  return (value) lib_handle;
}

CAMLprim value
ml_dlsym( value lib_handle, value func_name )
{
  char *status;
  void (*func_handle)();
  (void)dlerror();
  *(void **)(&func_handle) = dlsym((void *)lib_handle, String_val(func_name));
  status = dlerror();
  if (status != NULL) failwith(status);
  return (value) func_handle;
}

CAMLprim value
ml_dlclose( value lib_handle )
{
  int status = dlclose((void *)lib_handle);
  if (status != 0) failwith("dlclose");
  return Val_unit;
}

#define Val_tagged(v, tag) \
  this = caml_alloc(1, tag); \
  Store_field(this, 0, v);

//define ALLOC_INT_PARAM 1
//define ALLOC_DOUBLE_PARAM 1
//define ALLOC_STRING_PARAM 1

CAMLprim value
ml_fficall( value func_handle, value ml_args, value ml_return )
{
  CAMLparam3(func_handle, ml_args, ml_return);
  CAMLlocal2(ml_ret, this);

  unsigned int i, nargs;
  ffi_cif    cif;
  ffi_type   **arg_types;
  void       **arg_values;
  ffi_status status;
  ffi_type   *rtype;
  int ffi_prep_cif_failed = 0;

  nargs = Wosize_val(ml_args);

  arg_types  = (ffi_type **) malloc(nargs * sizeof(ffi_type *));
  arg_values = (void **) malloc(nargs * sizeof(void *));

  /* Set up the parameters */
  for (i = 0; i < nargs; i++) {
    value v = Field(ml_args, i);
    switch (Tag_val(v)) {
      case 0:  /* Int */
#if defined(ALLOC_INT_PARAM)
      { int *arg;
        arg = malloc(sizeof(int));
        *arg = Long_val(Field(v,0));
        arg_types[i] = &ffi_type_sint;
        arg_values[i] = arg;
      } break;
#else
      { arg_types[i] = &ffi_type_sint;
        Field(v,0) >>= 1;
        arg_values[i] = &Field(v,0);
      } break;
#endif
      case 1:  /* Float */
      { float *arg;
        arg = malloc(sizeof(float));
        *arg = (float) Double_val(Field(v,0));
        arg_types[i] = &ffi_type_float;
        arg_values[i] = arg;
      } break;
      case 2:  /* Double */
#if defined(ALLOC_DOUBLE_PARAM)
      { double *arg;
        arg = malloc(sizeof(double));
        *arg = Double_val(Field(v,0));
        arg_types[i] = &ffi_type_double;
        arg_values[i] = arg;
      } break;
#else
      { arg_types[i] = &ffi_type_double;
        arg_values[i] = &Double_val(Field(v,0));
      } break;
#endif
      case 3:  /* String */
#if defined(ALLOC_STRING_PARAM)
      { char *arg;
        int len = caml_string_length(Field(v,0)) + 1;
        arg = malloc(sizeof(char) * len);
        memcpy(arg, String_val(Field(v,0)), len);
        arg_types[i] = &ffi_type_float;
        arg_values[i] = &arg;
      } break;
#else
      { arg_types[i] = &ffi_type_pointer;
        arg_values[i] = &Byte(v,0);
      } break;
#endif
      case 4:  /* Ptr */
      { arg_types[i] = &ffi_type_pointer;
        arg_values[i] = &Field(v,0);
      } break;
      case 5:  /* Void */
        caml_invalid_argument("fficall");
        break;
    }
  }

  switch (Int_val(ml_return)) {  /* return_type */
    case 0:  /* Return_int */
      rtype = &ffi_type_sint;
      break;
    case 1:  /* Return_float */
      rtype = &ffi_type_float;
      break;
    case 2:  /* Return_double */
      rtype = &ffi_type_double;
      break;
    case 3:  /* Return_string */
      rtype = &ffi_type_pointer;
      break;
    case 4:  /* Return_ptr */
      rtype = &ffi_type_pointer;
      break;
    case 5:  /* Return_void */
      rtype = &ffi_type_uint;
      break;
  }

  status = ffi_prep_cif(&cif, FFI_DEFAULT_ABI, nargs, rtype, arg_types);
  if (status != FFI_OK) {
    ffi_prep_cif_failed = 1;
    goto freeandfail;
  }

  switch (Int_val(ml_return)) {
    case 0:  /* Return_int */
    { int result;
      ffi_call(&cif, (void (*)(void)) func_handle, &result, arg_values);
      ml_ret = Val_tagged( Val_long(result), 0);
    } break;
    case 1:  /* Return_float */
    { float result;
      ffi_call(&cif, (void (*)(void)) func_handle, &result, arg_values);
      ml_ret = Val_tagged( caml_copy_double(result), 1);
    } break;
    case 2:  /* Return_double */
    { double result;
      ffi_call(&cif, (void (*)(void)) func_handle, &result, arg_values);
      ml_ret = Val_tagged( caml_copy_double(result), 2);
    } break;
    case 3:  /* Return_string */
    { char *result;
      ffi_call(&cif, (void (*)(void)) func_handle, &result, arg_values);
      ml_ret = Val_tagged( caml_copy_string((result == NULL ? "" : result)), 3);
    } break;
    case 4:  /* Return_ptr */
    { void *result;
      ffi_call(&cif, (void (*)(void)) func_handle, &result, arg_values);
      ml_ret = Val_tagged( (value) result, 4);
    } break;
    case 5:  /* Return_void */
    { ffi_arg result;
      ffi_call(&cif, (void (*)(void)) func_handle, &result, arg_values);
      ml_ret = Val_int(0);
    } break;
  }

freeandfail:
  for (i = 0; i < nargs; i++) {
    switch (Tag_val(Field(ml_args, i))) {
      case 0:  /* Int */
#if defined(ALLOC_INT_PARAM)
        free(arg_values[i]);
#endif
        break;
      case 1:  /* Float */
        free(arg_values[i]);
        break;
      case 2:  /* Double */
#if defined(ALLOC_DOUBLE_PARAM)
        free(arg_values[i]);
#endif
        break;
      case 3:  /* String */
#if defined(ALLOC_STRING_PARAM)
      { char **arg;
        arg = arg_values[i];
        free(*arg);
      }
#endif
        break;
      case 4:  /* Ptr */
        break;
    }
  }
  free(arg_values);
  free(arg_types);

  if (ffi_prep_cif_failed)
    failwith("ffi_prep_cif");

  CAMLreturn(ml_ret);
}

