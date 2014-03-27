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
float vtable[][3] = {
// theta:  0.8354 *****
{ -0.6437,-0.7116,-0.2815 },
{ -0.5888,-0.6508, 0.4794 },
{ -0.1889,-0.2088, 0.9595 },
{  0.3216, 0.3555, 0.8776 },
{  0.6437, 0.7116, 0.2815 },
{  0.5888, 0.6508,-0.4794 },
// theta:  1.6208 *****
{  0.0480,-0.9584,-0.2815 },
{  0.0439,-0.8765, 0.4794 },
{  0.0141,-0.2812, 0.9595 },
{ -0.0240, 0.4788, 0.8776 },
{ -0.0480, 0.9584, 0.2815 },
{ -0.0439, 0.8765,-0.4794 },
// theta:  2.4062 *****
{  0.7116,-0.6437,-0.2815 },
{  0.6508,-0.5888, 0.4794 },
{  0.2088,-0.1889, 0.9595 },
{ -0.3555, 0.3216, 0.8776 },
{ -0.7116, 0.6437, 0.2815 },
{ -0.6508, 0.5888,-0.4794 },
// theta:  3.1916 *****
{  0.9584, 0.0480,-0.2815 },
{  0.8765, 0.0439, 0.4794 },
{  0.2812, 0.0141, 0.9595 },
{ -0.4788,-0.0240, 0.8776 },
{ -0.9584,-0.0480, 0.2815 },
{ -0.8765,-0.0439,-0.4794 },
// theta:  3.9770 *****
{  0.6437, 0.7116,-0.2815 },
{  0.5888, 0.6508, 0.4794 },
{  0.1889, 0.2088, 0.9595 },
{ -0.3216,-0.3555, 0.8776 },
{ -0.6437,-0.7116, 0.2815 },
{ -0.5888,-0.6508,-0.4794 },
// theta:  4.7624 *****
{ -0.0480, 0.9584,-0.2815 },
{ -0.0439, 0.8765, 0.4794 },
{ -0.0141, 0.2812, 0.9595 },
{  0.0240,-0.4788, 0.8776 },
{  0.0480,-0.9584, 0.2815 },
{  0.0439,-0.8765,-0.4794 },
};

int vsize = 36;
