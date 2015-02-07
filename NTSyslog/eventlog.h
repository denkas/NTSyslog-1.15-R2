/*-----------------------------------------------------------------------------
 *
 *  eventlog.h - Windows NT eventlog module
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
 *  $Id: eventlog.h,v 1.3 2007/10/25 18:20:59 frzem Exp $
 *
 *----------------------------------------------------------------------------*/

#ifndef _EVENTLOG_H_
#define _EVENTLOG_H_

#ifndef _NTSL_H_
#include "ntsl.h"
#endif

#ifndef _EVENT_H_
#define	_EVENT_H_

/*-------------------------------[ static data ]------------------------------*/
#define NTSL_EVENT_ERROR	"Error"
#define NTSL_EVENT_WARNING	"Warning"
#define NTSL_EVENT_INFORMATION	"Info"
#define NTSL_EVENT_SUCCESS	"Success"
#define NTSL_EVENT_FAILURE	"Failure"
#define NTSL_EVENT_FORMAT_LEN	NTSL_EVENT_LEN
#define NTSL_DEFAULT_PRIORITY	9

/*-------------------------------[ ntsl_event ]-------------------------------*/
typedef struct
{
	int	priority;
	char	date[NTSL_DATE_LEN];
        char	host[NTSL_SYS_LEN];
        const char* eType ;
        char	szSource[NTSL_SYS_LEN];
	uint32	dwCode;
        char	szUser[NTSL_SYS_LEN];
        char	msg[NTSL_EVENT_LEN];
} ntsl_event;

int	event_output(ntsl_event *Event);
#endif	/* _EVENT_H_ */

int	eventlog_parse_libs(char*, char*, uint32);
int	eventlog_init(void);
void	eventlog_shutdown(void);
void	eventlog_check_events(void);
#endif	/* _EVENTLOG_H_	*/
