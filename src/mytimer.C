/*
    Sabre Fighter Plane Simulator 
    Copyright (c) 1997/98 Dan Hammer
    Portions Donated By Antti Barck

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 1, or (at your option)
    any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/
#include <stdio.h>
#include <stdlib.h>
#include "mytimer.h"
/*
 *  08/31/87 Antti Barck 
 *           Portable timing using gettimeofday                 
 */

MTimer::MTimer()
{
#ifdef PORTABLE_TIMING
  gettimeofday( &t1, &dontcare );
#endif  
#ifdef CLOCK_TIMING
  t1 = clock();
#endif  
}

float MTimer::get_time()
{
  float d;
#ifdef PORTABLE_TIMING
  gettimeofday( &t2, &dontcare );
  d = (float)((t2.tv_sec-t1.tv_sec)+(t2.tv_usec-t1.tv_usec)*0.000001);
  gettimeofday( &t1, &dontcare );
  return d;
#endif  
#ifdef CLOCK_TIMING
  t2 = clock();
  d = (float) (t2 - t1);
  t1 = clock();
  return (d / ((float)CLOCKS_PER_SEC));
#endif  
}

int MTimer::check_time()
{
#ifdef PORTABLE_TIMING
  gettimeofday( &t2, &dontcare );
  /* rough figures but only checking here */
  return (((t2.tv_sec<<20)+t2.tv_usec) > ((t1.tv_sec<<20)+t1.tv_usec)); 
  /* must optimize this ! */
#endif
#ifdef CLOCK_TIMING
  t2 = clock();
  return (t2 > t1);
#endif  
}

void MTimer::reset()
{
#ifdef PORTABLE_TIMING
  gettimeofday( &t1, &dontcare );
#endif  
#ifdef CLOCK_TIMING
  t1 = clock();
#endif  
}

#ifdef TEST_MYTIMER
int main()
{
  MTimer timer;

  /* sleep for three seconds */
  sleep( 3 );
  printf("timer: %5.2f\n", timer.get_time());

  /* sleep for 3/10 seconds */
  usleep( 300000 );
  printf("timer: %5.2f\n", timer.get_time());

  /* be busy (time depends on platfrom, ~3/100 on Alpha 21066) */
  for( double i=0; i<100.0; i+=0.0003 )
  ;
  
  printf("timer: %5.2f\n", timer.get_time());
  
  return 0;
}
#endif
