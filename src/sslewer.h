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
 * File   : sslewer.h                            *
 * Date   : April, 1998                          *
 * Author : Dan Hammer                           *
 * base class for anything that can move and     *
 * and rotate in a 3-D world                     *
 *************************************************/
#ifndef SSLEWER_H
#define SSLEWER_H

#ifdef __cplusplus
#include "smath.h"
#include "sobject.h"

class sSlewer : public sObject
{
public:

  sSlewer()
    {}

  virtual bool IsA() const
    {
      return (sSLEWER_T);
    }

  virtual void SetPosition(const sPoint &position)
    {
      pos = position;
    }
  void GetPosition(sPoint &position)
    {
      position = pos;
    }
  virtual void SetAttitude(const sAttitude &attitude)
    {
      att = attitude;
      BuildMatrixes();
    }
  void GetAttitude(sAttitude &attitude)
    {
      attitude = att;
    }
  void GetPositionAndAttitude(sPoint &position, sAttitude &attitude)
    {
      position = pos;
      attitude = att;
    }
  virtual void SetPositionAndAttitude(const sPoint &position, const sAttitude &attitude)
    {
      pos = position;
      att = attitude;
      BuildMatrixes();
    }

  void GetDirectionNormal(sVector &directionNormal)
    {
      directionNormal = dirNormal;
    }

  virtual void Rotate(const sAttitude &rotateMatrix);
  void TranslateWorld(const sVector &worldVector);
  void TranslateBody(const sVector &bodyVector);

  void BodyVector2WorldVector(const sVector &body, sVector &world);
  void WorldVector2BodyVector(const sVector &world, sVector &body);
  void BodyPoint2WorldPoint(const sPoint &body, sPoint &world);
  void WorldPoint2BodyPoint(const sPoint &world, sPoint &body);
  void GetAttitudeToPoint(const sPoint &wPoint, sVector &direction, sAttitude &attitude);
protected:
  sPoint    pos;
  sAttitude att;
  sVector   mat[3];
  sVector   invMat[3];
  sVector   dirNormal;

  void BuildMatrixes()
    {
      sMatrix3Build(att,mat);
      sMatrix3Transpose(mat,invMat);
      BuildDirectionNormal();
    }
  void BuildDirectionNormal();
};

#endif

#endif
