#include <support/strutils.h>
#include <stdlib.h>
#include <stdio.h>


int
main(int argc __attribute__ (( unused )),
     char* argv[] __attribute__ (( unused )))
{
  const char* testString = "  whee!  ";
  char* chomp = strchomp(testString);
  char* chug = strchug(testString);
  printf("test string: >>%s<<\n", testString);
  printf("    chomp'd: >>%s<<\n", chomp);
  printf("     chug'd: >>%s<<\n", chug);
  free(chug);

  return 0;
}
  
