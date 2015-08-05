/*
    SABRE Fighter Plane Simulator 
    Copyright (c) 1997 Dan Hammer
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
 *           SABRE Fighter Plane Simulator       *
 * File   : unguided.h                           *
 * Date   : March, 1997                          *
 * Author : Dan Hammer                           *
 *************************************************/
#ifndef UNGUIDED_H
#define UNGUIDED_H

#include "target.h"
#include "viewobj.h"
#include "moveable.h"
#include "weapons.h"

class Rocket_Trail;

#define MAX_RADIUS_HITS 10

class Projectile : public Moveable
{
public:
  float max_time;
  float elapsed_time;
  FlightLight *flt;
  FlightLight_Specs *specs;
  int damage;
  int view_color;
  float view_len;
  bool is_tracer;
  Weapon_Specs *wspecs;
  Target *hit;
  Target *radius_hits[MAX_RADIUS_HITS];
  int    rad_hits;
  Target *launcher;
  Rocket_Trail *smk;
  int fuse_type;
  float fuse_data;
  float hitScaler;

  R_3DPoint t_position;
  R_3DPoint l_position,s_position;

  Projectile(FlightLight_Specs *spcs)
    : specs(spcs)
    {
      active = 0;
      flt = NULL;
      view = NULL;
      max_time = spcs->time_limit;
      damage = 1;
      view_len = 0.0;
      launcher = NULL;
      is_tracer = 1;
      wspecs = NULL;
      smk = NULL;
      rad_hits = 0;
      fuse_data = 0.0;
      fuse_type = 0;
    }
  virtual ~Projectile();
  virtual void create_view();
  virtual void init();
  void activate(Launch_Params &);
  void activate(R_3DPoint &, Vector &);
  void activate(R_3DPoint &r)
    { Moveable::activate(r); }
  void activate(Flight &f, Gun_Specs *gs, int n = 1)
    { Moveable::activate(f,gs,n); }
  void activate(Flight &f)
    { Moveable::activate(f); }
  virtual void de_activate();
  virtual void update();
  void pause()
    { if (flt) flt->pause(); }
  void start()
    { if (flt) flt->start(); }
  int check_for_hit();
  int check_radius_hits();
  virtual void add_draw_list(DrawList &dl, Port_3D &port);
  int has_damage_radius()
    {
      return (wspecs->getDamageRadius() > 0.0);
    }
  int wepType()
    {
      return (wspecs->getType());
    }
  Weapon_Specs *getWepSpecs()
    {
      return (wspecs);
    }
};

class BlastMap : public TMoveable
{
public:
  TextrMap *initmap;
  TextrMap *map;
  float    initdur;
  int      initflag;
  BlastMap()
    {
      initdur = 0.0;
      initflag = 0;
    }

  void init();
  virtual void update();
  virtual void de_activate();
  void add_draw_list(DrawList & dl, Port_3D & port);
  
  static float bm_sizex;
  static float bm_sizey;
  static float bm_dur;
  static const char *bm_map;
  static const char *bm_initmap;
  static float bm_initdur;
};

class DustUp : public TMoveable
{
public:
  void init();
  virtual void de_activate();
  float size;
  DustUp(float mtime = 0.55f)
    : TMoveable(mtime),
    size(0.13f)
    {}
};

#define MAX_VSMOKES 64

class Smokey : public TMoveable
{
public:
  int         n_smokes;
  Bitmap_View *views[MAX_VSMOKES];
  int         next_view;
  int         nview;
  const char        *smokemap_id;
  TextrMap    *smokemap;
  const char        *firemap_id;
  TextrMap    *firemap;
  float       init_size;
  float       d_size,d_len;
  float       kink;
  Vector      v;
  int         has_flame;
  float       flame_time_max;
  float       flame_time;
  int         flame_flag;
  int         nflames;

  R_3DPoint   cur_position;

  float       cur_size;

  static int   sm_n_smokes;
  static float sm_init_size;
  static float sm_d_size;
  static float sm_d_len;
  static float sm_kink;
  static float sm_max_time;
  static int   sm_has_flame;
  static float sm_flame_time_max;
  static int   sm_nflames;
  static const char  *sm_smokemap_id;
  static const char  *sm_firemap_id;
  

  Smokey();
  virtual ~Smokey();
  static void initSmokey();

  void setLocalParams();
  virtual void init();
  virtual void activate(R_3DPoint &);
  virtual void activate(Flight &flt)
    { TMoveable::activate(flt); }
  virtual void activate(R_3DPoint &p, Vector &vc)
    { TMoveable::activate(p,vc); }
  virtual void activate(Flight &launcher, Gun_Specs *g_specs, int n = 1)
    { TMoveable::activate(launcher,g_specs,n); }
  virtual void activate(Launch_Params &lp)
    { TMoveable::activate(lp); }

