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
 * File   : cpoly.C                              *
 * Date   : March, 1997                          *
 * Author : Dan Hammer                           *
 * Classes for handling polygons, shapes         *
 * of the static variety                         *
 *************************************************/
#include <stdio.h>
#include <values.h>
#include <math.h>
#include <iostream>
#include <string.h>
#include "vmath.h"
#include "rendpoly.h"
#include "rndrpoly.h"
#include "rndzpoly.h"
#include "simerr.h"
#include "simfile.h"
#include "cpoly.h"

extern Vector world_light_source;
extern REAL_TYPE world_scale;
extern int frame_switch;

static char fcontext_buff[32];

extern int shape_cnt;
extern int poly_cnt;
int shape_cnt;
int poly_cnt;

static int point_cnt;

bool C_Poly::txtrflag = 1;

inline void set_context()
{
  sprintf(fcontext_buff,"S:%d P:%d R:%d",shape_cnt,poly_cnt,point_cnt);
  file_context = fcontext_buff;
}

void C_PolyInfo::read(std::istream &is)
{
  char c;
  int txtflag;
  char cc;

  set_context();

  READ_TOKI('[',is,c)
    is >> npoints >> cc;
  if (cc != '0' && cc != '1')
    {
      error_jump("%s: %s %c, Invalid TxtrFlag", current_path,fcontext_buff,cc);
    }

  txtflag = cc == '1';

  lpoints = new R_3DPoint[npoints];
  MYCHECK(lpoints != NULL);

  if (txtflag)
    {
      tpoints = new TxtPoint[npoints];
      MYCHECK(tpoints != NULL);
    }
  bcube.flg = 0;
  point_cnt = 0;
  for (int i=0;i <npoints;i++)
    {
      set_context();
      if (!txtflag)
	is >> lpoints[i];
      else
	{
	  char c1;
	  READ_TOKI('<',is,c1)
	    is >> lpoints[i].x >> lpoints[i].y >> lpoints[i].z;
	  is >> tpoints[i].u >> tpoints[i].v;
	  READ_TOK('>',is,c1);
	}
      point_cnt++;
      bcube.set(lpoints[i]);
    }
  READ_TOK(']',is,c)
    delete_flag = 1;
}


void C_PolyInfo::write(std::ostream &os)
{
  int txtflg = (tpoints != NULL);
  os << '[' << npoints << " " << txtflg << " ";
  if (!lpoints)
    {
      printf("lpoints is NULL!\n");
      return;
    }
  for (int i=0; i<npoints;i++)
    {
      if (!txtflg)
	{
	  R_3DPoint p = lpoints[i];
	  os << p << " ";
	}
      else
	{
	  os << "< " << lpoints[i].x << " " << lpoints[i].y << " ";
	  os << lpoints[i].z << " " << tpoints[i].u << " " << tpoints[i].v;
	  os << " >";
	}
    }
  os << ']' << '\n';
}


void C_PolyInfo::copy(const C_PolyInfo &ci)
{
  npoints = ci.npoints;
  if (lpoints && delete_flag)
    delete [] lpoints;
  lpoints = new R_3DPoint[npoints];
  for (int i=0;i<npoints;i++)
    lpoints[i] = ci.lpoints[i];
  if (tpoints && delete_flag)
    delete [] tpoints;
  tpoints = NULL;
  if (ci.tpoints != NULL)
    {
      tpoints = new TxtPoint[npoints];
      for (int i=0;i<npoints;i++)
	tpoints[i] = ci.tpoints[i];
    }
  bcube = ci.bcube;
  delete_flag = 1;
}

C_PolyInfo &C_PolyInfo::operator *=(REAL_TYPE scaler)
{
  if (lpoints != NULL)
    {
      bcube.flg = 0;
      for (int i=0;i<npoints;i++)
	{
	  lpoints[i] *= scaler;
	  bcube.set(lpoints[i]);
	}
    }
  return(*this);
}

