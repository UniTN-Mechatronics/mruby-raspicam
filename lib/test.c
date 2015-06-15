
#include <stdio.h>
#include "laserCam.h"

int main (int argc, char const *argv[])
{
  int x = 0, y = 0;
  if (argc != 2) {
    printf("Need name for image file to be saved!\n");
    return 0;
  }
  CRaspicamLaser rcl = newCRaspicamLaser(1280, 1024);
  CRaspicamLaserOpenCamera(rcl);
  if (!CRaspicamLaserAvailable(rcl)) {
    printf("*** Camera not available!\n");
    return -1;
  }
  if (0 != CRaspicamLaserPosition(rcl, &x, &y)) {
    printf("*** Error reading from camera!\n");
    return -2;
  }
  printf("x=%d, y=%d\n", x, y);
  CRaspicamLaserSaveFrame(rcl, argv[1], 2);
  printf("Saved image %s\n", argv[1]);
  CRaspicamLaserCloseCamera(rcl);
  return 0;
}