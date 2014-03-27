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
 *           Sabre Fighter Plane Simulator       *
 * Version: 0.1                                  *
 * File   : zview.h                              *
 * Date   : March, 1997                          *
 * Author : Dan Hammer                           *
 * A class for defining and rendering an object  *
 * level depth sort.                             *
 *************************************************/
#ifndef __zview_h
#define __zview_h

#define MAX_ZSHAPES 16
#define MAX_ZNODES  20
#define MAX_ZPTRS   20
#define R_TOP 0
#define R_RIGHT 1
#define R_BACK 2

#ifndef __copoly_h
#include "copoly.h"
#endif

#ifndef __obj_3d_h
#include "obj_3d.h"
#endif


typedef enum switch_types { sw_init, sw_top, sw_right, sw_front };
extern Vector world_light_source;

class Z_Viewer;
class Z_Node_Manager;

class Z_Node_Manager
{
public:
  C_ShapeInfo *reference_info;
  C_ShapeInfo *shape_info;
  C_Oriented_Shape *reference_shape;
  C_Oriented_Shape *shapes;
  shape_params *s_params;
  int n_reference;
  int n_shapes;
  C_Oriented_Shape *cur_shapes;
  C_Oriented_Shape *cur_reference;
  Port_3D *cur_port;
  R_3DPoint r;
  REAL_TYPE scaler;
  char *id; 

  Z_Node_Manager();
  ~Z_Node_Manager();

  friend std::istream & operator >>(std::istream &, Z_Node_Manager &);
  void read(std::istream &is);
  int read_file(char *);
  void create();
  friend std::ostream & operator <<(std::ostream &, Z_Node_Manager &);
  void write(std::ostream &os);
  int write_file(char *);
  void add(C_ShapeInfo &new_shape, shape_params &new_params);
  void write_params(std::ostream &os, int);
  void deleteShape(int which);
  void buildShapes();
  void setId(char *);

  static Z_Node_Manager **g_zmanagers;
  static int nzmanagers;
  static Z_Node_Manager *getZNode(char *);
};


class Z_Viewer : public C_3DObject_Base
{
public:
  //  R_3DPoint position;
  int draw_shadow;
  Port_3D *reference_port;
  C_Oriented_Shape *reference_shape;
  C_Oriented_Shape *shapes;
  virtual void draw_minimal(Port_3D &port);
  virtual void draw_partial(Port_3D &port);
  int nshapes;
  int nreference;
  // distance abstraction levels
  enum distance_abstraction_type { da_full, da_partial, da_minimal };
  // current level
  distance_abstraction_type da_level;
  // minimum screen width to qualify as full
  REAL_TYPE min_full;
  // minimum screen width to qualify as partial
  REAL_TYPE min_partial;
  // minimum screen width
  REAL_TYPE min_sw;
  Z_Node_Manager *z_manager;
  Z_Viewer()
    {
      reference_port = NULL;
      z_manager = NULL;
      shapes = NULL;
    }
  Z_Viewer(Z_Node_Manager *zm);
  ~Z_Viewer();

  void init(Z_Node_Manager *);
  void setManager(Z_Node_Manager *zm);
    
  virtual class_types class_type()
  { return Z_Viewer_t; }
  void draw(Port_3D &);
  void translate(R_3DPoint &){   };
  REAL_TYPE draw_prep(Port_3D &);
  void build_bounding_cube();
  void calc_screen_bounds(Port_3D &, R_3DPoint *);
  friend std::ostream &operator <<(std::ostream &os, Z_Viewer &zv);
  void write(std::ostream &os);
  void setReferencePort(Port_3D *prt)
    {
      reference_port = prt;
    }
};

#endif
