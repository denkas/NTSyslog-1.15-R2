/*-----------------------------------------------------------------------------
 *
 *  engine.c - Event processing engine
 *
 *    Copyright (c) 1998, SaberNet.net - All rights reserved
 *
 *    This program is free software; you can redistribute it and/or
 *    modify it under the terms of the GNU General Public License
 *    as published by the Free Software Foundation; either version 2
 *    of the License, or (at your option) any later version.
 *    
 *    This program is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *    
 *    You should have received a copy of the GNU General Public License
 *    along with this program; if not, write to the Free Software
 *    Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307
 *
 *  $Id: engine.c,v 1.3 2007/10/25 18:20:59 frzem Exp $
 *
 *  Revision history:
 *    17-Aug-98  JRR  Module completed
 *
 *    21-Oct-2007 EM
 *	- Small Code-changes and fixes ...
 *
 *----------------------------------------------------------------------------*/
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "ntsl.h"
#include "engine.h"

/*------------------------------[ private data ]------------------------------*/
static ntsl_event *engine_last_event  = NULL;
static HANDLE engine_event_mutex = NULL;


/*-------------------------------[ engine_init ]-------------------------------
 * Create mutex object and open logfile
 *----------------------------------------------------------------------------*/
void engine_init(void)
{
    if( (engine_event_mutex = CreateMutex(NULL, FALSE, NULL)) == 0 )    
	ntsl_die(NTSL_ERROR_ENGINE_MUTEX, GetLastError());
}


/*-----------------------------[ engine_shutdown ]-----------------------------
 * Force execution of shutdown functions.
 *----------------------------------------------------------------------------*/
void engine_shutdown(void)
{
    HANDLE h = engine_event_mutex;

    engine_event_mutex = NULL ;
    if( h != NULL )
	CloseHandle( h );

    if( engine_last_event != NULL )
	LocalFree((HLOCAL)engine_last_event);
    engine_last_event = NULL;
}


/*--------------------------[ engine_process_event ]--------------------------
 * Top level event handler.  
 *
 *	Returns:
 *		success	0
 *		failure	-1 
 *
 * NOTE: This module is resposible for freeing event objects.
 *----------------------------------------------------------------------------*/
int engine_process_event(ntsl_event *pEvent, char *group)
{
    int rc = -1;

    if (NULL != pEvent)
    {
	WaitForSingleObject(engine_event_mutex, INFINITE);

	event_output(pEvent);

	if( engine_last_event != NULL )
	    LocalFree((HLOCAL)engine_last_event);
	engine_last_event = pEvent;

	ReleaseMutex(engine_event_mutex);
	rc = 1;
    }
    return(rc);
}
