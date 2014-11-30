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
 * File   : obj_3d.h                             *
 * Date   : March, 1997                          *
 * Author : Dan Hammer                           *
 *************************************************/
#ifndef __obj_3d_h
#define __obj_3d_h

#include "cpoly.h"

class C_3DObject_Base
{
public:
  REAL_TYPE z_order;
  int visible_flag;
  int drawn_flag;
  int first_flag;
  int flags;
  bounding_cube bcube;
  REAL_TYPE max_sc;
  REAL_TYPE bounding_sphere;
  R_3DPoint location;
  int visible;
  static REAL_TYPE dab_min;

  C_3DObject_Base()
  {
    flags = 0;
    visible = 1;
  }
  virtual void draw(Port_3D &) = 0;
  virtual void translate(R_3DPoint &p)
    { location += p;}
  virtual REAL_TYPE draw_prep(Port_3D &);
  virtual class_types class_type()
    { return (C_3DObject_Base_t); }
  virtual void build_bounding_cube()
  {
  }

  virtual void calc_screen_bounds(Port_3D &, R_3DPoint * );
  virtual void calc_bounding_sphere();

  virtual ~C_3DObject_Base();
  C_3DObject_Base(const C_3DObject_Base& copy_from_me);
};

class C_3DPoly : public C_3DObject_Base
{
public:
  C_PolyInfo *info;
  C_Poly      poly;
  R_3DPoint location;
  REAL_TYPE scale;
  poly_params *param;

  C_3DPoly()
  {
    info = NULL;
    scale = 1.0;
    bounding_sphere = 0.0;
  }

  C_3DPoly(C_PolyInfo *an_info, R_3DPoint & a_location,
	   poly_params *a_param, REAL_TYPE a_scale = 1.0);

  virtual class_types class_type()
  { return (C_3DPoly_t); }
  virtual void draw(Port_3D &);
  virtual REAL_TYPE draw_prep(Port_3D &);
  virtual void translate(R_3DPoint &r);
  void build_bounding_cube();
  void create_poly(C_Poly &);
};


class DrawList;
class C_3DObject_List;
class C_3DObject_Group;

class C_3DObject_List
{
public:
  C_3DObject_Base *objects[MAX_OBJECTS];
  unsigned int n_objects;
  int object_idx;
  C_3DObject_List()
  { n_objects = 0; object_idx = -1; }
  int add_object(C_3DObject_Base *);
  void clear()
  { n_objects = 0; object_idx = -1; }
} ;

class DrawList  : public  C_3DObject_List
{
public:
  void draw(Port_3D &);
  void drawz(Port_3D &);
} ;

class C_3DInfoManager
{
public:
  C_3DObjectInfo *infos;
  unsigned int ninfos;
  char *path;

  C_3DInfoManager()
    : infos(NULL),
      ninfos(0),
      path(NULL)
    {}

  ~C_3DInfoManager()
    {
      if (infos)
	delete [] infos;
      if (path)
	delete path;
    }

  C_3DObjectInfo *getInfo(int );
  C_3DObjectInfo *getInfo(char *);

  void readFile(char *path);
  void writeFile(char *path);
  void read(std::istream &);
  void write(std::ostream &);
  friend inline std::istream &operator >>(std::istream &, C_3DInfoManager &);
  friend inline std::ostream &operator <<(std::ostream &, C_3DInfoManager &);
} ;

std::istream &operator >>(std::istream &is, C_3DInfoManager &cm)
{
  cm.read(is);
  return(is);
}

std::ostream &operator <<(std::ostream &os, C_3DInfoManager &cm)
{
  cm.write(os);
  return(os);
}

class C_3DObject2 : public C_3DObject_Base, public Target
{
public:
  unsigned int             nshapes;
  C_Shape         *shapes;
  bounding_cube   *bcubes;
  R_3DPoint       world_origin;
  R_3DPoint       w_origin;
  REAL_TYPE       scale;
  C_3DInfoManager *im;
  C_3DObjectInfo  *ci;
  char            obj_id[16];
  unsigned int    hit_shape;
  R_3DPoint       hit_point;
  bool            txtr_flag;

  C_3DObject2()
    :shapes(NULL),
     bcubes(NULL),
     im(NULL),
     ci(NULL)
    {
      visible = 1;
      hurt = 0;
      scale = 1.0;
      bounding_sphere = 0.0;
      hit_shape = 0;
      txtr_flag = 1;
    }

  
  C_3DObject2(C_3DInfoManager *im2)
    :shapes(NULL),
     bcubes(NULL),
     ci(NULL)
  {
    this->im = im2;
    visible = 1;
    hurt = 0;
    scale = 1.0;
    bounding_sphere = 0.0;
    hit_shape = 0;
    txtr_flag = 1;
  }

  ~C_3DObject2()
    {
      if (shapes)
	delete [] shapes;
      if (bcubes)
	delete [] bcubes;
    }

  void setIm(C_3DInfoManager *im2)
    {
      this->im = im2;
    }

  virtual class_types class_type()
  { return (C_3DObject_t); }
  virtual void draw(Port_3D &);
  virtual REAL_TYPE draw_prep(Port_3D &);
  virtual void translate(R_3DPoint &r);
  virtual int ouch(const R_3DPoint &, int, Target * = NULL);
  virtual int ouch(const R_3DPoint &, const R_3DPoint &, int, Target * = NULL);
  virtual int ouch(const R_3DPoint &, float, int, Target * = NULL);
  void build_bounding_cube();
  R_3DPoint *get_hit_point();
  R_3DPoint *get_position();
  virtual int getType()
    { 
      return (C_3DOBJECT_T); 
    }

  void read(std::istream &);
  void write(std::ostream &);
  friend inline std::istream &operator >>(std::istream & is, C_3DObject2 &ob);
  friend inline std::ostream &operator <<(std::ostream & os, C_3DObject2 &ob);
};

std::istream & operator >>(std::istream &is, C_3DObject2 &ob)
{
  ob.read(is);
  return(is);
}

std::ostream & operator <<(std::ostream &os, C_3DObject2 &ob)
{
  ob.write(os);
  return(os);
}

class C_DrawList2
{
public:
  C_3DObject2           *objects;
  C_3DInfoManager       *im;
  C_3DObject_Group      *groups;
  unsigned int n_shapes,n_objects,n_groups;

  C_DrawList2(C_3DInfoManager *im2)
    : objects(NULL),
      groups(NULL),
      n_shapes(0),
      n_objects(0),
      n_groups(0)
  { 
    this->im = im2;
  }

  ~C_DrawList2();
  friend std::istream & operator >>(std::istream &is, C_DrawList2 &cs);
  void read(std::istream &is);
  void translate(R_3DPoint &);
  void add_draw_list(DrawList &, Port_3D &port);
  void get_targets(Target_List &);
  void setTextrFlag(int flag, const char *id, int id_len);
  void setVisibleFlag(int flag, const char *id, int id_len);
};

#endif