C_PolyInfo &C_PolyInfo::operator +=(const R_3DPoint &p)
{
  if (lpoints != NULL)
    {
      bcube.flg = 0;
      for (int i=0;i<npoints;i++)
	{
	  lpoints[i] += p;
	  bcube.set(lpoints[i]);
	}
    }
  return(*this);

}

void C_ShapeInfo::read(std::istream &is)
{
  char c;
  set_context();
  READ_TOKI('{',is,c)
    is >> npolys;
  polyinfos = new C_PolyInfo[npolys];
  MYCHECK(polyinfos != NULL);
  bcube.flg = 0;
  poly_cnt = 0;
  for (int i=0;i<npolys;i++)
    {
      is >> polyinfos[i];
      bcube.set(polyinfos[i].bcube);
      poly_cnt++;
    }
  delete_flag = 1;
  c = '0';
  READ_TOKX('}','{',is,c)
}

void C_ShapeInfo::write(std::ostream &os)
{
  os << "{\n";
  os << npolys <<  "\n";
  for (int i=0;i<npolys;i++)
    os << polyinfos[i] ;
  os << "}\n";
}


void C_ShapeInfo::copy(const C_ShapeInfo &cs)
{
  if (polyinfos != NULL && delete_flag)
    delete [] polyinfos;
  npolys = cs.npolys;
  polyinfos = new C_PolyInfo[npolys];
  for (int i=0;i<npolys;i++)
    polyinfos[i] = cs.polyinfos[i];
  bcube = cs.bcube;
  delete_flag = 1;
}

void C_ShapeInfo::add(const C_PolyInfo &p)
{
  C_PolyInfo *tmp;
 
  tmp = new C_PolyInfo[npolys + 1];
  for (int i=0;i<npolys;i++)
    tmp[i] = polyinfos[i];
  if (delete_flag)
    delete [] polyinfos;
  polyinfos = tmp;
  npolys++;
  polyinfos[npolys-1] = p;
  bcube.set(p.bcube);
  delete_flag = 1;
}

C_ShapeInfo &C_ShapeInfo::operator *=(REAL_TYPE scaler)
{
  if (polyinfos != NULL)
    {
      bcube.flg = 0;
      for (int i=0;i<npolys;i++)
	{
	  polyinfos[i] *= scaler;
	  bcube.set(polyinfos[i].bcube);
	}
    }
  return (*this);
}

C_ShapeInfo &C_ShapeInfo::operator +=(const R_3DPoint &p)
{
  if (polyinfos != NULL)
    {
      bcube.flg = 0;
      for (int i=0;i<npolys;i++)
	{
	  polyinfos[i] += p;
	  bcube.set(polyinfos[i].bcube);
	}
    }
  return (*this);
}

void poly_params::read(std::istream &is)
{
  char c;
  READ_TOKI('(',is,c)
  if (is)
    is >> color >> tmap >> flags >> color_range >> c;
  READ_TOK(')',is,c)
}

void poly_params::write(std::ostream &os)
{
  os << '(' << color << " " << tmap << " " << flags << " " << color_range << ')';
}

void shape_params::read(std::istream &is)
{
  char c;
  if (p_params)
    delete [] p_params;
  p_params = NULL;
  
  READ_TOKI('[',is,c)
    
  unsigned int read_n_params;
    is >> flags >> read_n_params;
  n_params = static_cast<int>(read_n_params);
  p_params = new poly_params[static_cast<unsigned int>(n_params)];
  MYCHECK(p_params != NULL);

  if (flags & CPY_PARAMS)
    {
      poly_params tmp;
      is >> tmp;
      for (int i=0;i<n_params;i++)
	p_params[i] = tmp;
    }
  else
    for (int i=0;i<n_params;i++)
	is >> p_params[i];
    READ_TOK(']',is,c)
}

