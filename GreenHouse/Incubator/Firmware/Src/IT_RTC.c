#include "main.h"

extern volatile int processClock;

void HAL_RTC_AlarmAEventCallback(RTC_HandleTypeDef *hrtc)
{
   processClock = 1;
}
