/*
 *  tparserprivate.h
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


#ifndef TPARSERPRIVATE_H
#define TPARSERPRIVATE_H

#include "tparser.h"

/* void TokenBegin(char *t); */

int panoScriptDataReset(void);
int panoScriptParserInit(char* filename);
void panoScriptParserClose(void);

int  panoScriptScannerGetNextChar(char* b, int maxBuffer);
void panoScriptScannerTokenBegin(char* t);
void panoScriptParserError(char const* errorstring, ...);
void yyerror(char const* st);
void* panoScriptReAlloc(void** ptr, int size, int* count);


#endif
