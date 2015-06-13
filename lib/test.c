
#include <stdio.h>
#include "laserCam.h"

int main (int argc, char const *argv[])
{
  int x = 0, y = 0;
  CRaspicamLaser rcl = newCRaspicamLaser();
  if (!CRaspicamLaserAvailable(rcl)) {
    printf("*** Camera not available!\n");
    return -1;
  }
  if (0 == CRaspicamLaserPosition(rcl, &x, &y)) {
    printf("x=%d, y=%d\n", x, y);
    CRaspicamLaserSaveFrame(rcl, "./test.jpg", 0);
  }
  else
    printf("*** Error reading from camera!\n");
  return 0;
}