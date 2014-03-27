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
/*********************************************************
 * weapons.h                                             *
 *********************************************************/
#ifndef __weapons_h
#define __weapons_h

#include <string.h>
#include "fltlite.h"

class Target;
class Z_Node_Manager;
class Z_Viewer;
class Port_3D;

enum weapon_types { gun_t, rocket_t, bomb_t, missile_t, 
		    fueltank_t, cannon_t };

enum fuse_types { f_impact, f_timed, f_altitude };

class Weapon_Specs
{
public:
  char wep_name[32];
  FlightLight_Specs flt_specs;
  float min_range;
  float max_range;
  int damage_factor;
  int wep_type;
  void read(std::istream &is);
  void write(std::ostream &os);
 
  Weapon_Specs()
    {
      min_range = 0.0;
      max_range = 1000.0;
      wep_type = -1;
    }

  friend std::istream &operator >>(std::istream &is, Weapon_Specs &ws)
    {
      ws.read(is);
      return(is);
    }
  friend std::ostream &operator <<(std::ostream &os, Weapon_Specs &ws)
    {
      ws.write(os);
      return(os);
    }
  FlightLight_Specs *getFltSpecs()
    {
      return (&flt_specs);
    }
  virtual Z_Node_Manager *getZNodeMgr()
    {
      return (NULL);
    }
  virtual float getDamageRadius()
    {
      return (0.0);
    }
  int getType()
    {
      return (wep_type);
    }
  virtual int hasBlastInfo()
    {
      return (0);
    }
  virtual float getBlastMapSize()
    {
      return (0.0);
    }
  virtual float getBlastDur()
    {
      return (0.0);
    }
  virtual char *getBlastMap()
    {
      return (NULL);
    }
  virtual char *getSmokeMap()
    {
      return (NULL);
    }
};

class Gun_Specs : public Weapon_Specs
{
public:
  int tracer_color;
  int tracer_freq;
  float rounds_per_second;
  int max_rounds;

  Gun_Specs()
    {
      wep_type = gun_t;
      rounds_per_second = 0.0;
    }
  void read(std::istream &is);
  void write(std::ostream &os);
  friend std::istream &operator >>(std::istream &is, Gun_Specs &ws)
    {
      ws.read(is);
      return(is);
    }
  friend std::ostream &operator <<(std::ostream &os, Gun_Specs &ws)
    {
      ws.write(os);
      return(os);
    }
};

class Cannon_Specs : public Gun_Specs
{
public:
  char blast_map[32];
  char smoke_map[32];
  float blast_radius;
  float blast_map_size;
  float blast_dur;

  Cannon_Specs()
    {
      wep_type = cannon_t;
      wep_type = bomb_t;
      blast_radius = 1.0;
      blast_map_size = 1.0;
      blast_dur = 1.0;
      strcpy(blast_map,"fire");
      strcpy(smoke_map,"smoke");
    }

  virtual float getDamageRadius()
    {
      return(blast_radius);
    }
  virtual int hasBlastInfo()
    {
      return (1);
    }
  virtual float getBlastMapSize()
    {
      return (blast_map_size);
    }
  virtual float getBlastDur()
    {
      return (blast_dur);
    }
  virtual char *getBlastMap()
    {
      return (blast_map);
    }
  virtual char *getSmokeMap()
    {
      return (smoke_map);
    }

  void read(std::istream &is);
  void write(std::ostream &os);
  friend std::istream &operator >>(std::istream &is, Cannon_Specs &cs)
    {
      cs.read(is);
      return(is);
    }
  friend std::ostream &operator <<(std::ostream &os, Cannon_Specs &cs)
    {
      cs.write(os);
      return(os);
    }
};

class Bomb_Specs : public Weapon_Specs
{
public:
  char shape_file[13];
  char blast_map[32];
  char smoke_map[32];
  Z_Node_Manager *zm;
  float blast_radius;
  float blast_map_size;
  float blast_dur;
  Bomb_Specs()
    {
      wep_type = bomb_t;
      blast_radius = 1.0;
      blast_map_size = 1.0;
      blast_dur = 1.0;
      zm = NULL;
      strcpy(blast_map,"fire");
      strcpy(smoke_map,"smoke");
    }
  
  virtual ~Bomb_Specs();
  void createZManager(char *path);
  void read(std::istream &is);
  void write(std::ostream &os);
  friend std::istream &operator >>(std::istream &is, Bomb_Specs &ws)
    {
      ws.read(is);
      return(is);
    }
  friend std::ostream &operator <<(std::ostream &os, Bomb_Specs &ws)
    {
      ws.write(os);
      return(os);
    }
  Z_Node_Manager *getZNodeMgr()
    {
      return (zm);
    }
  virtual float getDamageRadius()
    {
      return(blast_radius);
    }
  virtual int hasBlastInfo()
    {
      return (1);
    }
  virtual float getBlastMapSize()
    {
      return (blast_map_size);
    }
  virtual float getBlastDur()
    {
      return (blast_dur);
    }
  virtual char *getBlastMap()
    {
      return (blast_map);
    }
  virtual char *getSmokeMap()
    {
      return (smoke_map);
    }
};

