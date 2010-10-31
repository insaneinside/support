#define _GNU_SOURCE
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <support/timeutil.h>
#include <support/scalar.h>

/** Fill time by doing some expensive calculations.
 *
 * @param n Number of iterations to loop through.
 */
void fillTime(int n)
{
  scalar_t x;
  for ( int i = 1; i <= n; i++ )
    x += S_POW((scalar_t) i, (scalar_t) i);
}

int
main(int argc __attribute__ (( unused )),
     char* argv[] __attribute__ (( unused )))
{
  timeutil_init_mark_variables();
  timeutil_begin("Wasting time");
  fillTime(100000);
  timeutil_end();

  timeutil_begin("Sleeping for one second");
  sleep(1);
  timeutil_end();


  return 0;
}
