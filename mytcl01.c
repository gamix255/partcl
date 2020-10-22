
#include <stdio.h>

#define TEST
#include "tcl.c"

int status = 0;

#define FAIL(...)                                                              \
  do {                                                                         \
    printf("FAILED: " __VA_ARGS__);                                            \
    status = 1;                                                                \
  } while (0)


static void tcl_check_eval(struct tcl *tcl, const char *s, char *expected) {
  int destroy = 0;
  if (tcl == NULL) {
    struct tcl tmp;
    tcl_init(&tmp);
    tcl = &tmp;
    destroy = 1;
  }
  if (tcl_eval(tcl, s, strlen(s) + 1) == FERROR) {
    FAIL("eval returned error: %s, (%s)\n", tcl_string(tcl->result), s);
  } else if (strcmp(tcl_string(tcl->result), expected) != 0) {
    FAIL("Expected %s, but got %s. (%s)\n", expected, tcl_string(tcl->result),
         s);
  } else {
    printf("OK: %s -> %s\n", s, expected);
  }
  if (destroy) {
    tcl_destroy(tcl);
  }
}


static void tcl_test_flow_mini() {
  printf("\n");
  printf("##########################\n");
  printf("### CONTROL FLOW TESTS ###\n");
  printf("##########################\n");
  printf("\n");

  tcl_check_eval(NULL, "set x 1; proc two {} { set x 2;}; two; subst $x", "1");
  /* Example from Picol */
  tcl_check_eval(NULL, "proc fib {x} { if {<= $x 1} {return 1} "
                   "{ return [+ [fib [- $x 1]] [fib [- $x 2]]]}}; fib 20",
             "10946");

  struct tcl tcl;
  tcl_init(&tcl);
  tcl_check_eval(&tcl, "proc square {x} { * $x $x }; square 7", "49");
  tcl_check_eval(&tcl, "set a 4", "4");
  tcl_check_eval(&tcl, "square $a", "16");
  tcl_check_eval(&tcl, "subst \"$a[]*$a ?\"", "4*4 ?");
  tcl_check_eval(&tcl, "subst \"I can compute that $a[]x$a = [square $a]\"",
             "I can compute that 4x4 = 16");
  tcl_check_eval(&tcl, "set a 1", "1");
  tcl_check_eval(&tcl, "while {<= $a 10} { puts \"$a [== $a 5]\";"
                   "if {== $a 5} { puts {Missing five!}; set a [+ $a 1]; "
                   "continue;}; puts \"I can compute that $a[]x$a = [square "
                   "$a]\" ; set a [+ $a 1]}",
             "0");

  tcl_destroy(&tcl);
}


int main() {
  tcl_test_flow_mini();
  return status;
}
