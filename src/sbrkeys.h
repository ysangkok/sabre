/*
    Sabre Fighter Plane Simulator
    Copyright (c) 1997/98 Dan Hammer

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
 * Sabre Fighter Plane Simulator                 *
 * File   : sbrkeys.h                            *
 * Date   : December, 1998                       *
 * Author : Dan Hammer                           *
 *************************************************/
#ifndef __sbrkeys_h
#define __sbrkeys_h

#include "pc_keys.h"

class SbrKeyboard
{
  static int keyBuff[256];
  static int keyInIndex;
  static int keyOutIndex;
public:
  static void Init();
  static void Flush(void);
  static int GetNextSabreVKey(void);
  static void PushSabreVKey(int sabreVKeyCode);
  static void PressKey(int keyCode);
  SbrKeyboard()
    {
      Init();
    }
};

#endif
