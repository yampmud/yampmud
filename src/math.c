
#include <sys/types.h>
#include <sys/time.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include "merc.h"
#include "db.h"

int powerf(int value, int pow)
{
  int loop;

  if (pow == 0)
    return 1;

  if (pow == 1)
    return value;

  for (loop = 1; loop < pow; loop++)
  {
    value *= value;
  }
  return value;
}
