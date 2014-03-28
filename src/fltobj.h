/*
    Sabre Fighter Plane Simulator 
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
 *           Sabre Fighter Plane Simulator       *
 * File   : fltobj.h                             *
 * Date   : February, 1998                       *
 * Author : Dan Hammer                           *
 * Some support classes for the flight modeling  *
 * Originally in flight.h                        *
 *************************************************/
#ifndef __fltobj_h
#define __fltobj_h

#include "dvmath.h"
#include "spid.h"

class C_3DObject_Base;
extern float time_frame;
extern const float nz;
extern const float wb_damp;
extern float p_height;
extern const float g;
extern float time_factor;
extern float world_scale;

/* engine types */
#define PROP_TYPE       0
#define JET_TYPE        1
#define JET_AB_TYPE     2
#define TURBO_JET_TYPE  3
/* radar type mask */
#define RADAR_TYPE_MASK 0x07L
/* Default minimum & maximum values for controls */
#define AILERON_MIN    -30     
#define AILERON_MAX     30
#define ELEVATOR_MIN   -30
#define ELEVATOR_MAX    30
#define RUDDER_MIN     -30
#define RUDDER_MAX      30
#define FLAPS_MIN        0
#define FLAPS_MAX       20
#define FLAPS_STEP       5
#define TRIM_MIN       -30
#define TRIM_MAX        30

/* enums for view */
enum flight_view {
				fv_front, fv_left, fv_right, fv_rear, fv_satellite,
			   fv_external, fv_track, fv_target_track, fv_flyby,
				fv_front_up, fv_front_down, fv_left_up, fv_left_down, fv_right_up,
				fv_right_down, fv_rear_up, fv_rear_down, fv_up, fv_down,
				fv_weapon_track, fv_virtual, fv_front_left, fv_front_left_up,
				fv_front_left_down, fv_front_right, fv_front_right_up, fv_front_right_down,
				fv_rear_left, fv_rear_left_up, fv_rear_left_down, fv_rear_right,
				fv_rear_right_up, fv_rear_right_down, fv_padlock
};

/******************************************************************
 * specifications class                                           *
 * Units are fps (feet-pounds-seconds), angles in radians         *
 ******************************************************************/
class Flight_Specs
{
public:
  float weight;                        /* takeoff weight in pounds */
  float max_thrust;                    /* maximum thrust in lbs/ft */
  float max_speed;                     /* maximum speed in kts */
  float corner_speed;                  /* "corner" speed */
  float drag_factor;                   /* drag coefficient */
  float idrag_factor;                  /* induced drag coefficient */
  float lift_factor;                   /* lift coefficient */
  float wing_aoa;                      /* angle-of-attack of wing */
  float max_aoa;                       /* maximum angle-of-attack before stall */
  float max_aoa_factor;                /* max_aoa before stall relative to velocity */
  /*****************************
   * Additional sources of drag
   *****************************/
  float drag_aoa;                      /* additional drag from angle-of-attack */
  float drag_yaw;                      /* additional drag from yaw */
  float drag_wb;                       /* drag produced by engaged wheel-brakes */
  float drag_gr;                       /* drag produced by lowered landing gear */
  float drag_sb;                       /* drag produced by speed-brakes */
  float drag_whls;                     /* drag from wheels on ground */
  /*****************************
   * Rotational parameters     
   *****************************/
  float control_pitch;	               /* pitching moment produced by elevator position */
  float control_yaw;                   /* yawing moment produced by rudder position */
  float control_roll;	               /* rolling moment produced by aileron positions */
  float return_pitch;	               /* pitching moment towards flight path produced by horiz. stab. */
  float return_yaw;		               /* yawing moment towards flight path produced by vert. stab. */ 
  float pitch_drag;                    /* drag factor for pitching */
  float yaw_drag;                      /* drag factor for yawing */
  float roll_drag;                     /* drag factor for rolling */
  float pitch_damp;                    /* damp on pitching */
  float yaw_damp;                      /* damp on yawing */
  float roll_damp;                     /* damp on rolling */
  /*****************************
   * landing parameters        *
   *****************************/
  float lspeed;  		       /* maximum landing speed */
  float l_z;     		       /* minimum vertical velocity */
  float l_aoa_max;                     /* angle-of-attack */
  float max_wb;			       /* speed at which wheel-brakes become effective */
  /****************************
   * adverse yaw and roll     *
   ****************************/
  float adv_yaw;		       /* yawing moment produced by ailerons */
  float adv_roll;		       /* rolling moment produced by rudder */
  /****************************
   * more aeorodynamically    *
   * suspicious stuff         *
   ****************************/
  float g_height;           /* height of fuselage when on ground w gear down */
  float flap_lift;   		 /* how much flaps contribute to lift */
  float flap_drag;		    /* how much flaps contribute to drag */
  float fuel_cap;		       /* capacity in lbs of internal fuel tank */
  float wb_damp;		       /* damping factor for wheel brakes */
  float load_limit;		    /* limit on load (lift / weight) */
  float stall_spin;		    /* rolling moment from stall */
  int max_damage;		       /* maximum amount of abuse the poor plane can stand */
  R_3DPoint view_point;		 /* pilot's head position in cockpit ("port" coords") */
  int engine_type;		    /* 0 = prop, 1 = jet, 2 = jet with afterburner */
  long flags;			       /* general-purpose flags */
  char model[32];           /* model */

