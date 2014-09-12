/*
    Sabre Flight Simulator 
    Copyright (C) 1997	Dan Hammer

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
 *           Sabre Flight Simulator              *
 * Version: 0.1                                  *
 * File   : input.h                              *
 * Date   : March, 1997                          *
 * Author : Dan Hammer                           *
 *************************************************/
#ifndef __input_h
#define __input_h

#include "linux_joy.h"

class InputDevice
{
public:
  virtual ~InputDevice();
  virtual int IsAvailable( void ) const = 0;
  virtual void Update( void ) = 0;
};

class InputDevice_2D : public InputDevice
{
public:
  float x, y;
  int  screenx, screeny;
  int buttons;
public:
  InputDevice_2D()
    {}
  virtual ~InputDevice_2D();
  virtual float GetX()
    { return x; }
  virtual float GetY()
    { return y; }
  virtual int GetButtons( void ) const { return buttons; }
  virtual int GetScreenX(void ) const { return screenx; }
  virtual int GetScreenY(void ) const { return screeny; }
};

class Joystick : public InputDevice_2D
{
protected:
  float z,r;
public:
  LinuxJoystick ljoy;
  int HasZ()
    {
      return (ljoy.has_z);
    }
  int HasR()
    {
      return (ljoy.has_r);
    }
  int NAxis()
    {
      return (ljoy.naxis);
    }

public:
  Joystick(int js)
    : ljoy(js)
    {}

  virtual ~Joystick();// { ljoy.close(); }

  int IsAvailable() const
    {
      return (ljoy.active);
    }

  float GetZ()
    { return z; }
  float GetR()
    { return r; }

  void Update()
    {
      ljoy.update();
      x = ljoy.cooked_x;
      y = ljoy.cooked_y;
      z = ljoy.cooked_z;
      r = ljoy.cooked_r;
      screenx = ljoy.raw_x;
      screeny = ljoy.raw_y;
      buttons = ljoy.buttons;
    }

};

class Mouse : public InputDevice_2D
{
private:
  int ResetDriver( void ) const;

public:
  virtual void Update( void );
  virtual int IsAvailable( void ) const { return ResetDriver(); }
  ~Mouse();
  void set_position(int x, int y);
};

#endif
