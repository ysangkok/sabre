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

#include <stdio.h>
#include <ctype.h>
#include "pc_keys.h"
#include "kbdhit.h"
#include "vga_13.h"


/*
 Note: 12/14/97
 Use of vga_getkey for Version 0.2.2
 Replace with getchar() if strange
 or unexpected results are occurring
 */

/* 12/98 David Vandewalle: SDL port */

#ifdef HAVE_LIBSDL
#include <SDL2/SDL.h>
#else
#include <directfb.h>
#endif

// #define TESTME


#ifdef HAVE_LIBSDL
// Q&D hack, sorry
static const unsigned char sdl_to_standard[256] =
{   0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15,
   16,'i','m','l','j', 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31,
   32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47,
   48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63,
   64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79,
   80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95,
   96, 97, 98, 99,100,101,102,103,104,105,106,107,108,109,110,111,
  112,113,114,115,116,117,118,119,120,121,122,123,124,125,126,127,
  128,129,130,131,132,133,134,135,136,137,138,139,140,141,142,143,
  144,145,146,147,148,149,150,151,152,153,154,155,156,157,158,159,
  160,161,162,163,164,165,166,167,168,169,170,171,172,173,174,175,
  176,177,178,179,180,181,182,183,184,185,186,187,188,189,190,191,
  192,193,194,195,196,197,198,199,200,201,202,203,204,205,206,207,
  208,209,210,211,212,213,214,215,216,217,218,219,220,221,222,223,
  224,225,226,227,228,229,230,231,232,233,234,235,236,237,238,239,
  240,241,242,243,244,245,246,247,248,249,250,251,252,253,254,255
};
#endif


KBHit kbhit;
int   KBHit::kbdin = 0;

#ifdef HAVE_LIBVGA
static IDirectFBEventBuffer *buffer = NULL;
static IDirectFBInputDevice *keyboard = NULL;
IDirectFB* dfb = NULL;
#endif

KBHit::KBHit()
{

  tcgetattr(0,&oldios);
  newios = oldios;

  newios.c_lflag &= static_cast<unsigned>(~(ICANON | ECHO));
  newios.c_iflag &= static_cast<unsigned>(~(ISTRIP | IGNCR | ICRNL | INLCR | IXOFF | IXON));
  newios.c_cc[VMIN] = 0;    /* Making these 0 seems to have the */
  newios.c_cc[VTIME] = 0;    /* desired effect. */

  tcsetattr(0,TCSANOW,&newios);
  FD_ZERO(&rfds);
  FD_SET(0,&rfds);

}

int KBHit::kbdhit()
{
  struct timeval tv;
  tv.tv_sec = 0;
  tv.tv_usec = 0;
  FD_ZERO(&rfds);
  FD_SET(0,&rfds);
  return(select(1,&rfds,NULL,NULL,&tv));
}

#ifdef HAVE_LIBVGA
static int mouse_x, mouse_y, mouse_y_min, mouse_y_max, mouse_x_min, mouse_x_max, raxis;
extern int adjusted_mouse_x;
extern int adjusted_mouse_y;
extern int adjusted_buttons;
int adjusted_mouse_x;
int adjusted_mouse_y;
int adjusted_buttons;

void show_mouse_event(DFBInputEvent*);
void show_mouse_event(DFBInputEvent* evt) {
  if (evt->type == DIET_AXISMOTION) {
       if (evt->flags & DIEF_AXISABS) {
            switch (evt->axis) {
            case DIAI_X:
                 mouse_x = evt->axisabs;
                 mouse_x_min = evt->min;
                 mouse_x_max = evt->max;
            break;
            case DIAI_Y:
                 mouse_y = evt->axisabs;
                 mouse_y_min = evt->min;
                 mouse_y_max = evt->max;
                 break;
            case DIAI_Z:
                 printf("Z axis (abs): %d\n", evt->axisabs);
                 break;
            case DIAI_LAST:
                 printf("Axis %d (abs): %d\n", evt->axis, evt->axisabs);
                 break;
            }
            raxis = 0;
       }
       else if (evt->flags & DIEF_AXISREL) {
            switch (evt->axis) {
            case DIAI_X:
                 mouse_x += evt->axisrel;
                 break;
            case DIAI_Y:
                 mouse_y += evt->axisrel;
                 break;
            case DIAI_Z:
                 printf("Z axis (rel): %d\n", evt->axisrel);
                 break;
            case DIAI_LAST:
                 printf("Axis %d (rel): %d\n", evt->axis, evt->axisrel);
                 break;
            }
            raxis = 1;
       }

       /* Touchpad axis range may not be the same as screen size */
       if ((mouse_y_min < mouse_y_max) && (mouse_x_min < mouse_x_max)) {
            adjusted_mouse_x = CLAMP (mouse_x, 0, mouse_x_max);
            adjusted_mouse_y = CLAMP (mouse_y, 0, mouse_y_max);
            adjusted_mouse_x = ((SCREEN_WIDTH  - 1) * adjusted_mouse_x) / mouse_x_max;
            adjusted_mouse_y = ((SCREEN_HEIGHT - 1) * adjusted_mouse_y) / mouse_y_max;
       } else {
            adjusted_mouse_x = CLAMP (mouse_x, 0, SCREEN_WIDTH  - 1);
            adjusted_mouse_y = CLAMP (mouse_y, 0, SCREEN_HEIGHT - 1);
       }
  } else if (evt->type == DIET_BUTTONPRESS) {
       printf ("Button press %d\n", evt->button);
       adjusted_buttons ^= 1 << evt->button;
  } else if (evt->type == DIET_BUTTONRELEASE) {
       printf ("Button release %d\n", evt->button);
       adjusted_buttons ^= 1 << evt->button;
  }
}

