/*-----------------------------------------------------------------------------
 *
 *  event.h - Event type definition
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
 *  $Id: event.h,v 1.1.1.1 2001/11/28 05:25:03 sabernet Exp $
 *
 *----------------------------------------------------------------------------*/
#ifndef _EVENT_H_
#define _EVENT_H_

#include "ntsl.h"

/*-------------------------------[ static data ]------------------------------*/
#define NTSL_EVENT_ERROR		 "[error]"
#define NTSL_EVENT_ERROR		 "[error]"
#define NTSL_EVENT_WARNING  	 "[warning]"
#define NTSL_EVENT_INFORMATION   "[info]"
#define NTSL_EVENT_SUCCESS       "[success]"
#define NTSL_EVENT_FAILURE       "[failure]"
#define NTSL_EVENT_FORMAT_LEN    NTSL_EVENT_LEN

/*-------------------------------[ ntsl_event ]-------------------------------*/
typedef struct 
{
    char    date[NTSL_DATE_LEN];
    char    host[NTSL_SYS_LEN];
	char	source[NTSL_SYS_LEN];
	char	etype[NTSL_SYS_LEN];	
    char    msg[NTSL_EVENT_LEN];
} ntsl_event;


int event_output(ntsl_event *event);

#endif
