#include "wdt.h"

#include "sam3x8e.h"

void wdt_disable()
{
  Wdt.mr = _b(15);
}