  /**************************
   * AND MORE! AND MORE!    *
	**************************/
  _sPID pitchPid;					/* pid for pitch control */
  _sPID rollPid;					/*         roll control  */
  _sPID yawPid;               /*         yaw control   */
  _sPID gPid;                 /*         g's control   */
  _sPID airSpeedPid;          /*         air speed     */
  _sPID targetClosurePid;     /*         target closure */

  /*************************
   * when will it end???   *
	*************************/
  float formationWingLen;	   /* wing length for formation spacing */
  float formationOffset;		/* distance to align point for formation */

  /************************************
   * Contributed by Pierre de Moveaux *
   * Dihedral effect parameters       *
   ************************************/
  float dihedralX;
  float dihedralY;
  float dihedralZ;

  Flight_Specs();

  void read(std::istream &);
  void write(std::ostream &);
  friend std::istream &operator >>(std::istream &is, Flight_Specs &fs)
    {
      fs.read(is);
      return(is);
    }
  friend std::ostream &operator <<(std::ostream &os, Flight_Specs &fs)
    {
      fs.write(os);
      return(os);
    }
  int read_file(char *);
  int write_file(char *);
  static Flight_Specs **g_specs;
  static int nspecs;
  static Flight_Specs *getSpecs(char *model);
};


/******************************************************************
 * controls class                                                 *
 ******************************************************************/
class Flight_Controls
{
public:
  /* Local min & max control values */
  float aileron_min,aileron_max;
  float elevator_min, elevator_max;
  float rudder_min, rudder_max;
  float flaps_min, flaps_max, flaps_step;
  float trim_min, trim_max;
public:
  float ailerons;                 /* aileron position */	
  float elevators;    	          /* elevator position */
  float throttle;    	          /* throttle percentage (0 ... 100%) */
  float rudder;                   /* rudder position */
  float flaps;                    /* degree of flaps */
  float trim;                     /* trim-tab position */
  
  /* Toggle (on/off) controls */
  int speed_brakes;       
  int wheel_brakes;
  int landing_gear;
  int engine_on;
  int bang_bang;                  /* fire weapon */
  int autopilot;
  int radar;
  int armed_w;                    /* arm weapon */
  int auto_coord;						 /* no adverse yaw, roll */
  int a_burner;                   /* after burner stage */
  int s_weapon;                   /* selected weapon index */

  /* display flags */
  int cockpit;                    /* display cockpit graphic if available */
  int vextern;                    /* display exterior */
  int hud_on;                     /* display hud */
  int vect_on;                    /* display flight-path vector */
  int show_controls;              /* display yoke position, rudder position etc */
  /* for new ai -- display maneuver stack & target flags */
  int show_ai_info;
  /* outside viewing params */
  R_3DPoint vPoint;					 /* view position in world coords */
  REAL_TYPE vdist;                /* distance from aircraft */                 
  REAL_TYPE vtheta;               /* horiz. rotation */
  REAL_TYPE vphi;                 /* vert. rotation */
  /* virtual cockpit params */
  REAL_TYPE virtTheta;				 /* horiz rot from front */
  REAL_TYPE virtPhi;					 /* vert rot from front */
  REAL_TYPE virtStep;				 /* rotation step */
  /* current view */
  int view;