class Rocket_Specs : public Bomb_Specs
{
public:
  R_3DPoint flume_origin;
  float     flume_initl_size;
  float     flume_delta_size;
  char      flume_flame_map[32];
  char      flume_smoke_map[32];

  Rocket_Specs()
    :Bomb_Specs()
    {
      wep_type = rocket_t;
      flume_initl_size = 10.0;
      flume_delta_size = 0.0;
      strcpy(flume_flame_map,"fire");
      strcpy(flume_smoke_map,"smoke");
    }
  void read(std::istream &is);
  void write(std::ostream &os);
  friend std::istream &operator >>(std::istream &is, Rocket_Specs &rs)
    {
      rs.read(is);
      return(is);
    }
  friend std::ostream &operator <<(std::ostream &os, Rocket_Specs &rs)
    {
      rs.write(os);
      return(os);
    }
};

class Missile_Specs : public Bomb_Specs
{
public:
  Missile_Specs()
    :Bomb_Specs()
    {
      wep_type = missile_t;
    }
};

class FuelTank_Specs : public Bomb_Specs
{
public:
  FuelTank_Specs()
    :Bomb_Specs()
    {
      wep_type = fueltank_t;
    }
};

#define LP_PORT   0x01
#define LP_TRACER 0x02

class Launch_Params
{
public:
  Weapon_Specs *specs;
  R_3DPoint target_position;
  R_3DPoint departure_point;
  Vector departure_vector;
  Target *launcher;
  Flight *flight;
  Port_3D port;
  Vector_Q launcher_velocity;
  int flags;
  int fuse_type;
  float fuse_data;
  float hitScaler;
  Launch_Params()
    {
      fuse_type = f_impact;
      fuse_data = 0.0;
      launcher = NULL;
      flight = NULL;
      flags = 0;
		hitScaler = 1.0;
    }
} ;

class Weapon_Instance;
#define WP_SETVIEW 0x01

class Weapon
{
public:
  Weapon_Specs    *w_specs;
  Weapon_Instance *wep_i;
  int             n;
  R_3DPoint       *positions;
  Vector          *vectors;
  R_3DPoint       aim_position;
  Vector          aim_vector;
  Flight          *host_flight;
  int             idx;
  R_3DPoint       impact_point;
  char            id[32];

  Weapon();
  virtual ~Weapon();
  Weapon(Weapon_Specs *ws);

  virtual int update(Flight &host_flight, Unguided_Manager *um, 
		     Weapon_Instance *wi,
		     Target *launcher) = 0;
  virtual R_3DPoint predict_path(Flight & host_flight, float flt_time);
  virtual REAL_TYPE calc_htime(Flight &host_flight, R_3DPoint &p);
  virtual REAL_TYPE calc_hvtime(Flight &host_flight, R_3DPoint &);
  virtual void get_launch_params(Launch_Params &lp);
  virtual int getMaxRounds()
    {
      return (n);
    }
  void read(std::istream &is);
  void write(std::ostream &os);
  friend std::istream &operator >>(std::istream &is, Weapon &wp)
    {
      wp.read(is);
      return(is);
    }
  friend std::ostream &operator <<(std::ostream &os, Weapon &wp)
    {
      wp.write(os);
      return(os);
    }
};

class Guns : public Weapon
{
public:

  Gun_Specs *gun_specs;
  Guns(Gun_Specs *spcs)
    : Weapon(spcs)
    {
      gun_specs = spcs;
    }

  friend std::istream &operator >>(std::istream &is, Guns &gp);
  int update(Flight &host_flight, Unguided_Manager *um, 
	     Weapon_Instance *, Target *);
  int getMaxRounds();
};

class Bombs : public Weapon
{
public:
  Bomb_Specs *bomb_specs;
  Bombs(Bomb_Specs *spcs)
    : Weapon(spcs)
    {
      bomb_specs = spcs;
    }
  friend std::istream &operator >>(std::istream &is, Bombs &b);
  int update(Flight &host_flight, Unguided_Manager *um, 
	     Weapon_Instance *, Target *);
};

class Rockets : public Weapon
{
public:
  Rocket_Specs *rocket_specs;
  Rockets(Rocket_Specs *spcs)
    : Weapon(spcs)
    {
      rocket_specs = spcs;
    }
  friend std::istream &operator >>(std::istream &is, Rockets &r);
  int update(Flight &host_flight, Unguided_Manager *um, 
	     Weapon_Instance *, Target *);
};

class Missiles : public Weapon
{
public:
  Missile_Specs *missile_specs;
  Missiles(Missile_Specs *spcs)
    : Weapon(spcs)
    {
      missile_specs = spcs;
    }
  friend std::istream &operator >>(std::istream &is, Missiles &m);
  int update(Flight &host_flight, Unguided_Manager *um, 
	     Weapon_Instance *, Target *);
};