void shape_params::write(std::ostream &os)
{
  if (p_params)
    {
      flags &= ~(static_cast<unsigned long>(CPY_PARAMS));
      int copy = 1;
      for (int i=1;i<n_params;i++)
	{
	  if (! (p_params[i] == p_params[0]))
	    {
	      copy = 0;
	      break;
	    }
	}
      if (copy)
	flags |= CPY_PARAMS;
      os << "[\n" << flags << ' ' << n_params << '\n';
      if (flags & CPY_PARAMS)
	os << p_params[0] << '\n';
      else
	{
	  for (int i=0;i<n_params;i++)
	    os << p_params[i] << '\n';
	}
      os << "]\n";
    }
}

void shape_params::copy(const shape_params &sp)
{
  flags = sp.flags;
  n_params = sp.n_params;
  if (p_params)
    delete [] p_params;
  p_params = new poly_params[static_cast<unsigned int>(n_params)];
  for (int i=0;i<n_params;i++)
    p_params[i] = sp.p_params[i];
}

void shape_params::add(const poly_params &pm)
{
  if (p_params != NULL)
    {
      poly_params *tmp = new poly_params[static_cast<unsigned int>(n_params + 1)];
      for (int i=0;i<n_params;i++)
	tmp[i] = p_params[i];
      delete [] p_params;
      p_params = tmp;
    }
  else
    p_params = new poly_params[1]; 
  n_params++;
  p_params[n_params - 1] = pm;
}

void C_3DObjectInfo::readFile(char *pat)
{
  std::ifstream is;
  if (this->path)
    delete this->path;
  this->path = strdup(pat);
  if (open_is(is,pat))
    read(is);
}

void C_3DObjectInfo::writeFile(char *pat)
{
  std::ofstream os;
  MYCHECK(pat != NULL);
  if (this->path == NULL)
    this->path = strdup(pat);
  else if (strcmp(this->path,pat))
    {
      delete this->path;
      this->path = strdup(pat);
    }
  if (open_os(os,pat))
    write(os);
}

void C_3DObjectInfo::read(std::istream &is)
{
  char c;
  if (shapes)
    delete [] shapes;
  if (dflt_params)
    delete [] dflt_params;
  READ_TOKI('{',is,c)
    is >> id;
  is >> nshapes;
  // std::cout << id << ": " << nshapes << " shapes\n";
  shapes = new C_ShapeInfo[nshapes];
  MYCHECK(shapes != NULL);
  dflt_params = new shape_params[nshapes];
  MYCHECK(dflt_params != NULL);
  
  for (int i=0;i<nshapes;i++)
    {
      READ_TOKI('{',is,c)
	is >> shapes[i];
      is >> dflt_params[i];
      READ_TOKI('}',is,c)
	}
  
  READ_TOKI('}',is,c)
    
    }

void C_3DObjectInfo::write(std::ostream &os)
{
  if (shapes)
    {
      os << "{\n";
      os << id << '\n';
      os << nshapes;
      for (int i=0;i<nshapes;i++)
	{
	  os << shapes[i] << '\n';
	  os << dflt_params[i] << '\n';
	}
      os << "}\n";
    }
}


C_Poly::C_Poly()
  :color(0),
   p_info(NULL),
   wpoints(NULL),
   zpoints(NULL)
{
  flags = 0;
  color_range = 0;
  scale = 1.0;
}

int C_Poly::create(C_PolyInfo *p)
{
  p_info = p;
  //	wpoints = new R_3DPoint[npoints];
  //	return (wpoints != NULL);
  wpoints = NULL;
  return (1);
}


void C_Poly::set_params(poly_params *prms)
{
  color = prms->color;
  tmap = prms->tmap;
  flags = prms->flags;
  color_range = prms->color_range;
  base_color = color;
}

void C_Poly::set_params(const C_Poly &cp)
{
  color = cp.color;
  tmap  = cp.tmap;
  flags = cp.flags;
  color_range = cp.color_range;
  base_color  = cp.base_color;
}

