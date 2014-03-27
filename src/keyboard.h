/*
    Sabre Fighter Plane Simulator
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
 * Sabre Fighter Plane Simulator                 *
 * File   : keyboard.h                           *
 * Date   : December, 1998                       *
 * Author : Dan Hammer                           *
 * Buffered keyboard input                       *
 *************************************************/
#ifndef keyboard_h
#define keyboard_h

class sKeyboard
{
  static int keyBuff[256];
  static int keyInIndex;
  static int keyOutIndex;
  static unsigned char keyboard[256];
public:
  static void Init();
  static int KeyIsDown(int keyCode);
  static void Flush(void);
  static int GetNextSabreVKey(void);
  static void PushSabreVKey(int sabreVKeyCode);
  static void PressKey(int keyCode);
  static void ReleaseKeys(void);
  static void ReleaseKey(int keyCode);

  sKeyboard()
    {
      Init();
    }
};

#endif
