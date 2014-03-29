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
// step:  0.7854 *******
extern const float vtable[][3];
const float vtable[][3] = {
// theta:  0.8354 *****
{ -0.6437f,-0.7116f,-0.2815f },
{ -0.5888f,-0.6508f, 0.4794f },
{ -0.1889f,-0.2088f, 0.9595f },
{  0.3216f, 0.3555f, 0.8776f },
{  0.6437f, 0.7116f, 0.2815f },
{  0.5888f, 0.6508f,-0.4794f },
// theta:  1.6208 *****
{  0.0480f,-0.9584f,-0.2815f },
{  0.0439f,-0.8765f, 0.4794f },
{  0.0141f,-0.2812f, 0.9595f },
{ -0.0240f, 0.4788f, 0.8776f },
{ -0.0480f, 0.9584f, 0.2815f },
{ -0.0439f, 0.8765f,-0.4794f },
// theta:  2.4062 *****
{  0.7116f,-0.6437f,-0.2815f },
{  0.6508f,-0.5888f, 0.4794f },
{  0.2088f,-0.1889f, 0.9595f },
{ -0.3555f, 0.3216f, 0.8776f },
{ -0.7116f, 0.6437f, 0.2815f },
{ -0.6508f, 0.5888f,-0.4794f },
// theta:  3.1916 *****
{  0.9584f, 0.0480f,-0.2815f },
{  0.8765f, 0.0439f, 0.4794f },
{  0.2812f, 0.0141f, 0.9595f },
{ -0.4788f,-0.0240f, 0.8776f },
{ -0.9584f,-0.0480f, 0.2815f },
{ -0.8765f,-0.0439f,-0.4794f },
// theta:  3.9770 *****
{  0.6437f, 0.7116f,-0.2815f },
{  0.5888f, 0.6508f, 0.4794f },
{  0.1889f, 0.2088f, 0.9595f },
{ -0.3216f,-0.3555f, 0.8776f },
{ -0.6437f,-0.7116f, 0.2815f },
{ -0.5888f,-0.6508f,-0.4794f },
// theta:  4.7624 *****
{ -0.0480f, 0.9584f,-0.2815f },
{ -0.0439f, 0.8765f, 0.4794f },
{ -0.0141f, 0.2812f, 0.9595f },
{  0.0240f,-0.4788f, 0.8776f },
{  0.0480f,-0.9584f, 0.2815f },
{  0.0439f,-0.8765f,-0.4794f },
};

extern const int vsize;
const int vsize = 36;