std::istream & operator >>(std::istream &is, C_Poly &cp)
{
  char c;
  is >> c;
  while (is && c != '(')
    is >> c;
  if (is)
    {
      is >> cp.color >> cp.tmap >> cp.flags >> cp.color_range >> c;
      cp.base_color = cp.color;
      while (is && c != ')')
	is >> c;
    }
  return is;
}

C_Poly::~C_Poly()
{
  if (wpoints)
    delete [] wpoints;
  if (zpoints)
    delete [] zpoints;
}

void C_Poly::set_world_points(R_3DPoint &location)
{
  if (wpoints == NULL)
    wpoints = new R_3DPoint[p_info->npoints];
  MYCHECK(wpoints != NULL);
  for (int i = 0;i < p_info->npoints; i++)
    {
      R_3DPoint p = p_info->lpoints[i];
      if (scale != 1.0)
	{
	  p.x *= scale;
	  p.y *= scale;
	  p.z *= scale;
	}
      wpoints[i] = p + location;
    }
}

void C_Poly::getWorldPoint(R_3DPoint &wp, int i, R_3DPoint &location)
{
  if (i >= 0 && i < p_info->npoints)
    {
      wp = p_info->lpoints[i];
      wp *= scale;
      wp += location;
    }
}

void C_Poly::getBounds(bounding_cube & bcube)
{
  if (wpoints != NULL)
    {
      bcube.reset();
      for (int i=0;i<p_info->npoints;i++)
	bcube.set(wpoints[i]);
    }
}


void C_Poly::draw(Port_3D &port, int zclip)
{
  if (wpoints != NULL)
    {
      if (txtrflag && (flags & TXTR_MAP) && 
	  (p_info->tpoints != NULL) && (tmap >= 0))
	{
	  TextrMap *tmap_ptr = map_man->get_map_ptr(tmap);
	  if (tmap_ptr != &nullmap)
	    tmap_ptr->setColorMaps(base_color,color_range,color);
	  tr_rendpoly(wpoints,p_info->tpoints,p_info->npoints, port,color,tmap_ptr);
	}
      else if (zbuff != NULL && zbff_flag)
	rendzpoly(wpoints,p_info->npoints,color,port);
      else
	rendpoly(wpoints,p_info->npoints,color, port,zclip);
    }
}

int C_Poly::getScreenPoints(Port_3D &port, R_2DPoint *spoints)
{
  int result = 0;
  if (wpoints != NULL)
    result = rendzpoly(wpoints,p_info->npoints,color,port,spoints);
  return (result);
}

int C_Poly::getScreenPoints(Port_3D &port, TR_2DPoint *spoints)
{
  int result = 0;
  if (wpoints != NULL)
    {
      if (txtrflag && (flags & TXTR_MAP) && 
	  (p_info->tpoints != NULL) && (tmap >= 0))
	{
	  TextrMap *tmap_ptr = map_man->get_map_ptr(tmap);
	  if (tmap_ptr != &nullmap)
	    tmap_ptr->setColorMaps(base_color,color_range,color);
	  result = tr_rendpoly(wpoints,p_info->tpoints,p_info->npoints,
		      port,color,tmap_ptr,spoints);
	}
    }
  return (result);
}

// The 'xpoints' parameter, which defaults to null, allows me to
// use this routine with the 'oriented' poly, which will have
// already created a set of world points
void C_Poly::compute_plane_constant(R_3DPoint &loc, R_3DPoint *xpoints)
{
  Vector side1,side2;
  R_3DPoint p0,p1,p2;

  if (xpoints != NULL)
    {
      side1 = xpoints[1] - xpoints[0];
      side2 = xpoints[2] - xpoints[1];
    }
  else
    {
      p0 = p_info->lpoints[0];
      p1 = p_info->lpoints[1];
      p2 = p_info->lpoints[2];

      p0 *= scale;
      p1 *= scale;
      p2 *= scale;

      side1 = (p1 + loc) - (p0 + loc);
      side2 = (p2 + loc) - (p1 + loc);
    }

  surface_normal = side1.Cross(side2);
  surface_normal.Normalize();
  if (xpoints != NULL)
    plane_constant = -surface_normal.Dot(xpoints[0]);
  else
    plane_constant = -surface_normal.Dot(p0 + loc);
}

