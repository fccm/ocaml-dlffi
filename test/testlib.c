/* Copyright (c) 2010 Florent Monnier */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int func_static_handle(float v)
{
  static int handle = 1;
  fprintf(stderr, "fun called %d times, arg: %g\n", handle, v);
  fflush(stderr);  /* ocaml's stderr is different from C's stderr */
  return handle++;
}

char *msgs[] = {
  "alpha",
  "beta",
  "gamma",
  "delta",
  "epsilon",
  "zeta",
};

char *func_string_param(const char *s)
{
  static int handle = 0;
  char *ret;
  ret = &(msgs[handle % 6][0]);
  handle++;
  fprintf(stderr, "c: got string (%s) index[%d], returning: [%s]\n", s, handle, ret);
  fflush(stderr);
  return ret;
  //return NULL;  /* test the special case with NULL */
}

int func_int_param(const int d)
{
  printf("c: got int (%d)\n", d); fflush(stdout);
  return 107;
}

double func_double_param(const double d)
{
  printf("c: got double (%g)\n", d); fflush(stdout);
  return 233.1;
}

float func_float_param(const float f)
{
  printf("c: got float (%g)\n", f); fflush(stdout);
  return 233.2;
}

void func_void_param(void)
{
  printf("c: void function\n"); fflush(stdout);
}

