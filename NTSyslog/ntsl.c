/*-----------------------------------------------------------------------------
 *
 *  ntsl.c - NTSysLog main
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
 *  $Id: ntsl.c,v 1.3 2007/10/25 18:20:59 frzem Exp $
 *
 *	Options:
 *	  _DEBUG     -	Turns on memory leak detection
 *    NTSL_STUB  -  Minimal functions for module testing
 *
 *  Revision history:
 *    17-Aug-98  JRR  Module completed
 *
 *    21-Oct-2007 EM
 *	- Code-changes, because most Logging-functions are in service.c
 *
 *----------------------------------------------------------------------------*/
#include <windows.h>
#include <stdio.h>
#include <stdarg.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <direct.h>
#include <process.h>
#include <time.h>
#include "ntsl.h"
#include "eventlog.h"
#include "engine.h"

#ifdef	_DEBUG
#include <crtdbg.h>
#endif

/*-------------------------------[ static data ]------------------------------*/
#define	MAX_ERROR_LEN		512


/*-----------------------------[ ntsl_log_error ]-----------------------------
 * Report error to the event log
 *----------------------------------------------------------------------------*/
void ntsl_log_error(char *format, ...)
{
    va_list args;
    char s[MAX_ERROR_LEN];

    va_start(args, format);
    _vsnprintf(s, MAX_ERROR_LEN, format, args);
    va_end(args);

    s[MAX_ERROR_LEN-1] = '\0';
#ifdef	_DEBUG
    fprintf(stderr, "%s\n", s);
#endif
    _LogEventEx2(EVENTLOG_ERROR_TYPE, 20uL, 1u, s, NULL);
}

/*------------------------------[ ntsl_log_info ]-----------------------------
 * Write information message to the event log
 *----------------------------------------------------------------------------*/
void ntsl_log_info(char *format, ...)
{
    va_list args;
    char s[MAX_ERROR_LEN];

    va_start(args, format);
    _vsnprintf(s, MAX_ERROR_LEN, format, args);
    va_end(args);

    s[MAX_ERROR_LEN-1] = '\0';
#ifdef	_DEBUG
    fprintf(stderr, "%s\n", s);
#endif
    _LogEventEx2(EVENTLOG_INFORMATION_TYPE, 24uL, 1u, s, NULL);
}

/*----------------------------[ ntsl_log_warning ]----------------------------
 * Write information message to the event log
 *----------------------------------------------------------------------------*/
void ntsl_log_warning(char *format, ...)
{
    va_list args;
    char s[MAX_ERROR_LEN];

    va_start(args, format);
    _vsnprintf(s, MAX_ERROR_LEN, format, args);
    va_end(args);

    s[MAX_ERROR_LEN-1] = '\0';
#ifdef	_DEBUG
    fprintf(stderr, "%s\n", s);
#endif
    _LogEventEx2(EVENTLOG_WARNING_TYPE, 21uL, 1u, s, NULL);
}

#if 0
/*------------------------------[ ntsl_log_msg ]-----------------------------
 * Write message to the event log
 *----------------------------------------------------------------------------*/
void ntsl_log_msg(uint16 etype, char *format, ...)
{
    va_list args;
    char s[MAX_ERROR_LEN];

    va_start(args, format);
    _vsnprintf(s, MAX_ERROR_LEN, format, args);
    va_end(args);

    s[MAX_ERROR_LEN-1] = '\0';
#ifdef	_DEBUG
    fprintf(stderr, "%s\n", s);
#endif
    _LogEventEx2(etype, 0uL, 1u, s, NULL);
}
#endif

/*--------------------------------[ ntsl_die ]--------------------------------
 * Log error and exit.
 *----------------------------------------------------------------------------*/
void ntsl_die(char *format, ...)
{
    va_list args;
    char s[MAX_ERROR_LEN];

    va_start(args, format);
    _vsnprintf(s, MAX_ERROR_LEN, format, args);
    va_end(args);

    s[MAX_ERROR_LEN-1] = '\0';
#ifdef	_DEBUG
    fprintf(stderr, "%s\n", s);
#endif
    _LogEventEx2(EVENTLOG_ERROR_TYPE, 10uL, 1u, s, NULL);

#ifndef NTSL_STUB
    if( _hSrvrStopEvent != NULL )
	SetEvent( _hSrvrStopEvent );
    SrvrIsHalting( 100uL );
#else
    exit( 1 );
#endif
}

/*--------------------------------[ ntsl_nit ]--------------------------------
 * Initialize sybsystems
 *----------------------------------------------------------------------------*/
void	ntsl_init(void)
{
#ifdef _DEBUG
    _CrtSetReportMode(_CRT_WARN,   _CRTDBG_MODE_FILE   );
    _CrtSetReportFile(_CRT_WARN,   _CRTDBG_FILE_STDOUT );
    _CrtSetReportMode(_CRT_ERROR,  _CRTDBG_MODE_FILE   );
    _CrtSetReportFile(_CRT_ERROR,  _CRTDBG_FILE_STDOUT );
    _CrtSetReportMode(_CRT_ASSERT, _CRTDBG_MODE_FILE   );
    _CrtSetReportFile(_CRT_ASSERT, _CRTDBG_FILE_STDOUT );
#endif

    engine_init();
    eventlog_init();
}

int WINAPI  SrvrInit(int argc, TCHAR *argv[])
{
    ntsl_init()	;
    return  (0)	;
}


/*------------------------------[ ntsl_shutdown ]------------------------------
 *  Shutdown subsystems
 *----------------------------------------------------------------------------*/
void	ntsl_shutdown(void)
{
    engine_shutdown();
    eventlog_shutdown();

#ifdef	_DEBUG
    _CrtDumpMemoryLeaks();
#endif
}

int	WINAPI	SrvrStop(DWORD dwCtrl)
{
    ntsl_shutdown();
    return  (0)	;
}

/*--------------------------------[ ntsl_run ]--------------------------------
 * Service event loop
 *----------------------------------------------------------------------------*/
int WINAPI  SrvrRun(int argc, TCHAR *argv[])
{
    while( !SrvrIsHalting(NTSL_BIAS_WAIT) )
    {
#ifdef _DEBUG
	printf("\nScanning event logs...\n");
#endif
	eventlog_check_events();
#ifdef _DEBUG
	printf("\nSleeping...\n");
#endif
    }

    ntsl_shutdown();
    return  (0)	;
}
