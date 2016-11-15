#include "wdt.h"

#include "reg.h"

void wdt_disable()
{
  WDT_MR = WDDIS;
}