void show_any_button_event( DFBInputEvent *);
void show_any_button_event( DFBInputEvent *evt ) {
  printf ("Button %d %s\n", evt->button,
            (evt->type == DIET_BUTTONPRESS) ? "pressed" : "released");
}

void show_any_axis_event( DFBInputEvent *);
void show_any_axis_event( DFBInputEvent *evt ) {
  if (evt->flags & DIEF_AXISABS)
       printf (
                 "Axis %d (abs): %d\n", evt->axis, evt->axisabs);
  else
       printf (
                 "Axis %d (rel): %d\n", evt->axis, evt->axisrel);
}

typedef struct _DeviceInfo DeviceInfo;

struct _DeviceInfo {
     DFBInputDeviceID           device_id;
     DFBInputDeviceDescription  desc;
     DeviceInfo                *next;
};

DFBInputDeviceTypeFlags get_device_type(DeviceInfo *devices, DFBInputDeviceID device_id);
DFBInputDeviceTypeFlags get_device_type(DeviceInfo *devices, DFBInputDeviceID device_id)
{
     while (devices) {
          if (devices->device_id == device_id)
               return devices->desc.type;
          devices = devices->next;
     }

     return DIDTF_NONE;
}

DFBEnumerationResult enum_input_device( DFBInputDeviceID, DFBInputDeviceDescription, void *);
DFBEnumerationResult
enum_input_device( DFBInputDeviceID           device_id,
                   DFBInputDeviceDescription  desc,
                   void                      *data )
{
     DeviceInfo **devices = (DeviceInfo**) data;
     DeviceInfo  *device;

     device = (DeviceInfo*) malloc( sizeof(DeviceInfo) );

     device->device_id = device_id;
     device->desc      = desc;
     device->next      = *devices;

     *devices = device;

     return DFENUM_OK;
}
#endif

int KBHit::getch() {
#ifdef HAVE_LIBSDL
  SDL_Event event;
#else
  if (!buffer) {
    dfb->GetInputDevice (dfb, DIDID_KEYBOARD, &keyboard);
    keyboard->CreateEventBuffer (keyboard, &buffer);
  }
  DFBInputEvent event;
#endif


#ifdef HAVE_LIBSDL
  if (SDL_PollEvent(&event)) {
    unsigned char c;
    c = static_cast<unsigned char>(event.key.keysym.sym);
    c = sdl_to_standard[c];
    if (c==ESC) {
      c = static_cast<unsigned char>(event.key.keysym.sym);
      if (c=='[') {
        c = static_cast<unsigned char>(event.key.keysym.sym);
      } else {
        putchar(c);
        c = ESC;
      }
    }
    tcflush(0,TCIFLUSH);
    kbdin = static_cast<int>(c);
    return kbdin;
  }
  kbdin = 0;
  return (0);
#else
  while (buffer->GetEvent (buffer, DFB_EVENT(&event)) == DFB_OK) {
    if (event.type == DIET_AXISMOTION || event.type == DIET_BUTTONPRESS || event.type == DIET_BUTTONRELEASE) {
      DeviceInfo *devices = NULL;
      dfb->EnumInputDevices( dfb, enum_input_device, &devices );
      DFBInputDeviceTypeFlags device_type = get_device_type( devices, event.device_id);
      if (device_type & DIDTF_MOUSE )
        show_mouse_event(&event);
      else if (event.type == DIET_BUTTONPRESS || event.type == DIET_BUTTONRELEASE)
        show_any_button_event(&event);
      else
        show_any_axis_event(&event);
    } else if (event.type == DIET_KEYPRESS) {
      unsigned char c;
      c = (unsigned char) event.key_symbol;
/*
      if (c==ESC) {
        if (c=='[') {
        } else {
            putchar(c);
            c = ESC;
        }
      }
      tcflush(0,TCIFLUSH);
*/
      kbdin = (int) c;
      return ((int)c);
    }
  }
  kbdin = 0;
  return 0;
#endif
}

KBHit::~KBHit()
{
  tcsetattr(0,TCSAFLUSH,&oldios);
}
