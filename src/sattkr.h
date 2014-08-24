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
 * File   : smath.h                              *
 * Date   : April, 1998                          *
 * Author : Dan Hammer                           *
 * Attacker list                                 *
 *************************************************/
#ifndef __iattkr_h
#define __iattkr_h

#ifdef __cplusplus

#include "smath.h"
#include "dhlist.h"

/*
 * Attacker record
 */
class sAttacker
{
protected:
	uint32_t idx;
	sREAL	threatValue;
public:

	sAttacker()
	{
		idx = 0L;
		threatValue = 0.0;
	}

	sAttacker(uint32_t ix)
	{
		this->idx = ix;
		threatValue = 0.0;
	}
       
	void Copy(const sAttacker &attkr)
	{
		idx = attkr.idx;
		threatValue = attkr.threatValue;
	}

	sAttacker(const sAttacker &attkr)
	{
		Copy(attkr);
	}

	sAttacker & operator =(const sAttacker &attkr)
	{
		Copy(attkr);
		return (*this);
	}

	int IsEqual(const sAttacker &attkr)
	{
		return (idx == attkr.idx);
	}

    uint32_t GetIdx()
    {
      return (idx);
    }

    void SetThreatValue(sREAL threatVal)
    {
        this->threatValue = threatVal;
    }

    sREAL GetThreatValue()
    {
      return (threatValue);
    }

    friend class sAttackerList;
};

/*
 * Linked list of attackers
 */
class sAttackerList
{
protected:
	Dhlist attacker_list;		/* "c" linked list container */
	int	   count;
public:
	sAttackerList()
	{
		new_dhlist(&attacker_list);
		count = 0;
	}

	~sAttackerList()
	{
		dhlist_free(&attacker_list,1);
	}

	void AddAttacker(uint32_t idx);
	void DeleteAttacker(uint32_t idx);
	void DeleteAllAttackers();
	int GetCount()
	{
	   return (count);
	}

	sAttacker *GetFirstAttacker();
	sAttacker *GetNextAttacker();
	sAttacker *GetAttackerByIdx(uint32_t idx);

	static void DeleteSAttacker(void *);
	static int CompareSAttacker(void *, void *);
	static int CompareSAttackerThreat(void *, void *);


};

#endif /* __cplusplus */

#endif