  Flight_Controls()
  {
    aileron_min = AILERON_MIN;
    aileron_max = AILERON_MAX;
    elevator_min = ELEVATOR_MIN;
    elevator_max = ELEVATOR_MAX;
    rudder_min = RUDDER_MIN;
    rudder_max = RUDDER_MAX;
    flaps_min = FLAPS_MIN;
    flaps_max = FLAPS_MAX;
    flaps_step = FLAPS_STEP;
    trim_min = TRIM_MIN;
    trim_max = TRIM_MAX;

    elevators = 0;
    ailerons = 0;
    throttle = 0;
    rudder = 0;
    speed_brakes = 0;
    flaps = 0;
    trim = 0;
    wheel_brakes = 0;
    landing_gear = 0;
    view = fv_front;
    engine_on = 0;
    bang_bang = 0;
    autopilot = 0;
    s_weapon = 0;
    a_burner = 0;
    radar = 0;
    armed_w = 0;
    cockpit = 1;
    vextern = 1;
    hud_on = 1;
    vect_on = 0;
    vdist = 100;
    vtheta = 0.6;
    vphi = 0.2;
	 virtTheta = 0.0;
	 virtPhi = 0.0;
	 virtStep = 0.1;
    show_controls = 1;
	 show_ai_info = 0;
	 auto_coord = 0;
  }

  void read(std::istream &is);
  void write(std::ostream &os);
  friend std::istream & operator >>(std::istream &is, Flight_Controls &fc)
    {
      fc.read(is);
      return(is);
    }
  friend std::ostream & operator <<(std::ostream &os, Flight_Controls &fc)
    {
      fc.write(os);
      return(os);
    }
};

/***************************************************************
  A way of representing 'vector' quantities in two parts - a
  float scalar value, representing the magnitude, and
  normalized vector representing the direction. The magnitude
  is always positive.
  *************************************************************/
class Vector_Q
{
public:
  float magnitude;
  DVector direction;
  Vector_Q()
    : magnitude(0),
      direction(0,0,0)
  {}
  Vector_Q(float &mag, const DVector &v)
    : magnitude(mag),direction(v)
  {}
  Vector_Q(const Vector_Q &vq)
    : magnitude(vq.magnitude),
      direction(vq.direction)
  {}
  Vector_Q &operator =(const Vector_Q &vq)
  {
    magnitude = vq.magnitude; direction = vq.direction;
    return *this;
  }
  friend DVector to_vector(Vector_Q &vq);
  friend Vector_Q operator +(Vector_Q &, Vector_Q &);
  friend DVector operator +(DVector &, Vector_Q &);

  friend std::ostream &operator <<(std::ostream &os, Vector_Q &vq)
  {
    os << '(' << vq.magnitude << ' ' << vq.direction << ')';
    return os;
  }

  friend std::istream &operator >>(std::istream &is, Vector_Q &vq);
};

/***********************************************************
 * current forces in effect                                *
 ***********************************************************/
class Flight_Forces
{
public:
  /* "Vector" forces                                      */
  Vector_Q thrust; /* thrust of engine                    */
  Vector_Q drag;   /* opposing drag                       */
  Vector_Q lift;   /* lift                                */
  Vector_Q g;      /* weight                              */
  /* rotational accelerations                             */
  float pitch_acc; /* net pitching acceleration           */
  float yaw_acc;   /* net yawing acceleration             */
  float roll_acc;  /* net rolling acceleration            */
  Flight_Forces()
  {
    pitch_acc = yaw_acc = roll_acc = 0.0;
  }
};

/***********************************************************
 * current state of flight                                 *
 ***********************************************************/