void C_Poly::set_poly_icolor(Vector &light)
{
  int col;
  float intensity;
  if (flags & I_SHADE)
    {
      float val = static_cast<float>(light.Dot(surface_normal)) + 1.0f;
      intensity = (val / 2.0f);
      if (intensity > 1.0f)
	intensity = 1.0f;
      col = (base_color + color_range - 1) - 
	static_cast<int>(intensity * (color_range - 1) );
      if (col <= base_color + color_range &&
	  col >= base_color)
	color = col;
    }
}


void C_Poly::render_shadow(Port_3D &theport, int z_clip,
			   REAL_TYPE z_value,
			   Vector &light_source,
			   int shadow_color)
{
  if (!(flags & SHADOW))
    return;
  set_shadow(z_value,light_source);
  draw_shadow(theport,z_clip,shadow_color);
  if (zpoints)
    {
      delete [] zpoints;
      zpoints = NULL;
    }
}

extern REAL_TYPE shadow_level;
void C_Poly::set_shadow(REAL_TYPE z_value, Vector &light_source)
{
  extern REAL_TYPE getGroundLevel(R_3DPoint &p);
  if (zpoints != NULL)
    delete [] zpoints;
  if (!wpoints)
    return;
  //  z_value = shadow_level;
  int npoints = p_info->npoints;
  zpoints = new R_3DPoint[npoints];
  if (zpoints != NULL)
    {
      float lx,ly,lz,z;
      lx = light_source.X;
      ly = light_source.Y;
      lz = light_source.Z;
      z = z_value;
      for (int i=0;i<npoints;i++)
	{
	  z = getGroundLevel(wpoints[i]) + shadow_level;
	  float wx,wy,wz;
	  float sx,sy,sz;
	  float t;
	  wx = wpoints[i].x;
	  wy = wpoints[i].y;
	  wz = wpoints[i].z;
	  float tmp = lz - wz;
	  if (tmp == 0.0)
	    tmp += 1E-7;
	  t = (z - wz) / tmp;
	  sx = wx + (t * (lx - wx));
	  sy = wy + (t * (ly - wy));
	  sz = z;
	  zpoints[i] = R_3DPoint(sx,sy,sz);
	}
    }
}

void C_Poly::draw_shadow(Port_3D &theport, int z_clip, int shadow_color)
{
  if (zpoints != NULL && (flags & SHADOW))
    {
      if (zbuff != NULL)
	rendzpoly(zpoints,p_info->npoints,shadow_color,theport);
      else
	rendpoly(zpoints,p_info->npoints,shadow_color,theport,z_clip);
    }
}

C_Shape::C_Shape()
  :npolys(0),
   polys(NULL)
{
  memopt = 1;
}

int C_Shape::create(C_ShapeInfo *ptr)
{
  int result = 1;
  info_ptr = ptr;
  npolys = info_ptr->npolys;
  polys = new C_Poly[npolys];
  if (polys != NULL)
    {
      for (int i=0;i<npolys;i++)
	{
	  if (!polys[i].create(&info_ptr->polyinfos[i]))
	    {
	      delete [] polys;
	      polys = NULL;
	      npolys = 0;
	      result = 0;
	      break;
	    }
	}
    }
  else
    {
      result = 0;
      npolys = 0;
    }
  return result;
}

void C_Shape::set_params(shape_params *prms)
{
  flags = prms->flags;
  for (int i=0;i<npolys;i++)
    polys[i].set_params(&prms->p_params[i]);
}

