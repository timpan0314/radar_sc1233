/*
 * Copyright (c) 2018-2019 Socionext Inc.
 * All rights reserved.
 */

/*
 * rs_sleep.cpp
 */

#ifdef WIN32
#include <windows.h>
#else	/* WIN32 */
#  if defined(__MBED__)
#include <mbed_wait_api.h>
#  else
//#include <unistd.h>  
#  endif
#endif	/* WIN32 */

#include "rs_sleep.h"


//#define RS_SLEEP_DEBUGPRINT

#ifdef RS_SLEEP_DEBUGPRINT
#include <stdio.h>
#define rs_sleep_dp(_usec_)	{fprintf(stdout, "rs_usleep(%d)\n", (int) (_usec_)); fflush(stdout);}
#else   /* RS_SLEEP_DEBUGPRINT */
#define rs_sleep_dp(...)
#endif  /* RS_SLEEP_DEBUGPRINT */


#ifdef __cplusplus
extern "C" {
#endif
  
void usleep(uint32_t usec)
{
  unsigned int i,s;
  for(s=0;s<usec;s++){
  for (i=0;i<650;i++)
    asm("nop");
  }
}
    

void rs_usleep(uint32_t usec)
{
	rs_sleep_dp(usec);
#ifdef WIN32
	Sleep((DWORD) (((usec) + 999) / 1000));
#else	/* WIN32 */
#  if defined(__MBED__)
	wait_us(usec);
#  else
	usleep(usec); 
#  endif
#endif	/* WIN32 */
}

#ifdef __cplusplus
}
#endif
