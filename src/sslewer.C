/*
    Sabre Fighter Plane Simulator 
    Copyright (c) 1997/1998 Dan Hammer
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
 *         Sabre Fighter Plane Simulator         *
 * File   : sslewer.C                            *
 * Date   : April, 1998                          *
 * Author : Dan Hammer                           *
 * base class for anything that can move and     *
 * and rotate in a 3-D world                     *
 *************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "sslewer.h"

void sSlewer::Rotate(const sAttitude &rotateMatrix)
{
  sAttitude attitude;
  GetAttitude(attitude);
  attitude.pitch = sIncrementAngle(attitude.pitch,rotateMatrix.pitch);
  attitude.roll = sIncrementAngle(attitude.roll,rotateMatrix.roll);
  attitude.yaw = sIncrementAngle(attitude.yaw,rotateMatrix.yaw);
  SetAttitude(attitude);
}

void sSlewer::TranslateWorld(const sVector &worldVector)
{
  sPoint position;
  GetPosition(position);
  position += worldVector;
  SetPosition(position);
}

void sSlewer::TranslateBody(const sVector &bodyVector)
{
  sVector worldVector;
  BodyVector2WorldVector(bodyVector,worldVector);
  TranslateWorld(worldVector);
}

void sSlewer::BodyVector2WorldVector(const sVector &body, sVector &world)
{
  sMatrix3Rotate(mat,body,world);
}

void sSlewer::WorldVector2BodyVector(const sVector &world, sVector &body)
{
  sMatrix3Rotate(invMat,world,body);
}

void sSlewer::BodyPoint2WorldPoint(const sPoint &body, sPoint &world)
{
  sPoint position;
  sVector worldVector;
  GetPosition(position);
  BodyVector2WorldVector(static_cast<const sVector &>(body),worldVector);
  world = position + worldVector;
}

void sSlewer::WorldPoint2BodyPoint(const sPoint &world, sPoint &body)
{
  sPoint position;
  sVector worldVector;
  sVector bodyVector;

  GetPosition(position);
  worldVector = world - position;
  WorldVector2BodyVector(worldVector,bodyVector);
  body = static_cast<sPoint &>(bodyVector);
}

void sSlewer::BuildDirectionNormal()
{
  sVector bodyNorm = sVector(0,1.0,0);
  BodyVector2WorldVector(bodyNorm,dirNormal);
}

void sSlewer::GetAttitudeToPoint(const sPoint &wPoint, sVector &direction, sAttitude &attitude)
{
  sPoint position;
  sVector bodyV;

  GetPosition(position);
  direction = wPoint - position;
  WorldVector2BodyVector(direction,bodyV);
  sVector2sAttitude(bodyV,attitude);
}