  virtual void de_activate();
  virtual void update();
  virtual void add_draw_list(DrawList &, Port_3D &);
};

class Smoke_Trail : public Smokey
{
public:
  Flight *flight;
  virtual void activate(Flight &);
  virtual void activate(R_3DPoint &r)
    { flame_flag = 0; Smokey::activate(r); }
  virtual void activate(R_3DPoint &r, Vector &v)
    { flame_flag = 0; Smokey::activate(r,v); }
  virtual void activate(Flight &launcher, Gun_Specs *g_specs, int n = 1)
    { flame_flag = 0; Smokey::activate(launcher,g_specs,n); }
  virtual void activate(Launch_Params &lp)
    { flame_flag = 0; Smokey::activate(lp); }

  void de_activate();
  void init();
  virtual void update();
  virtual void add_draw_list(DrawList &, Port_3D &);
  Smoke_Trail();
};

class Rocket_Trail : public Smoke_Trail
{
public:
  FlightLight *fltlte;
  Rocket_Specs *rspcs;
  Rocket_Trail(Rocket_Specs *rspc)
    :Smoke_Trail()
    {
      this->rspcs = rspc;
      fltlte = NULL;
    }
  virtual void activate(FlightLight *);
  virtual void update();

  virtual void activate(R_3DPoint &r)
    { flame_flag = 0; Smokey::activate(r); }
  virtual void activate(R_3DPoint &r, Vector &v)
    { flame_flag = 0; Smokey::activate(r,v); }
  virtual void activate(Flight &launcher, Gun_Specs *g_specs, int n = 1)
    { flame_flag = 0; Smokey::activate(launcher,g_specs,n); }
  virtual void activate(Launch_Params &lp)
    { flame_flag = 0; Smokey::activate(lp); }
  virtual void activate(Flight &f)
    { Smoke_Trail::activate(f); }
};

class Debris : public Projectile
{
public:
  FlightLight_Specs *global_specs;
  FlightLight_Specs *local_specs;
  Debris(FlightLight_Specs *spcs)
    : Projectile(spcs),
    global_specs(spcs),
    local_specs(NULL)
    {}
  virtual ~Debris()
    {
      if (local_specs != NULL)
	delete local_specs;
    }
  void de_activate();
  void init();
  void update();
  void create_view();
};

#define MAX_ROUNDS 340
#define MAX_FLAK   60
#define MAX_BLASTS 40
#define MAX_DUSTUPS 40
#define MAX_DEBRIS 220
#define MAX_SMOKES 40
#define MAX_STRAILS 40

class Unguided_Manager
{
public:
  FlightLight_Specs specs;
  FlightLight_Specs flak_specs;
  int nrounds;
  int active_count;
  Projectile *rounds[MAX_ROUNDS];
  Projectile *flak[MAX_FLAK];
  int nflak,active_flak;
  float rtime,elapsed_time;
  Unguided_Manager();
  ~Unguided_Manager();

  int update();
  int new_round(Launch_Params &);
  int new_flak(R_3DPoint &, Vector &);
  void add_draw_list(DrawList &, Port_3D &);
  int check_hits();
  void handle_hit(Target *hit, Projectile *p);
  void handle_radius_hits(Projectile *p);
  void create_debris(R_3DPoint &p, int l, int w, int n,
		     Vector_Q *vq = NULL);
  void pause();
  void start();
  void boom(R_3DPoint &origin, int flags);
  BlastMap *blasts[MAX_BLASTS];
  int nblasts;

  int new_blast(R_3DPoint &, float, float, float, const char * = "fire",
		const char * = "smoke", float = 1.0);

  int endProjectilePath(Projectile *p);

  DustUp *dustups[MAX_DUSTUPS];
  int ndustups;
  int new_dustup(R_3DPoint &);

  Smokey *smokeys[MAX_SMOKES];
  int new_smoke(R_3DPoint &p,
		int n_smokes,
		float init_size,
		float d_size,
		float d_len,
		float kink,
		float max_time,
		int has_flame,
		float flame_time_max,
		int n_flames,
		const char *smokmap_id,
		const char *firemap_id);

  Smoke_Trail *smoke_trails[MAX_STRAILS];
  int new_smoke_trail(Flight &);

  FlightLight_Specs debris_specs;
  Debris *debris[MAX_DEBRIS];

  Moveables m_rounds,
    m_flak,
    m_blasts,
    m_dustups,
    m_debris,
    m_smokes,
    m_strails;

  static Unguided_Manager *the_umanager;
  static Target          **targets;
  static int               ntargets;
  static int               inited;
  static void setTargetList(Target **ts, int n)
    {
      targets = ts;
      ntargets = n;
    }
  static void setUnguidedManager(Unguided_Manager *um)
    {
      the_umanager = um;
    }
  static void initUnguided();
};

#endif