C_Shape::~C_Shape()
{
  if (polys)
    delete [] polys;
}

std::istream & operator >>(std::istream &is, C_Shape &cs)
{
  is >> cs.flags;
  if (cs.flags & CPY_PARAMS)
    {
      is >> cs.polys[0];
      for (int i=1;i<cs.npolys;i++)
	cs.polys[i].set_params(cs.polys[0]);
    }
  else
    {
      for (int i=0;i < cs.npolys;i++)
	is >> cs.polys[i];
    }
  return is;
}

void C_Shape::set_visible(Port_3D &theport)
{
  int i;
  visible = 0;
  for (i = 0; i<npolys;i++)
    {
      if (CheckPlaneEquation(Vector(theport.look_from),
			     polys[i].surface_normal,
			     polys[i].plane_constant))
	{
	  polys[i].visible = 1;
	  visible = 1;
	}
      else
	polys[i].visible = 0;
    }
}

void C_Shape::set_poly_color(int n, int color)
{
  if (n >= 0 && n < npolys)
    polys[n].color = color;
}

void C_Shape::set_poly_icolor(Vector & light)
{
  for (int i = 0; i < npolys; i++)
    polys[i].set_poly_icolor(light);
}


void C_Shape::set_world_location(R_3DPoint &loc)
{
  maxlen = 0;
  location = loc;
  for (int i =0;i<npolys;i++)
    {
      if (!memopt)
	polys[i].set_world_points(loc);
      polys[i].compute_plane_constant(loc);
    }
}

void C_Shape::set_scale(REAL_TYPE scl)
{
  for (int i=0;i<npolys;i++)
    polys[i].set_scale(scl);
}

void C_Shape::set_shadow(REAL_TYPE z, Vector &light)
{
  shadow_z = z;
  light_source = light;
  for (int i =0;i<npolys;i++)
    {
      polys[i].shadow_flag = 0;
      if (polys[i].flags & SHADOW)
	{
	  if (CheckPlaneEquation(light,polys[i].surface_normal,
				 polys[i].plane_constant))
            polys[i].shadow_flag = 1;
	}
    }
}

void C_Shape::draw_shadow(Port_3D &port )
{
  if (!visible || !(flags & SHADOW))
    return;
  for(int i=0;i<npolys;i++)
    if (polys[i].shadow_flag)
      {
	if (polys[i].wpoints == NULL)
	  polys[i].set_world_points(location);
	polys[i].render_shadow(port,
			       static_cast<int>(flags & Z_CLIPME),
			       shadow_z,
			       light_source,
			       0
			       );
      }
}

void C_Shape::draw(Port_3D &port, R_3DPoint &loc)
{
  int i;
  if (!visible)
    return;
  location = loc;
  for (i = 0; i <npolys;i++)
    {
      if (polys[i].visible)
	{
	  if (memopt || polys[i].wpoints == NULL)
	    polys[i].set_world_points(loc);
	  if (flags & Z_CLIPME)
	    polys[i].draw(port,1);
	  else
	    polys[i].draw(port,0);
	  if (memopt)
	    {
	      if (polys[i].wpoints != NULL)
		delete [] polys[i].wpoints;
	      polys[i].wpoints = NULL;
	    }
	}
      else if (memopt && polys[i].wpoints != NULL)
	{
	  delete [] polys[i].wpoints;
	  polys[i].wpoints = NULL;
	}
    }
}

void C_Shape::getBounds(bounding_cube &bcube)
{
  bounding_cube work;
  bcube.reset();
  for (int i=0;i<npolys;i++)
    {
      if (polys[i].wpoints == NULL)
	polys[i].set_world_points(location);
      polys[i].getBounds(work);
      if (memopt && polys[i].wpoints != NULL)
	{
	  delete [] polys[i].wpoints;
	  polys[i].wpoints = NULL;
	}
      bcube.set(work);
    }
}
