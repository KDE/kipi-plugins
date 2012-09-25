/*
 *  tparserdebug.h
 *
 *  Copyright  Daniel M. German
 *  
 *  April 2007
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 *  This software is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public
 *  License along with this software; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 *
 *  Author: Daniel M German dmgerman at uvic doooot ca
 *
 */

#ifndef TPARSERDEBUG_H
#define TPARSERDEBUG_H

#ifdef YYDEBUG
#define DEBUG_1(a) fprintf(stderr, #a "\n");
#define DEBUG_2(a,b) fprintf(stderr, #a "\n", b);
#define DEBUG_3(a,b,c) fprintf(stderr, #a "\n", b, c);
#define DEBUG_4(a,b,c,d) fprintf(stderr, #a "\n", b, c, d);
#else
#define DEBUG_1(a)
#define DEBUG_2(a,b)
#define DEBUG_3(a,b,c) 
#define DEBUG_4(a,b,c,d)
#endif

#endif