class Flight_State
{
public:
  Vector_Q velocity;		 /* velocity in feet-per-second, in world coordinants */
  DVector vel_pv;        /* direction of velocity in port coordinants */
  float z_vel;           /* magnitude of velocity along z-axis ("air speed") */
  float delta_z_vel;     /* rate of change in above */
  float climb_vel;       /* climb rate in feet-per-second */
  float delta_climb_vel; /* rate of change of rate of climb (whew!) */
  float angle_of_attack; /* also known as "angle of incidence", the flight-path v fuselage */
  float yaw;             /* actually, the cos of the yaw (1 ... -1) */
  DVector h_direction;   /* Vector representing movement on xy plane */
  float heading;         /* the heading for above, in radians */
  Port_3D flight_port;   /* gives position and orientation */
  int inverted;          /* true if inverted */
  int negative_roll;     /* true if rolled into negative angle */
  int negative_phi;      /* true if pitched into negative angle */
  int crashed;           /* true if aerodynamically challenged */
  int stalled;           /* true if stalled */
  float near_stall;      /* how far from stall angle (radians) */
  float pitch_rate;      /* angular rate of pitch (radians per second) */
  float d_pitch_rate;    /* rate of change for above */
  float yaw_rate;        /* angular rate of yaw (radians per second) */
  float d_yaw_rate;      /* rate of change for above */
  float roll_rate;		 /* angular rate of roll (radians per second) */
  float d_roll_rate;     /* rate of change for above */
  int on_ground;         /* true if on ground */
  /* landing parameters */
  float landing_z;       /* vertical velocity upon landing */
  float landing_velocity;/* speed upon landing */
  float landing_aoa;		 /* aoa on landing */
  float landing_pitch;   /* pitch relative to ground on landing */
  float landing_roll;    /* roll on landing */
  int was_airborne;      /* flag indicating we were at one time airborne */
  int landing_recorded;  /* landing data recorded */
  /* more stuff */
  float load;            /* load (lift / weight); */
  float d_load;          /* change in */
  float air_density;		 /* air density factor */
  float vel_factor;		 /* velocity factor in determining roll rates */
  float ground_height;   /* height-of-terrain below us */
  float d_ground_height; /* change in */
  float agl;             /* Altitude above Ground Level */
  float d_agl;           /* change in */
  float weight;          /* current weight */
  float fuel;            /* current lbs of fuel */
  float elapsed_time;    /* elapsed time in seconds */
  Flight_State()
  {
    z_vel = 0.0;
    delta_z_vel = 0.0;
    climb_vel = 0.0;
    delta_climb_vel = 0.0;
    angle_of_attack = 0.0;
    yaw = 0.0;
    heading = 0.0;
    inverted = 0;
    negative_roll = 0;
    negative_phi = 0;
    crashed = 0;
    stalled = 0;
    near_stall = 0;
    pitch_rate = d_pitch_rate = 0.0;
    yaw_rate = d_yaw_rate = 0.0;
    roll_rate = d_roll_rate = 0.0;
    landing_z = 0.0;
    landing_velocity = 0.0;
    landing_aoa = 0.0;
    landing_pitch = 0.0;
    load = d_load = 0.0;
    was_airborne = 0;
    landing_recorded = 0;
    vel_factor = 0.0;
    ground_height = 0.0;
    d_ground_height = 0.0;
    agl = 0.0;
    d_agl = 0.0;
    weight = 0.0;
    fuel = 0.0;
    elapsed_time = 0.0;
  }
  void read(std::istream &is);
  void write(std::ostream &os)
    {
      os << velocity.magnitude << flight_port;
    }
  friend std::istream &operator >>(std::istream &is, Flight_State &fp)
    {
      fp.read(is);
      return(is);
    }
  friend std::ostream &operator <<(std::ostream &os, Flight_State &fp)
    {
      fp.write(os);
      return(os);
    }
};

// This class will be used to modify the performance of the flight
// due to variations such as altitude & air speed. It will also keep track of
// battle damage
class Flight_Mods
{
public:
  // Total amount of battle damage : 100 == kaput!
  int battle_damage;
  // The following vars modify the forces generated
  // by the various aircraft components
  float engine_e;
  float wing_e;
  float elevators_e;
  float ailerons_e;
  float rudder_e;
  float h_stab_e;
  float v_stab_e;
  float flaps_e;
  float speed_brakes_e;
  float wheel_brakes_e;
  // out-of-control spin factor
  float spin_out;
  float yaw_out;
  float pitch_out;
  // drag modifier
  float drag_e;
  // fuel loss modifier
  float fuel_e;
  Flight_Mods()
  {
    battle_damage = 0;
    engine_e = 1.0;
    wing_e = 1.0;
    elevators_e = 1.0;
    ailerons_e = 1.0;
    rudder_e = 1.0;
    h_stab_e= 1.0;
    v_stab_e= 1.0;
    flaps_e = 1.0;
    speed_brakes_e = 1.0;
    wheel_brakes_e = 1.0;
    drag_e = 1.0;
    spin_out = 0.0;
    yaw_out = 0.0;
    pitch_out = 0.0;
    fuel_e = 0.0;
  }
};

inline int IS_COCKPIT_VIEW(int viewType)
{
	return viewType <= fv_rear ||
			 (viewType >= fv_front_up && viewType <= fv_rear_down) ||
			 (viewType >= fv_virtual && viewType <= fv_padlock);
}
#endif
