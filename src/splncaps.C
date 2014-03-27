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
 * File   : splncaps.cpp                         *
 * Date   : April, 1998                          *
 * Author : Dan Hammer                           *
 *************************************************/
#include <stdio.h>
#include <string.h>

#include "splncaps.h"

sObjectArray sPlaneCaps::sPlaneCapsArray(sMAXPLANECAPS,1);

void sPlaneCaps::AddsPlaneCaps(sPlaneCaps &PlaneCaps)
{
	sPlaneCapsArray.Add(&PlaneCaps);
}

void sPlaneCaps::AddsPlaneCaps(splane_caps *caps)
{
    if (sPlaneCapsArray.Count() < sMAXPLANECAPS)
    {
       sPlaneCaps *PlaneCaps = new sPlaneCaps(*caps);
		 AddsPlaneCaps(*PlaneCaps);
    }
}

sPlaneCaps *sPlaneCaps::GetsPlaneCaps(const char *model)
{
    sPlaneCaps *result = NULL;
    for (int i=0;i<sPlaneCapsArray.Count();i++)
    {
      sPlaneCaps *PlaneCaps = (sPlaneCaps *)sPlaneCapsArray[i];
      if (!strcmp(PlaneCaps->model,model))
      {
        result = PlaneCaps;
        break;
      }
    }
   return (result);
} 

void sPlaneCaps::FlushsPlaneCaps()
{
	sPlaneCapsArray.Flush();
}

sPlaneCaps::sPlaneCaps(const splane_caps &caps)
{
	strcpy(model,caps.model);

}
