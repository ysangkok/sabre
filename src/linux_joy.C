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
/*************************************************
 *           Sabre Fighter Plane Simulator              *
 * Version: 0.1                                  *
 * File   : linux_joy.C                          *
 * Date   : March, 1997                          *
 * Author : Dan Hammer                           *
 *          Additional programming by            *
 *          Pierre de Mouveaux                   *
 *************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <values.h>
// Define this in the Makefile if joystick drivers
// installed
#ifdef JSTICK_INSTALLED
#include <linux/joystick.h>
#else
#include "joydummy.h"
#endif
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <iostream>
#include <fstream>
#include "linux_joy.h"


const char *LinuxJoystick::joydef0 = "joy0.def";
const char *LinuxJoystick::joydef1 = "joy1.def";
const char *LinuxJoystick::joydev0 = "/dev/js0";
const char *LinuxJoystick::joydev1 = "/dev/js1";

const float LinuxJoystick::DEAD_ZONE = 0.05f;



#ifdef JOYCAL
inline const char *build_libpath(const char *s)
{
  return (s);
}

int main(int argc, char *argv[])
{
  int w;
  if (argc < 2)
    w = 0;
  else
    w = atoi(argv[1]);
  LinuxJoystick joystk(w,argc > 2 ? 0 : 1);
  if (joystk.calibrated)
    {
      joystk.close();
      printf("Testing joystick %d\n",joystk.joy);
      printf("Press any button to exit\n");
      if (joystk.open())
	{
	  while (joystk.update())
	    {
	      printf("raw_x: %d  raw_y: %d  cooked_x: %1.4f cooked_y: %1.4f \r",
		     joystk.raw_x,
		     joystk.raw_y,
		     joystk.cooked_x,
		     joystk.cooked_y);
	      if (joystk.buttons)
		break;
	    }
	}
      joystk.close();
    }
}
#else
extern char *build_libpath(const char *);
#endif

LinuxJoystick::LinuxJoystick(int which, int doCalibrate)
{
  joy = which;
  fd = -1;
  has_z = has_r = 0;
  naxis = 2;
  read_def_file(joy ? joydef1 : joydef0 );
  if (doCalibrate)
    calibrate();
  active = open();
}

int LinuxJoystick::read_def_file(const char *path)
{
  std::ifstream is(build_libpath(path));
  int i;

  if (is)
    {
      is >> cor_x >> cor_y >> max_x >> max_y >> min_x >> min_y;
      for (i=0;i<NUM_AXIS;i++)
	is >> dead_zone[i] >> scale[i] >> lin[i] >> symetrical[i] >> reverse[i];
    }
  else
    {
      fprintf(stderr,"linux_joystick: No def files found\n");
      cor_x = 3;
      cor_y = 3;
      max_x = 164;
      max_y = 146;
      min_x = 3;
      min_y = 3;
      for (int j=0;j<NUM_AXIS;j++) 
	{
	  lin[j] = 0;
	  dead_zone[j] = DEAD_ZONE;
	  scale[j] = 1.0f/(1.0f-dead_zone[j]);
	  symetrical[j] = 1;
	  reverse[j] = 1.0;
	}
    }
  return 1;
}

int LinuxJoystick::open()
{
  long tmpl;
  struct JS_DATA_TYPE js_data;
  int status;
  const char *devpath = joy ? joydev1 : joydev0;
  fd = ::open(devpath, O_RDONLY);
  if (fd < 0)
    {
      perror("LinuxJoystick");
      return(0);
    }
  js_data.x = cor_x;
  js_data.y = cor_y;
  
  status = ioctl(fd,JS_SET_CAL,&js_data);
  if (status == -1)
    {
      perror("LinuxJoystick");
      return 0;
    }
  tmpl = 1L;
  status = ioctl (fd, JS_SET_TIMELIMIT, &tmpl);
  if (status == -1) 
    {
      perror ("jscal");
      return 0;
    }
  return(1);
}

int LinuxJoystick::update()
{
  struct JS_DATA_TYPE js_data;
  
  if (read(fd, &js_data, JS_RETURN) > 0)
    {
      buttons = js_data.buttons;
      raw_x = js_data.x;
      raw_y = js_data.y;
      cooked_x = -1.0f + (((float)raw_x) / ((float) (min_x + max_x - min_x)) * 2.0f );
      cooked_y = -1.0f + (((float)raw_y) / ((float)(min_y + max_y - min_y)) * 2.0f );
      if (cooked_x < -1.0)
	cooked_x = -1.0;
      if (cooked_x > 1.0)
	cooked_x = 1.0;
      if (cooked_y < -1.0)
	cooked_y = -1.0;
      if (cooked_y > 1.0)
	cooked_y = 1.0;	
 
     if (symetrical[X_AXIS]) {
        cooked_x = reverse[X_AXIS]*shapeSym(X_AXIS,cooked_x);
      } else {
        cooked_x = reverse[X_AXIS]*shapeAsym(X_AXIS,cooked_x);
      }
 
      if (symetrical[Y_AXIS]) {
        cooked_y = reverse[Y_AXIS]*shapeSym(Y_AXIS,cooked_y);
      } else {
        cooked_y = reverse[Y_AXIS]*shapeAsym(Y_AXIS,cooked_y);
      }

      cooked_r = cooked_x;
      cooked_z = cooked_y;
      return (1);
    }
  else
    return (0);
}

inline float LinuxJoystick::shapeSym(unsigned int idx, float value)
{	
float temp;
  if (idx >= NUM_AXIS)
    return 0.0;
  if (value >= dead_zone[idx]) {
    temp = (value-dead_zone[idx])*scale[idx];
    if (lin[idx])
      return temp;
    else
      return temp*temp;
  } else
    if (value <= -dead_zone[idx]) {
      temp = (value+dead_zone[idx])*scale[idx];
      if (lin[idx])
        return temp;
      else
        return -temp*temp;
    } else 
      return 0.0;
}

inline float LinuxJoystick::shapeAsym(unsigned int idx, float value)
{	
  if (idx >= NUM_AXIS)
    return 0.0;

  float temp = (value + 1.0f)/2.0f;
  if (value >= dead_zone[idx]) {
    temp = (temp - dead_zone[idx])*scale[idx];
    if (lin[idx])
      return temp * 2.0f - 1.0f;
    else
      return temp * temp * 2.0f -1.0f;
  } 
  else 
    return -1.0;
}

void LinuxJoystick::setReverse(unsigned int idx,  int rev)
{
  if (idx>=NUM_AXIS)
	return;
  reverse[idx] = rev ? -1.0f : 1.0f;
}

void LinuxJoystick::close()
{
  if (fd > 0)
    ::close(fd);
}

void LinuxJoystick::setDeadZone(unsigned int idx,float zone)
{
  if (idx >= NUM_AXIS)
    return;
  if ((zone > 0.0) &&(zone > 0.5)) { // Let it work at least a minimum !!!
    dead_zone[idx] = zone;
    scale[idx] = 1.0f / (1.0f - zone);
  }
}

void LinuxJoystick::setLin(unsigned int idx, int _lin)
{
  if (idx > NUM_AXIS)
    return;
  lin[idx] = (_lin)?1:0;
}

void LinuxJoystick::calibrate()
{
  struct JS_DATA_TYPE js_data;
  long tmpl;
  int tmp;
  int status;
  char c;
  float deadZone;
  int i;

  const char *devpath = joy ? joydev1 : joydev0;
  fd = ::open(devpath, O_RDONLY);
  if (fd < 0)
    {
      perror("LinuxJoystick");
      return;
    }

  status = ioctl (fd, JS_GET_TIMEOUT, &tmp);
  if (status == -1) 
    {
      perror ("jscal");
      exit (1);
    }

  printf ("Timeout value = %d\n", tmp);

  status = ioctl (fd, JS_GET_TIMELIMIT, &tmpl);
  if (status == -1) 
    {
      perror ("jscal");
      return;
    }

  printf ("Timelimit value = %ld ms\nSetting Timelimit = 100 ms\n", tmpl);
 
  tmpl = 10;
  
  status = ioctl (fd, JS_SET_TIMELIMIT, &tmpl);
  if (status == -1) 
    {
      perror ("jscal");
      return;
    }
   status = ioctl (fd, JS_GET_CAL, &js_data);
  if (status == -1) 
    {
      perror ("jscal");
      return;
    }
  printf ("--------------------------\n");
  printf ("Calibrating joystick %d\n",joy);
  printf ("--------------------------\n\n");
  printf ("1. Getting correction values \n");
  printf ("Current correction: %d , %d\n", js_data.x, js_data.y);
  max_x = max_y = -MAXINT;
  printf("Move joystick back and forth several times,\npress any button to continue\n");
  while (1)
    {
      if (read(fd,&js_data,JS_RETURN) != JS_RETURN)
	perror("jscal");
      if (js_data.x > max_x)
	max_x = js_data.x;
      if (js_data.y > max_y)
	max_y = js_data.y;
      printf ("Got x = %d, y = %d   \r", js_data.x, js_data.y);
      if (js_data.buttons != 0x00)
	break;
    }

  for (tmp = 0; js_data.x > 0xff; tmp++, js_data.x = js_data.x >> 1);
  cor_x = tmp;
  for (tmp = 0; js_data.y > 0xff; tmp++, js_data.y = js_data.y >> 1);
  cor_y = tmp;

  printf ("Setting correction values to %d,%d\n\n",cor_x,cor_y);

  js_data.x = cor_x;
  js_data.y = cor_y;
  status = ioctl(fd,JS_SET_CAL,&js_data);
  if (status == -1)
    {
      perror("jscal");
      return;
    }

  printf ("2. Getting maximum & minimum values \n");

  min_x = min_y = MAXINT;
  max_x = max_y = -MAXINT;
  printf("Move joystick back and forth several times,\npress any button to continue\n");
  while(read(fd,&js_data,JS_RETURN) && js_data.buttons);

  while (1)
    {
      if (read(fd,&js_data,JS_RETURN) != JS_RETURN)
	perror("jscal");
      printf ("Got x = %d, y = %d  \r", js_data.x, js_data.y);
      if (js_data.x < min_x)
	min_x = js_data.x;
      if (js_data.y < min_y)
	min_y = js_data.y;
      if (js_data.x > max_x)
	max_x = js_data.x;
      if (js_data.y > max_y)
	max_y = js_data.y;
      if (js_data.buttons != 0x00)
	break;
    }
  printf("\nSetting maximum values to %d,%d\n\n",max_x,max_y);
  printf("\nSetting minimum values to %d,%d\n\n",min_x,min_y);
  printf("Do you wish to set the dead zones (y/n) ");
  c = (char) getchar();
  printf("\n");
  if (c=='Y'||c=='y')
    {
      printf("Getting Dead Zones\n");
      for (i=0;i<naxis;i++)
	{
	  deadZone = dead_zone[i];
	  printf("Dead zone for axis %d is %f\n",i,deadZone);
	  printf("Enter new value: (0 ... 1.0) >>");
	  scanf("%f",&deadZone);
	  printf("\n");
	  {
	    if (deadZone >= 0.0 && deadZone <= 1.0)
	      dead_zone[i] = deadZone;
	    scale[i] = 1.0f / (1.0f - deadZone);
	  }
	}
    }
  printf("Saving settings\n");
  write_def_file(joy ? build_libpath(joydef1) : build_libpath(joydef0));
  close();
}

void LinuxJoystick::write_def_file(const char *path)
{
  std::ofstream os(path);
  if (os)
    {
      os << cor_x << " " << cor_y << '\n';
      os << max_x << " " << max_y << '\n';
      os << min_x << " " << min_y << '\n';
      for (int i=0;i<NUM_AXIS;i++)
	os << dead_zone[i] << " " << scale[i] << " " << lin[i] << " " << symetrical[i]<< " " << reverse[i] << '\n';
    }
  else
    perror("joycal");
} 


