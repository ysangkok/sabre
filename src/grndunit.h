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
 * File   : grndunit.h                           *
 * Date   : September, 1997                      *
 * Author : Dan Hammer                           *
 *                                               *
 *************************************************/
#ifndef __grndunit_h
#define __grndunit_h

#include "target.h"
#include "zview.h"

enum ground_unit_types { base, AAA, armor };

class Ground_Unit_Specs 
{
public:
  int type;
  char id[16];
  char shape_file[13];
  float update_time;
  float acquire_range;
  float max_speed;
  float avg_speed;
  int max_damage;
  R_3DPoint view_point;
  Z_Node_Manager *zm;
  void           *specs_extra;
  Ground_Unit_Specs()
    {
      update_time = acquire_range = 
      max_speed = avg_speed = 0.0;
      zm = NULL;
      specs_extra = NULL;
    }

  ~Ground_Unit_Specs();

  void read(std::istream &);
  void write(std::ostream &);

  friend std::istream &operator >>(std::istream &is, Ground_Unit_Specs &gsp)
    {
      gsp.read(is);
      return (is);
    }
  friend std::ostream &operator <<(std::ostream &os, Ground_Unit_Specs &gsp)
    {
      gsp.write(os);
      return (os);
    }
};

class Ground_Unit : public Z_Viewer, public Target
{
public:
  Ground_Unit_Specs    *specs;
  Port_3D              ref_port;

  int view;
  int hit_shape;
  float current_speed;

  Ground_Unit()
    {
      specs = NULL;
    }

  Ground_Unit(Ground_Unit_Specs *spcs)
    {
      specs = spcs;
    }

  Ground_Unit(const Ground_Unit &gu) : Z_Viewer(gu)
    {
      copy(gu);
    }

  Ground_Unit &operator =(const Ground_Unit &gu)
    {
      copy(gu);
      return (*this);
    }

  virtual ~Ground_Unit();

  void copy(const Ground_Unit &gu);
  void init();
  virtual void update(Unguided_Manager *, Target_List &);
  void add_draw_list(DrawList &, Port_3D &);

  virtual class_types class_type()
    { return Ground_Unit_t; }
  virtual int getType()
    { return (GROUND_UNIT_T); }

  int ouch(const R_3DPoint &, const R_3DPoint &, int, Target * = NULL);
  int ouch(const R_3DPoint & ,float , int, Target * = NULL);
  R_3DPoint *get_hit_point();
  R_3DPoint *get_position();

  void updatePosition();
  void setPosition();
  virtual void read(std::istream &);
  virtual void write(std::ostream &);
  friend std::istream &operator >>(std::istream &, Ground_Unit &);
  friend std::ostream &operator <<(std::ostream &, Ground_Unit &);
};

class AAA_Unit : public Ground_Unit
{
public:
  int hits;
  int kills;
  Target *target;
  Gun_Specs *gspcs;
  float elapsed_time;
  float target_distance;
  R_3DPoint aim_point;

  AAA_Unit()
    {
      hits = kills = 0;
      target = NULL;
      elapsed_time = 0.0;
    }
  
  AAA_Unit(Ground_Unit_Specs *spcs)
    :Ground_Unit(spcs)
    {
      hits = kills = 0;
      target = NULL;
      gspcs = (Gun_Specs *) spcs->specs_extra;
      elapsed_time = 0.0;
    }
  
  Target *selectTarget(Target_List &);
  void trackTarget(Unguided_Manager *);
  void calcLead(R_3DPoint &, R_3DPoint &);
  void youHit(Target *tg);
  void update(Unguided_Manager *, Target_List &);
  void read(std::istream &);
  void write(std::ostream &);
};

class Ground_Unit_Manager
{
public:
  Ground_Unit_Specs *specs;
  int               nspecs;
  Ground_Unit        **units;
  int               nunits;
  Weapons_Manager    wm;

  Ground_Unit_Manager();
  ~Ground_Unit_Manager();

  void update(Unguided_Manager *, Target_List &);
  void add_draw_list(DrawList &, Port_3D &);

  void read(std::istream &);
  friend std::istream &operator >>(std::istream &, Ground_Unit_Manager &);
  void read_file(const char *);

  void write(std::ostream &);
  friend std::ostream &operator <<(std::ostream &, Ground_Unit_Manager &);
  void write_file(const char *);
  void addTargetList(Target_List &);
  Ground_Unit_Specs *getSpecs(char *);
};

#endif