class FuelTanks : public Weapon
{
public:
  FuelTank_Specs *fueltank_specs;
  FuelTanks(FuelTank_Specs *spcs)
    : Weapon(spcs)
    {
      fueltank_specs = spcs;
    }
  friend std::istream &operator >>(std::istream &is, FuelTanks &m);
  int update(Flight &host_flight, Unguided_Manager *um, 
	     Weapon_Instance *, Target *);
};

class Weapon_Instance
{
public:
  REAL_TYPE elapsed_time;
  int rounds_remaining;
  int launch_idx;
  int rounds_per_launch;
  int flags;
  int n;
  float hitScaler;

  R_3DPoint target_position;
  Weapon    *weapon;
  Z_Viewer  *viewers;
  Port_3D   *ports;

  void set_weapon(Weapon *);

  Weapon_Instance()
    {
      weapon = NULL;
      viewers = NULL;
      ports = NULL;
      elapsed_time = 0.0;
      rounds_remaining = 0;
      rounds_per_launch = 1;
      flags = 0;
      launch_idx = 0;
		hitScaler = 1.0;
    }

  Weapon_Instance(Weapon *wp)
    {
      elapsed_time = 0.0;
      set_weapon(wp);
      rounds_per_launch = 1;
      launch_idx = 0;
		hitScaler = 1.0;
    }

  void copy(const Weapon_Instance &wi)
    {
      flags = wi.flags;
      rounds_per_launch = wi.rounds_per_launch;
      rounds_remaining = weapon->getMaxRounds();
		hitScaler = wi.hitScaler;
      set_weapon(wi.weapon);
    }

  Weapon_Instance(const Weapon_Instance &wi)
    {
      elapsed_time = 0.0;
      copy(wi);
    }

  ~Weapon_Instance();
    
  Weapon_Instance &operator =(const Weapon_Instance & wi)
    {
      copy(wi);
      return(*this);
    }

  int update(Flight &host_flight, Unguided_Manager *um, Target *launcher);

  R_3DPoint predict_path(Flight & host_flight, float flt_time)
    {
      return (weapon->predict_path(host_flight,flt_time));
    }
  REAL_TYPE calc_htime(Flight &host_flight, R_3DPoint &p)
    {
      return (weapon->calc_htime(host_flight,p));
    }
  REAL_TYPE calc_hvtime(Flight &host_flight, R_3DPoint &p)
    {
      return (weapon->calc_hvtime(host_flight,p));
    }
  void get_launch_params(Launch_Params &lp)
    {
      weapon->get_launch_params(lp);
    }
  
  int hasExterns()
    {
      if (weapon)
	return (weapon->w_specs->wep_type > gun_t);
      else
	return (0);
    }
  int getType()
    {
      if (weapon)
	return (weapon->w_specs->wep_type);
      else
	return (-1);
    }
  REAL_TYPE draw_prep(Port_3D *, Port_3D *);
  void draw(Port_3D *);
  int isVisible();
};

class Weapon_List
{
public:
  int      n;
  int      idx;
  int      del_flag;
  char     id[32];
  Weapon   **weapons;

  Weapon_List()
    {
      n = 0;
      idx = 0;
      del_flag = 0;
      weapons = NULL;
    }
  Weapon_List(int, int = 0);
  ~Weapon_List();
  int add_weapon(Weapon *);
  Weapon *get_weapon(int);
  int create_list(int);
};

class Weapon_Instance_List
{
public:
  int             n_weaps;
  int             sel_wpn;
  Weapon_Instance *weapons;
  int             has_externs;

  Weapon_Instance_List()
    :weapons(NULL),
     has_externs(0)
    {}

  Weapon_Instance_List(Weapon_Instance *wi, int n);

  ~Weapon_Instance_List()
    {
      /*
      if (weapons)
	delete [] weapons;
	*/
    }

  Weapon_Instance_List(const Weapon_Instance_List &wl)
    {
      copy(wl);
    }

  Weapon_Instance_List &operator =(const Weapon_Instance_List &wl)
    {
      copy(wl);
      return(*this);
    }

  void copy(const Weapon_Instance_List &wpl);
  int hasExterns()
    {
      return (has_externs);
    }
  void draw_prep(Port_3D *port, Port_3D *);
  int isVisible();
  void draw(Port_3D *port);
};

class Weapons_Manager
{
public:
  Weapon_Specs         **wep_specs;
  int                  nspecs;
  Weapon_List          *master_list;
  int                  nweps;
  Weapon_List          *lists;
  int                  nlists;
  Weapons_Manager();
  ~Weapons_Manager();
  void read_file(char *);
  friend std::istream & operator >>(std::istream &is, Weapons_Manager &wp);
  void read(std::istream &);
  Weapon_List *get_list(int n);
  Weapon_List *get_list(char *);
  Weapon_Instance *build_instance_list(int n, int *cnt, char *id = NULL);
  Weapon_Instance_List *build_instance_list(int n);
  Weapon_Specs *getSpecs(char *);
  Weapon *getWeapon(char *);

  static Weapons_Manager *the_weapons_manager;
};



#define WPSPECS(a) ((a)->weapon->w_specs)
#define FLSPECS(a) (WSPECS(a))->getFltSpecs())

#endif


