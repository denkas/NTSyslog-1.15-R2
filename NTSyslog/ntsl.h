/*-----------------------------------------------------------------------------
 *
 *  ntsl.h - Common definitions and types
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
 *  $Id: ntsl.h,v 1.3 2007/10/25 18:20:59 frzem Exp $
 *
 *----------------------------------------------------------------------------*/
#ifndef _NTSL_H_
#define _NTSL_H_

#include "service.h"
#include "error.h"

#define NTSYSLOG_SOFTWARE_KEY	"SOFTWARE\\SaberNet"
#define NTSYSLOG_SYSLOG_KEY		"Syslog"
#define	NTSYSLOG_REGISTRY_KEY	"SOFTWARE\\SaberNet\\Syslog"

#define VERSION_MAJOR	"1"
#define VERSION_MINOR	"15R2-jp01p1（日本語対応版R2）"
#define COPYRIGHT   	"Copyright (c) 1998-2007, SaberNet.net - All rights reserved.\n日本語対応版R2 Copyright (c) 2004-2013 Ryo.Sugahara / Denka's Factory"
#define APP_NAME		"NTSyslog"
#define SERVICE_NAME	"NTSyslog"
#define SERVICE_EXE		"ntsyslog.exe"

//#define NTSL_NAME_LEN		   32
//#define NTSL_DESC_LEN		   80
#define NTSL_SYS_LEN		  256
#define NTSL_DATE_LEN		   16
//#define NTSL_EVENT_LEN		 1024
#define NTSL_EVENT_LEN		 10240 // Windows Eventhing 6.0対応のために増やしてみた
//#define NTSL_PATH_LEN		 1024
//#define NTSL_PASSWD_LEN 	   64
#define NTSL_LOOP_WAIT		  220	/* milliseconds to wait for shutdown event	*/
//#define NTSL_BIAS_WAIT		90000	/* milliseconds to sleep between scans		*/
#define NTSL_BIAS_WAIT		10000	/* milliseconds to sleep between scans		*/
#define NTSL_LOG_DIR		"log"

/*------------------------[ portable type definitions ]-----------------------*/
#ifndef uchar
#define uchar  UCHAR
#endif
#ifndef uint16
#define uint16 WORD
#endif
#ifndef uint32
#define uint32 DWORD
#endif
#ifndef int16
#define int16  SHORT
#endif
#ifndef int32
#define int32  LONG
#endif
#ifndef int64
#define int64  DWORDLONG
#endif
#ifndef bool
#define bool   BOOL
#endif
#ifndef true
#define true   TRUE
#endif
#ifndef false
#define false  FALSE
#endif

/*---------------------------------[ globals ]--------------------------------*/
void	ntsl_log_info(char *format, ...);
void	ntsl_log_warning(char *format, ...);
void	ntsl_log_error(char *format, ...);
void	ntsl_log_msg(uint16 etype, char *format, ...);
void	ntsl_die(char *format, ...);

void	ntsl_init(void);
void	ntsl_run(void);
void	ntsl_shutdown(void);
#endif
