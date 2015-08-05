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
/*************************************************************
 * grafix.h                                                  *
 *************************************************************/
#ifndef GRAFIX_H
#define GRAFIX_H

#include <utility>
#include <cstdlib>

#ifndef DEFS_H
#include "defs.h"
#endif

int limit(int x, int r);

class Point
{
public:
  int x;
  int y;
  Point() { x = y = 0; }
  Point(int _x, int _y) : x(_x),y(_y) { }
  Point(unsigned int _x, unsigned int _y) : x(static_cast<int>(_x)),y(static_cast<int>(_y)) { }
  friend inline Point operator +(const Point &a, const Point &b);
  friend inline Point operator -(const Point &a, const Point &b);
  friend inline bool operator ==(const Point &a, const Point &b);
  friend inline bool operator !=(const Point &a, const Point &b);
  inline Point & operator +=(const Point &p);
};

inline int limit(int x, int l)
{
  if (abs(x) > l)
    {
      if (x < 0)
	return -l;
      else
	return l;
    }
  else
    return x;
}

inline Point & Point::operator +=(const Point &p)
{
  x += p.x;
  y += p.y;
  return *this;
}

inline Point operator +(const Point &a, const Point &b)
{
  return Point(a.x + b.x, a.y + b.y);
}

inline Point operator -(const Point &a, const Point &b)
{
  return Point(a.x - b.x, a.y - b.y);
}

inline bool operator ==(const Point &a, const Point &b)
{
  return ((a.x == b.x) && (a.y == b.y));
}

inline bool operator !=(const Point &a, const Point &b)
{
  return ((a.x != b.x) || (a.y != b.y));
}

class Rect
{
public:
  Point topLeft;
  Point botRight;
  Rect() { }
  Rect(const Rect &r)
    { topLeft = r.topLeft; botRight = r.botRight; }
  Rect(Point tL, Point bR) : topLeft(tL), botRight(bR) { }
  Rect(int x, int y, int x1, int y1) : topLeft(x,y), botRight(x1,y1) { }
  Rect &operator =(const Rect &r)
    { topLeft = r.topLeft; botRight = r.botRight; return *this; }
  int left()
    { return topLeft.x; }
  int right()
    { return botRight.x; }
  int top()
    { return topLeft.y; }
  int bottom()
    { return botRight.y; }
};

inline bool is_visible(Rect &r, int x, int y)
{
  return
    (
     (x >= r.topLeft.x) &&
     (x <= r.botRight.x) &&
     (y >= r.topLeft.y) &&
     (y <= r.botRight.y)
     );
}

inline bool valid_rect(Rect &r)
{
  return
    (
     r.topLeft.x <= r.botRight.x
     &&
     r.topLeft.y <= r.botRight.y
     );
}

inline void cliprect2rect(const Rect &cr, Rect &r)
{
  if (r.topLeft.x < cr.topLeft.x)
    r.topLeft.x = cr.topLeft.x;
  if (r.botRight.x > cr.botRight.x)
    r.botRight.x = cr.botRight.x;
  if (r.topLeft.y < cr.topLeft.y)
    r.topLeft.y = cr.topLeft.y;
  if (r.botRight.y > cr.botRight.y)
    r.botRight.y = cr.botRight.y;
}

template<typename T>
typename std::make_unsigned<T>::type myabs(T x)
{
    return x < 0 ? -static_cast<decltype(myabs(x))>(x) : static_cast<decltype(myabs(x))>(x);
}

inline unsigned int RWIDTH(const Rect &r)
{
  return myabs(r.botRight.x - r.topLeft.x + 1);
}

inline unsigned int RHEIGHT(const Rect &r)
{
  return myabs(r.botRight.y - r.topLeft.y + 1);
}

#endif
