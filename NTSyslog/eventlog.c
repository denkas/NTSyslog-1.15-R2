/*-----------------------------------------------------------------------------
 *
 *  eventlog.c - Windows NT eventlog module
 *
 *    Copyright (c) 1998-2002, SaberNet.net - All rights reserved
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
 *  $Id: eventlog.c,v 1.4 2007/10/27 12:56:40 frzem Exp $
 *
 *  Revision history:
 *    22-Jul-2002  JRR  Added support for user defined event logs
 *    14-Jun-2002  LWE  Fixed compiler warning.
 *    22-Apr-2002  FK   Fixed language independence and multiple message files
 *    15-Oct-1999  JRR  Fixed handling of events w/ missing descriptions
 *    06-Jul-1998  JRR  Module completed
 *
 *    21-Oct-2007 EM
 *	- Small Code-changes and fixes ...
 *	- Other method of event-list (allocation)
 *	- Enhancement for formating Event-messages
 *    21-Oct-2007 EM
 *	- Additional (!) registry key for eventlog-filter
 *		HKEY_LOCAL_MACHINE\SOFTWARE\SaberNet\Syslog\<Source>\Default Facility=REG_DWORD:Number
 *		HKEY_LOCAL_MACHINE\SOFTWARE\SaberNet\Syslog\<Source>\Default Filter==REG_DWORD:Value,
 *		where Value is a Bit-mask: 0x01=Information, 0x02=Warning, 0x04=Error, 0x08=Success, 0x10=Failure
 *	- Default Behaviour for Event-Filter and -Facility:
 *		Following Default-Facility:	Source=Application	-> 8*1 (user-level messages)
 *						Source=Security		-> 8*4 (security/authorization messages)
 *						Source=System		-> 8*0 (kernel messages)
 *						Source=*Service*	-> 8*16 (local use 0)
 *						Source=All other	-> 8*17 (local use 2)
 *		Following Default-Filter:	Source=System, Security	-> Error, Warning, Audit Failure
 *						Source=All other	-> Error, Warning
 *
 *----------------------------------------------------------------------------*/
#include "windows.h"
#include <winevt.h>
#include <time.h>
#include <stdio.h>
#include <string.h>
#include "ntsl.h"
#include "eventlog.h"
#include "engine.h"

#pragma comment(lib, "wevtapi.lib")

/*-------------------------------[ static data ]------------------------------*/
#define REG_BUFFER_LEN			2048
#define EVENTLOG_BUFFER_LEN		(511*1024)	// Changed to support .NET
#define MAX_LOG_NAME_LEN		256
#define MAX_MSG_STRINGS			(100-1)		// FormatMessage(): %n = {1..99}
#define LAST_RUN_REG			"LastRun"
#pragma	intrinsic(_alloca)

#define EVENTLOG_NO_FLAGS			0x00
#define EVENTLOG_INFORMATION_FLAG	0x01
#define EVENTLOG_WARNING_FLAG		0x02
#define EVENTLOG_ERROR_FLAG			0x04
#define EVENTLOG_AUDIT_SUCCESS_FLAG	0x08
#define EVENTLOG_AUDIT_FAILURE_FLAG	0x10

//#define EVENTLOG_DEFAULT_FLAGS          (EVENTLOG_NO_FLAGS)
//#define EVENTLOG_DEFAULT_FLAGS          (EVENTLOG_WARNING_FLAG | EVENTLOG_ERROR_FLAG)
#define EVENTLOG_DEFAULT_FLAGS          (EVENTLOG_WARNING_FLAG | EVENTLOG_ERROR_FLAG | EVENTLOG_AUDIT_FAILURE_FLAG)
//#define EVENTLOG_DEFAULT_FLAGS          (EVENTLOG_INFORMATION_FLAG | EVENTLOG_WARNING_FLAG | EVENTLOG_ERROR_FLAG | EVENTLOG_AUDIT_SUCCESS_FLAG | EVENTLOG_AUDIT_FAILURE_FLAG)

#define EVENTLOG_DEFPRIO_INFORMATION    (8 + 5)
#define EVENTLOG_DEFPRIO_WARNING        (8 + 4)
#define EVENTLOG_DEFPRIO_ERROR          (8 + 3)
#define EVENTLOG_DEFPRIO_CRITICAL       (8 + 2)
#define EVENTLOG_DEFPRIO_AUDITSUCCESS   (8 + 7)
#define EVENTLOG_DEFPRIO_AUDITFAIL      (8 + 7)

#ifndef	_TIME32_T_DEFINED				// If no VS2005 ...
#define	__time32_t	time_t
#define	_localtime32	localtime
#define	_time32		time
#endif

#define EVT_ARRAY_SIZE 256		// 一度に読み込むWindows Eventの数（あまり多くしすぎるとRPC_S_INVALID_BOUNDになる）
#define EVT_OS_VERSION 6		// Windows Eventing 6.0を利用するWindowsバージョン（通常Vista/2008以降なので6固定）
//#define _FULLDUMP_DEBUG			// これを定義すると時間チェックをバイパスしてイベントログを出力する（デバッグ用）

/*---------------------------[ private structures ]---------------------------*/
typedef struct eventlog_data_st
{
	int 	captureFlags;

	int	informationPriority;
	int	warningPriority;
	int	errorPriority;
	int	auditSuccessPriority;
	int	auditFailurePriority;
	char	name[MAX_LOG_NAME_LEN];
} eventlog_data;

/*----------------------------[ private functions ]---------------------------*/
static int    eventlog_append_data(char *s, size_t len, const char* data, BOOL chop);
static char** eventlog_strings_to_array(char *strings, int num);
static int	eventlog_read_events(eventlog_data* eventlog, uint32 *lastrun, uint32 *thisrun);
static int  eventlog_read_lastrun(uint32 *lastrun);
static int  eventlog_write_lastrun(uint32 *lastrun);
static int    eventlog_set_event_type(ntsl_event *pEvent, int id);
static int    eventlog_set_event_priority(ntsl_event *pEvent, int type, eventlog_data* eventlog);
static int    eventlog_set_user(ntsl_event *pEvent, PSID sid, DWORD len);
static long __ReadRegDword(HKEY hReg, const char *szKey);

#define	__strncpy0(_d, _s, _l)	\
	{ strncpy((_d), (_s), (_l)); \
	(_d)[(_l)-1] = '\0' ;	}

/*-------------------------------[ static data ]------------------------------*/
static size_t eventlog_entries = 0u ;
static eventlog_data* eventlog_list = NULL;

extern OSVERSIONINFO osvi;

/*
//  Helper-functions ...
*/
#ifndef	__FormatMessage
DWORD	pascal	__FormatMessage(DWORD dwFlags, LPCVOID lpSource, DWORD dwMessageId,
			DWORD dwLanguageId, LPTSTR lpBuffer, DWORD nSize, va_list *Arguments)
{
    DWORD   bytes = 0 ;

    if( !dwLanguageId )
    {
	if( (bytes = FormatMessage(dwFlags, lpSource,
		dwMessageId, GetSystemDefaultLangID(),
		lpBuffer, nSize, Arguments)) != 0 )
	    return bytes;

	if( dwFlags & FORMAT_MESSAGE_ALLOCATE_BUFFER )
	{
	    if( *(LPTSTR*)lpBuffer != NULL )
		LocalFree((HLOCAL)*(LPTSTR*)lpBuffer);
	    *(LPTSTR*)lpBuffer = NULL;
	}
    }

    bytes = FormatMessage(dwFlags, lpSource, dwMessageId,
		dwLanguageId, lpBuffer, nSize, Arguments) ;
    return bytes;
 }
#endif

#if !defined(__STDC__) && (_MSC_VER >= 1400)
__forceinline int __isPrint(char chVal)
#else
static	int __isPrint(char chVal)
#endif
{
    return (
	(((char)0x20 <= chVal) && (chVal <= (char)0x7E)) ||
	(((BYTE)0x80 <= (BYTE)chVal) && ((BYTE)chVal < (BYTE)0xff)) ?
	((int)chVal & 0x0ff) : (0) );
}

/*--------------------------[ eventlog_append_data ]--------------------------
 * Appends up to n bytes of data to the end of the buffer. (Null terminating) 
 *
 *	Returns:
 *		success 	Length of String
 *		failure		-1 
 *----------------------------------------------------------------------------*/

static int eventlog_append_data(char *buffer, size_t n, const char *data, BOOL chop)
{
    if( (chop != FALSE) && (data != NULL) )
    {
	while( (*data == ' ') || !__isPrint(*data) )
	    ++data;
    }

    if ( (buffer != NULL) && (data != NULL) )
    {
	size_t i = strlen(buffer);

	while( (*data != '\0') && (i < n) )
	{
	    if( ((data[0] == '%') && (data[1] == '%'))
		&& (('0' <= data[2]) && (data[2] <= '9')) )
	    {
		DWORD dwID = (DWORD)atoi(data+2);
		LPCVOID errorMsg = NULL;

		DWORD bytes = __FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER |
			FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS | 0xff,
			NULL, dwID, 0, (LPTSTR)&errorMsg, 0, NULL);
		buffer[i] = '\0';
		if( bytes > 0 )
		{
		    i = (size_t)eventlog_append_data(buffer, n, (const char*)errorMsg, TRUE);
		    LocalFree((HANDLE)errorMsg);
		    data += 2;
		    while( ('0' <= *data) && (*data <= '9') )
			++data;
		    continue;		// REPEAT with while-LOOP !
		}
	    }

	    if( __isPrint(*data) )
		buffer[i++] = *data;
	    else
	    {
		if( *data == '\n' )	// Replace LineFeed's
		    buffer[i++] = ' ';
	    }
	    data++;
	}

	while( (i >= n) || (((chop != FALSE) && (i > 0))
	    && ((buffer[i-1] == ' ') || !__isPrint(buffer[i-1]))) )
	    --i;
	buffer[i] = '\0';
	return (int)i;
    }

// Otherwise: Error ...
    return (-1);
}

/*------------------------[ eventlog_strings_to_array ]------------------------
 * Converts a concatenation of null terminated strings to an array of strings.
 * (Null terminating)
 *----------------------------------------------------------------------------*/
static char **eventlog_strings_to_array(char *strings, int num)
{
    register int i;
    static char *pszArray[MAX_MSG_STRINGS + 1];

    if (strings == NULL)
    {
	pszArray[0] = NULL;
	return (pszArray);
    }

    if (num > MAX_MSG_STRINGS)
	num = MAX_MSG_STRINGS;

    for( i=0; i<num; i++ )
    {
	pszArray[i] = strings;
	strings += strlen(strings) + 1;
    }
    pszArray[i] = NULL ;
    return (pszArray);
}

/*--------------------------[ eventlog_check_event ]--------------------------
 * Returns non-zero value if interested in the event; otherwise returns 0
 *----------------------------------------------------------------------------*/
static int eventlog_check_event(eventlog_data* eventlog, int id)
{
    int rc = 0;

    switch(id)
    {
    case EVENTLOG_ERROR_TYPE:		
	rc = (eventlog->captureFlags & EVENTLOG_ERROR_FLAG);
	break;
    case EVENTLOG_WARNING_TYPE:     
	rc = (eventlog->captureFlags & EVENTLOG_WARNING_FLAG);  
	break;
    case EVENTLOG_AUDIT_FAILURE:    
	rc = (eventlog->captureFlags & EVENTLOG_AUDIT_FAILURE_FLAG);
	break;
    case EVENTLOG_AUDIT_SUCCESS:    
	rc = (eventlog->captureFlags & EVENTLOG_AUDIT_SUCCESS_FLAG);     
    break;
    case EVENTLOG_INFORMATION_TYPE: 
	rc = (eventlog->captureFlags & EVENTLOG_INFORMATION_FLAG);     
	break;
    }
    return(rc);
}

/*-------------------------[ eventlog_set_event_type ]-------------------------
 * Sets the type for the given event.
 *----------------------------------------------------------------------------*/
static int eventlog_set_event_type(ntsl_event *pEvent, int id)
{
    if( pEvent != NULL )
    {
	switch( id )
	{
	case EVENTLOG_ERROR_TYPE:
		pEvent->eType = NTSL_EVENT_ERROR;
		return (0);
		
	case EVENTLOG_WARNING_TYPE:
		pEvent->eType = NTSL_EVENT_WARNING;
		return (0);

	case EVENTLOG_INFORMATION_TYPE:
		pEvent->eType = NTSL_EVENT_INFORMATION;
		return (0);

	case EVENTLOG_AUDIT_SUCCESS:
		pEvent->eType = NTSL_EVENT_SUCCESS;
		return (0);

	case EVENTLOG_AUDIT_FAILURE:
		pEvent->eType = NTSL_EVENT_FAILURE;
		return (0);

	default:
		pEvent->eType = (const char*)NULL ;
		break;
	}
    }
    return (-1);
}

/*-----------------------[ eventlog_set_event_priority ]-----------------------
 * Sets the priority for the given event.
 *----------------------------------------------------------------------------*/
static int eventlog_set_event_priority(ntsl_event *pEvent, int type, eventlog_data* eventlog)
{
    if( NULL != pEvent )
    {
	switch( type )				// Set default.
	{
	case EVENTLOG_ERROR_TYPE:
		pEvent->priority = eventlog->errorPriority;
		return (0);

	case EVENTLOG_WARNING_TYPE:
		pEvent->priority = eventlog->warningPriority;
		return (0);

	case EVENTLOG_INFORMATION_TYPE:
		pEvent->priority = eventlog->informationPriority;
		return (0);

	case EVENTLOG_AUDIT_SUCCESS:
		pEvent->priority = eventlog->auditSuccessPriority;
		return (0);

	case EVENTLOG_AUDIT_FAILURE:
		pEvent->priority = eventlog->auditFailurePriority;
		return (0);

	default:
		pEvent->priority = NTSL_DEFAULT_PRIORITY;
		return (0);
	}
    }
    return (-1);
}


/*-------------------------[ eventlog_set_event_msg ]-------------------------
 * Retrieves the event message from the appropriate DLL.
 *
 *	Returns:
 *		success		0
 *		failure		-1 
 *----------------------------------------------------------------------------*/

static int eventlog_set_event_msg(ntsl_event *pEvent, char *logType,
			uint32 id, char *strings, int numStrings)
{
    char**  ppszArray;
    char    buffer[REG_BUFFER_LEN];
    char    dll[REG_BUFFER_LEN];
    uint32  bufsize = REG_BUFFER_LEN;
    uint32  bytes   = 0;
    uint32  regtype = 0;
    HANDLE  hlib;
    HKEY    hkey;
    uint32  sequencenumber = 0;
    char    singeldll[REG_BUFFER_LEN];
    BOOL    handled = false;

	/* check paramaters */
    if( (pEvent == NULL) || (logType == NULL) || (strings == NULL) )
	return (-1);

    ppszArray = eventlog_strings_to_array(strings, numStrings);
    pEvent->dwCode = (id & 0x0000FFFF);
	
	/* check strings array */
    if ( (numStrings) && (ppszArray[0] == NULL) )
	return (-1);

	/* build registry path */
    _snprintf(buffer, REG_BUFFER_LEN,
	"SYSTEM\\CurrentControlSet\\Services\\EventLog\\%s\\%s",
	logType, pEvent->szSource);
    buffer[REG_BUFFER_LEN-1] = '\0';

	/* load message text */
    if( RegOpenKey(HKEY_LOCAL_MACHINE, buffer, &hkey) == ERROR_SUCCESS )
    {
		if( RegQueryValueEx(hkey, "EventMessageFile", 0, &regtype, 
			(unsigned char*)buffer, &bufsize) == ERROR_SUCCESS )
		{
		    if( (regtype == REG_SZ) || ( ExpandEnvironmentStrings(buffer, dll, REG_BUFFER_LEN) > 0) )
		    {
				if(regtype == REG_SZ)
				{ strcpy_s(dll, REG_BUFFER_LEN, buffer); }

				/* Parse into different ones if needed */
				while( !eventlog_parse_libs(dll,singeldll,sequencenumber) )
				{
				    if( (hlib = LoadLibraryEx(singeldll, NULL, LOAD_LIBRARY_AS_DATAFILE)) != NULL )
				    {
						LPCVOID msg  = NULL;
					
						bytes = __FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER |
												FORMAT_MESSAGE_FROM_HMODULE |
												FORMAT_MESSAGE_ARGUMENT_ARRAY | 60, hlib, id, 
												0, (LPTSTR)&msg, NTSL_EVENT_LEN, ppszArray);

						if( bytes > 0 )
						{
						    handled = true;
						    pEvent->msg[0] = (char)0;
						    eventlog_append_data(pEvent->msg, NTSL_EVENT_LEN, (char *)msg, TRUE);
						    LocalFree((HANDLE)msg);
						}
						FreeLibrary((HMODULE)hlib);
					}
					if( bytes > 0 ) { break; }
					sequencenumber++;
				}
		    }
		}
		RegCloseKey(hkey);
    }

    if( handled )
    {
	; // handled above
    }
    else
    {
		if (numStrings > 0)
		{
		    register int i;

			for( i=0 ; ppszArray[i] != NULL ; i++ )
			{
				eventlog_append_data(pEvent->msg, NTSL_EVENT_LEN, ppszArray[i], FALSE);
				eventlog_append_data(pEvent->msg, NTSL_EVENT_LEN, " ", FALSE);
		    }
		}
		else
		{
			eventlog_append_data(pEvent->msg, NTSL_EVENT_LEN, "No description available", FALSE);
	    }
    }
    return (0);
}

/*--------------------------[ eventlog_read_events ]--------------------------
 * Read messages from the event log 
 *
 *	Returns:
 *		success		(0)
 *		error		(-1)	
 *----------------------------------------------------------------------------*/
static int eventlog_read_events(eventlog_data* eventlog,
			uint32 *lastrun, uint32 *thisrun)
{
	char    buffer[EVENTLOG_BUFFER_LEN];

    if( (lastrun == NULL) || (thisrun == NULL) || (buffer == NULL) ) return (-1);

	// Windows Vista/2008以前は従来のReadEventLog
	if (osvi.dwMajorVersion < EVT_OS_VERSION)
	{
		int	    useTimeStamp = 1;
		uint32  bytes;
		uint32  next;
		HANDLE  hLog;
		int	    status;

		if( (hLog = OpenEventLog(NULL, eventlog->name)) == NULL ) 
		{
			ntsl_log_error(NTSL_ERROR_EVENT_LOG_ACCESS, eventlog->name);
			return (-1);
		}

#ifdef _DEBUG
		printf("Reading %s event log...\n", eventlog->name);
#endif

		while( (status=ReadEventLog(hLog, EVENTLOG_FORWARDS_READ | EVENTLOG_SEQUENTIAL_READ,
				 0, buffer, EVENTLOG_BUFFER_LEN, &bytes, &next)) )
		{
			EVENTLOGRECORD *record = (EVENTLOGRECORD*)buffer;

			if( SrvrIsHalting(NTSL_LOOP_WAIT) )	// check for service shutdown
			    return (-1);

			while( bytes > 0 ) 
			{ 
			    ntsl_event	*pEvent;
				char	*source = (LPSTR) ((LPBYTE) record + sizeof(EVENTLOGRECORD));
				char	*computer = source + strlen(source) + 1;
				char	*strings  = (LPSTR) ((LPBYTE) record + record->StringOffset);
				struct tm	*time = _localtime32((__time32_t*)&record->TimeGenerated);

/*
FQDN Cutter Logic 2011.7.26 denka
以下の内容でregファイルを作成して入力することで機能をONにできる
---
Windows Registry Editor Version 5.00

32bit [HKEY_LOCAL_MACHINE\SOFTWARE\SaberNet\Syslog]
64bit [HKEY_LOCAL_MACHINE\SOFTWARE\Wow6432Node\SaberNet\Syslog]
"FullComputerName"=dword:00000001
---
*/
				// FQDN Cutter Definition
				char fqdn_cutter_buffer[256];
				int fqdn_cutter_cnt = 0;
				extern BOOL bFQDNoutput;

				// FQDN Cutter
				if (!bFQDNoutput)
				{
					strncpy(fqdn_cutter_buffer,computer,256);
					for (fqdn_cutter_cnt=0;(unsigned int)fqdn_cutter_cnt<strlen(computer);fqdn_cutter_cnt++)
					{
						if (fqdn_cutter_buffer[fqdn_cutter_cnt] == '.') fqdn_cutter_buffer[fqdn_cutter_cnt] = '\0';
					}
					computer = fqdn_cutter_buffer;
				}

/* If this event happened between the last time we ran, and the current
    time, and it is one we are interested in, then fill in an ntsl_event
    structure, and pass it to the engine to log.
*/
#ifdef _FULLDUMP_DEBUG
				*lastrun = 0; *thisrun = record->TimeWritten; // デバッグ用
#endif
			    if( (record->TimeWritten >  (uint32)*lastrun)
					&& (record->TimeWritten <= (uint32)*thisrun)
					&& (eventlog_check_event(eventlog, record->EventType))
					&& !(strcmp(source, "NTSYSLOG") == 0 && record->EventType == EVENTLOG_ERROR_TYPE))
			    {
					pEvent = (ntsl_event*)LocalAlloc(LPTR, sizeof(ntsl_event));
					if( pEvent == NULL )
					{
					    ntsl_log_error(NTSL_ERROR_EVENT_MALLOC);
					    return (-1);
					}	

					pEvent->msg[0] = 0;
					strftime(pEvent->date, NTSL_DATE_LEN, "%b %d %H:%M:%S", time);
					if (pEvent->date[4] == '0')	// Unix style formatting
						pEvent->date[4] = ' ';
					__strncpy0(pEvent->host, computer, sizeof(pEvent->host));
					__strncpy0(pEvent->szSource, source, sizeof(pEvent->szSource));
					eventlog_set_event_type(pEvent, record->EventType);
					eventlog_set_event_priority(pEvent, record->EventType, eventlog);
					eventlog_set_event_msg(pEvent, eventlog->name, record->EventID, strings, record->NumStrings);
					eventlog_set_user(pEvent, ((LPBYTE) record + record->UserSidOffset), record->UserSidLength);

					engine_process_event(pEvent, NULL /*host->group*/);
					pEvent = NULL;
			    }
			
			    bytes -= record->Length; 
			    record = (EVENTLOGRECORD *) ((LPBYTE) record + record->Length); 
			}
		}

	    if ( (status == 0) && (GetLastError() == ERROR_INSUFFICIENT_BUFFER) )
	    {
			ntsl_log_error(NTSL_ERROR_EVENT_READ_BUF, EVENTLOG_BUFFER_LEN, next);
	    }
 
		CloseEventLog(hLog);
	}

	// Windows Eventing 6.0による読み込み
	else
	{
		EVT_HANDLE hChannels = NULL;
		EVT_HANDLE hReadEvent = NULL;

		LPWSTR pBuffer = NULL;
	    LPWSTR pTemp = NULL;
		LPWSTR pMessageBuffer = NULL;
		DWORD dwBufferSize = 0;
		DWORD dwBufferUsed = 0;
		DWORD status = ERROR_SUCCESS;

		DWORD dwPropertyCount = 0;
		PEVT_VARIANT pRenderedContent = NULL;

		EVT_HANDLE hEvents[EVT_ARRAY_SIZE];
		DWORD dwReturned = 0;
		DWORD dwUsedBuffer = 0;

		EVT_HANDLE eventlog_context_handle;
		EVT_HANDLE eventlog_providermetadata_handle;
		LPWSTR query_array[] = {
			L"/Event/System/Provider/@Name",
			L"/Event/System/EventRecordID",
			L"/Event/System/Level",
			L"Application"};
		DWORD array_count = 4;

		ULONGLONG tmp_time;
		FILETIME ftime;
		wchar_t wtmp_buffer[1024*3];
		DWORD i;

#if 0
		FILETIME lftime;
		SYSTEMTIME systime;
#endif

		memset(wtmp_buffer,0,sizeof(wtmp_buffer));
		MultiByteToWideChar(CP_ACP, 0, eventlog->name, strlen(eventlog->name), wtmp_buffer, sizeof(wtmp_buffer));

#ifdef _DEBUG
			printf("reading...: %s\n",eventlog->name);
#endif

		hReadEvent = EvtQuery(NULL,wtmp_buffer,L"*",EvtQueryChannelPath);
		if (hReadEvent == NULL)
		{
#ifdef _DEBUG
			printf("Query failed: %s\n",eventlog->name);
#endif
			ntsl_log_error("Query failed: %s\n",eventlog->name);
			return (-1);
		}

		while (EvtNext(hReadEvent, EVT_ARRAY_SIZE, hEvents, INFINITE, 0, &dwReturned))
		{
			pRenderedContent = (PEVT_VARIANT)malloc(NTSL_EVENT_LEN);

			for (i=0;i<dwReturned;i++)
			{
			    ntsl_event	*pEvent;
				char	source[1024];
				char	computer[1024];
				char	strings[NTSL_EVENT_LEN];
				struct tm*	time;
			    LONGLONG ll;

				__time32_t TimeWritten = 0;
				DWORD	EventID = 0;
				WORD	EventType = 0;
				int		pSIDLength = 0;

				EVT_HANDLE hProviders = NULL;
				LPWSTR pwcsProviderName = NULL;

				// FQDN Cutter Definition
				int fqdn_cutter_cnt = 0;
				extern BOOL bFQDNoutput;

				eventlog_context_handle = EvtCreateRenderContext(0, NULL, EvtRenderContextSystem);
				if (!EvtRender(eventlog_context_handle, hEvents[i], EvtRenderEventValues, NTSL_EVENT_LEN, pRenderedContent, &dwBufferUsed, &dwPropertyCount))
				{
#ifdef _DEBUG
					printf ("EvtRender failed\n");
					printf ("Error code: %d\n",GetLastError());
#endif
				    ntsl_log_error("EvtRender failed, Error code: %d\n",GetLastError());

					EvtClose(hEvents[i]);
					hEvents[i] = NULL;
					EvtClose(eventlog_context_handle);
					continue;
				}

				// 時刻取得
				tmp_time = pRenderedContent[EvtSystemTimeCreated].FileTimeVal;

				// FILETIME形式に代入した後、time_t形式に変換する
				ftime.dwHighDateTime = (DWORD)((tmp_time >> 32) & 0xFFFFFFFF);
				ftime.dwLowDateTime = (DWORD)(tmp_time & 0xFFFFFFFF);

				ll = ((LONGLONG)ftime.dwHighDateTime << 32) + ftime.dwLowDateTime;
				ll = (ll - 116444736000000000) / 10000000;
				TimeWritten = (unsigned int)ll;

#ifdef _FULLDUMP_DEBUG
				*lastrun = 0; *thisrun = TimeWritten; // デバッグ用
#endif
				// パフォーマンス改善のため時間チェックだけ先に実施する
				if ( ((uint32)TimeWritten < (uint32)*lastrun) || ((uint32)TimeWritten >= (uint32)*thisrun) )
				{
					EvtClose(hEvents[i]);
					hEvents[i] = NULL;
					EvtClose(eventlog_context_handle);
					continue;
				}

				time = _localtime32(&TimeWritten);

				// ソース名取得
				WideCharToMultiByte(CP_THREAD_ACP,0, pRenderedContent[EvtSystemProviderName].StringVal, -1, source, sizeof(source), NULL, NULL);

				// コンピュータ名取得
				WideCharToMultiByte(CP_THREAD_ACP,0, pRenderedContent[EvtSystemComputer].StringVal, -1, computer, sizeof(computer), NULL, NULL);

				// FQDN Cutter
				if (!bFQDNoutput)
				{
					for (fqdn_cutter_cnt=0;(unsigned int)fqdn_cutter_cnt<strlen(computer);fqdn_cutter_cnt++)
					{
						if (computer[fqdn_cutter_cnt] == '.') computer[fqdn_cutter_cnt] = '\0';
					}
				}

				// メッセージ本体取得
			    ZeroMemory(&strings, sizeof(strings)) ;
				eventlog_providermetadata_handle = EvtOpenPublisherMetadata(NULL, pRenderedContent[EvtSystemProviderName].StringVal, NULL, 0, 0);
				if (NULL == eventlog_providermetadata_handle)
				{
					LPWSTR pXMLBuffer = NULL;
					char strings_tmp[NTSL_EVENT_LEN];
					char *pXMLPoint_s, *pXMLPoint_e;

					pXMLBuffer = (LPWSTR)malloc(NTSL_EVENT_LEN);
					if (!EvtRender(NULL, hEvents[i], EvtRenderEventXml, NTSL_EVENT_LEN, pXMLBuffer, &dwBufferUsed, &dwPropertyCount))
					{
#ifdef _DEBUG
						printf ("EvtOpenPublisherMetadata: EvtRender failed\n");
						printf ("Error code: %d\n",GetLastError());
#endif
						strcpy(strings, _T("このイベントを発生させるコンポーネントがローカル コンピューターにインストールされていないか、インストールが壊れています。"));
					}
					else
					{
						WideCharToMultiByte(CP_THREAD_ACP,0,pXMLBuffer, -1, strings_tmp, sizeof(strings_tmp), NULL, NULL);
						while( (pXMLPoint_s = strstr(strings_tmp, _T("<Data>"))) != NULL )
						{
							pXMLPoint_e = strstr(strings_tmp, _T("</Data>"));

							strncpy(strings+strlen(strings), pXMLPoint_s+6, pXMLPoint_e-(pXMLPoint_s+6));
							strcat(strings, " ");

							strcpy(strings_tmp, pXMLPoint_e+7);
						}
					}
					if (pXMLBuffer != NULL) free(pXMLBuffer);
				}
//				pMessageBuffer = (LPWSTR)malloc(NTSL_EVENT_LEN);
				pMessageBuffer = (LPWSTR)malloc(NTSL_EVENT_LEN * sizeof(WCHAR) / sizeof(TCHAR));
				memset(pMessageBuffer,0,NTSL_EVENT_LEN);

				if (strlen(strings) == 0 && !EvtFormatMessage(eventlog_providermetadata_handle, hEvents[i], 0, 0, NULL, EvtFormatMessageEvent, NTSL_EVENT_LEN, pMessageBuffer, &dwUsedBuffer))
				{
					LPWSTR pXMLBuffer = NULL;
					char strings_tmp[NTSL_EVENT_LEN];
					char *pXMLPoint_s, *pXMLPoint_e;

					pXMLBuffer = (LPWSTR)malloc(NTSL_EVENT_LEN);
					if (!EvtRender(NULL, hEvents[i], EvtRenderEventXml, NTSL_EVENT_LEN, pXMLBuffer, &dwBufferUsed, &dwPropertyCount))
					{
#ifdef _DEBUG
						printf ("EvtFormatMessage: EvtRender failed\n");
						printf ("Error code: %d\n",GetLastError());
#endif
						strcpy(strings, _T("メッセージ リソースは存在しますが、メッセージが文字列テーブル/メッセージ テーブルに見つかりません。"));
					}
					else
					{
						WideCharToMultiByte(CP_THREAD_ACP,0,pXMLBuffer, -1, strings_tmp, sizeof(strings_tmp), NULL, NULL);
						while( (pXMLPoint_s = strstr(strings_tmp, _T("<Data>"))) != NULL )
						{
							pXMLPoint_e = strstr(strings_tmp, _T("</Data>"));

							strncpy(strings+strlen(strings), pXMLPoint_s+6, pXMLPoint_e-(pXMLPoint_s+6));
							strcat(strings, " ");

							strcpy(strings_tmp, pXMLPoint_e+7);
						}
					}
					if (pXMLBuffer != NULL) free(pXMLBuffer);
				}
				if (strlen(strings) == 0) WideCharToMultiByte(CP_THREAD_ACP,0, pMessageBuffer, -1, strings, sizeof(strings), NULL, NULL);

				// イベントID取得
				EventID = pRenderedContent[EvtSystemEventID].UInt16Val;

				// イベントタイプ変換
				if (pRenderedContent[EvtSystemLevel].ByteVal == 0x01) EventType = EVENTLOG_ERROR_TYPE;
				if (pRenderedContent[EvtSystemLevel].ByteVal == 0x02) EventType = EVENTLOG_ERROR_TYPE;
				if (pRenderedContent[EvtSystemLevel].ByteVal == 0x03) EventType = EVENTLOG_WARNING_TYPE;
				if (pRenderedContent[EvtSystemLevel].ByteVal == 0x04) EventType = EVENTLOG_INFORMATION_TYPE;
				if (pRenderedContent[EvtSystemLevel].ByteVal == 0x00 && pRenderedContent[EvtSystemKeywords].UInt64Val == 0x8020000000000000) EventType = EVENTLOG_AUDIT_SUCCESS;
				if (pRenderedContent[EvtSystemLevel].ByteVal == 0x00 && pRenderedContent[EvtSystemKeywords].UInt64Val == 0x00a0000000000000) EventType = EVENTLOG_AUDIT_SUCCESS; // クラシック;成功の監査
				if (pRenderedContent[EvtSystemLevel].ByteVal == 0x00 && pRenderedContent[EvtSystemKeywords].UInt64Val == 0x8010000000000000) EventType = EVENTLOG_AUDIT_FAILURE;
				if (pRenderedContent[EvtSystemLevel].ByteVal == 0x00 && pRenderedContent[EvtSystemKeywords].UInt64Val == 0x0090000000000000) EventType = EVENTLOG_AUDIT_FAILURE; // クラシック;失敗の監
#if 0
				printf ("EventID: %d\n",EventID);
				printf ("Source: %s\n",source);
				printf ("Computer: %s\n",computer);

				FileTimeToLocalFileTime(&ftime, &lftime);
				FileTimeToSystemTime(&lftime, &systime);
				printf ("TimeCreated SystemTime(FILETIME): %02d/%02d/%02d %02d:%02d:%02d.%I64u)\n", 
					systime.wMonth, systime.wDay, systime.wYear, systime.wHour, systime.wMinute, systime.wSecond, systime.wMilliseconds);

				printf ("TimeCreated SystemTime(time_t): %02d/%02d/%02d %02d:%02d:%02d)\n", 
					 time->tm_mon+1,  time->tm_mday, time->tm_year+1900,  time->tm_hour,  time->tm_min,  time->tm_sec);
				printf ("Message: %s\n",strings);
#endif

/* If this event happened between the last time we ran, and the current
   time, and it is one we are interested in, then fill in an ntsl_event
   structure, and pass it to the engine to log.
*/
				if( ((uint32)TimeWritten >  (uint32)*lastrun)
					&& ((uint32)TimeWritten <= (uint32)*thisrun)
					&& (eventlog_check_event(eventlog, EventType))
					&& !(strcmp(source, "NTSYSLOG") == 0 && EventType == EVENTLOG_ERROR_TYPE))
			    {
					pEvent = (ntsl_event*)LocalAlloc(LPTR, sizeof(ntsl_event));
					if( pEvent == NULL )
					{
					    ntsl_log_error(NTSL_ERROR_EVENT_MALLOC);
					    return (-1);
					}	

					pEvent->msg[0] = 0;
					strftime(pEvent->date, NTSL_DATE_LEN, "%b %d %H:%M:%S", time);
					if (pEvent->date[4] == '0')	// Unix style formatting
						pEvent->date[4] = ' ';
					__strncpy0(pEvent->host, computer, sizeof(pEvent->host));
					__strncpy0(pEvent->szSource, source, sizeof(pEvent->szSource));
					eventlog_set_event_type(pEvent, EventType);
					eventlog_set_event_priority(pEvent, EventType, eventlog);
//					eventlog_set_event_msg(pEvent, eventlog->name, EventID, strings, 1);
					strcpy(pEvent->msg,strings);
				    pEvent->dwCode = (EventID & 0x0000FFFF);
					if (pRenderedContent[EvtSystemUserID].SidVal == NULL) pSIDLength = 0; else pSIDLength = 1;
					eventlog_set_user(pEvent, pRenderedContent[EvtSystemUserID].SidVal, pSIDLength);

					engine_process_event(pEvent, NULL /*host->group*/);
					pEvent = NULL;
				}

				free(pMessageBuffer);
				EvtClose(eventlog_providermetadata_handle);
				EvtClose(hEvents[i]);
				hEvents[i] = NULL;
				EvtClose(eventlog_context_handle);
			}
			free(pRenderedContent);
		}

		EvtClose(hReadEvent);
	}

	return (0);
}


/*-------------------------[ eventlog_read_lastrun ]---------------------------
 *  Reads last run times
 *
 *	Returns:
 *		success		(0)
 *		error		(-1)	
 *----------------------------------------------------------------------------*/
static long __ReadRegDword(HKEY hReg, const char *szKey)
{
    LONG    rc;
    DWORD   val, typ;
    DWORD   size = sizeof(val);

    rc = RegQueryValueEx(hReg, szKey, NULL, &typ, (BYTE*)&val, &size) ;
    if( (rc == ERROR_SUCCESS) && (typ == REG_DWORD) )
        return  (long)val ;
// Failure: Type or Value does not exist!
    return  -1L;
}

static int eventlog_read_lastrun(uint32 *lastrun)
{
    HKEY    hReg;
    DWORD   dummy = 0;
    long    rc, rv;

    if( lastrun == NULL )
	return (-1);

    //rc = RegOpenKeyEx(HKEY_LOCAL_MACHINE, NTSYSLOG_REGISTRY_KEY, 0, KEY_READ, &hReg);
    rc = RegCreateKeyEx(HKEY_LOCAL_MACHINE, NTSYSLOG_REGISTRY_KEY,
	0, REG_NONE, REG_OPTION_NON_VOLATILE, KEY_READ, NULL, &hReg, &dummy);
    if( rc != ERROR_SUCCESS )
	return (-1);

    rv = __ReadRegDword(hReg, LAST_RUN_REG) ;
    RegCloseKey(hReg);
    *lastrun = ( (rv == -1L)? 0 : (uint32)rv );
    return (0);
}

/*--------------------------[ eventlog_write_lastrun ]-----------------------
 *  Writes last run time
 *
 *	Returns:
 *		success		(0)
 *		error		(-1)	
 *----------------------------------------------------------------------------*/
static int eventlog_write_lastrun(uint32 *lastrun)
{
    HKEY    hReg;
    int32   size = sizeof(*lastrun);
    int32   rc, rv;

    if( lastrun == NULL )
	return (-1);

    rc = RegOpenKeyEx(HKEY_LOCAL_MACHINE, NTSYSLOG_REGISTRY_KEY, 0, KEY_SET_VALUE, &hReg);
    if( rc != ERROR_SUCCESS )
	return (-1);

    rv	= RegSetValueEx(hReg, LAST_RUN_REG, 0, REG_DWORD, (BYTE*)lastrun, size);
    RegCloseKey(hReg);
    if( rv != ERROR_SUCCESS )
	ntsl_die(NTSL_ERROR_TIME_DATA_WRITE, LAST_RUN_REG);
    return (0);
}


/*--------------------------[ eventlog_check_events ]--------------------------
 * Locate recent event entries to be processed
 *----------------------------------------------------------------------------*/
void eventlog_check_events(void)
{
    register int running = 1;
    register size_t n;
    uint32  lastrun;
    uint32  thisrun;

    _time32((__time32_t*)&thisrun);
    if( eventlog_read_lastrun(&lastrun) == 0 )
    {
		if( lastrun > 0 )
		{
		    for( n = 0u ; (n < eventlog_entries) && (running != 0) ; n++ )
		    {
				eventlog_data log = eventlog_list[n];

				if( log.captureFlags != EVENTLOG_NO_FLAGS )
					eventlog_read_events(&log, &lastrun, &thisrun);
		    }
		}

		if( running ) eventlog_write_lastrun(&thisrun);
    }
}

/*----------------------------[ eventlog_read_reg ]----------------------------
 * Read eventlog registry settings 
 *----------------------------------------------------------------------------*/
static long __eventlog_flags(HKEY hReg, const char *szKey, eventlog_data* eventlog, int uiMask)
{
    int32 rv;
    char szBuffer[48];

    strncpy(szBuffer, szKey, sizeof(szBuffer)-10);
    szBuffer[sizeof(szBuffer)-10] = '\0';
    if( (rv = __ReadRegDword(hReg, szBuffer)) >= 0 )
    {
	if( rv == 0 )
	    eventlog->captureFlags &= ~uiMask ;	// Flag(s) aktualisieren
	else
	    eventlog->captureFlags |=  uiMask ;
    }

    strcat(szBuffer, " Priority");
    return __ReadRegDword(hReg, szBuffer);
}

static void eventlog_read_reg(eventlog_data* eventlog)
{
    char    buffer[REG_BUFFER_LEN];
    HKEY    hReg;
    int32   rc, rv;

    if( eventlog )
    {
		int temp, iPriority = 8*17 ;		// Default Facility

		if( !_stricmp(eventlog->name, "Application") )
			iPriority = 8*1 ;
		else if( !_stricmp(eventlog->name, "Security") )
			iPriority = 8*4 ;
		else if( !_stricmp(eventlog->name, "System") )
			iPriority = 8*0 ;
		else
		{
			for( temp=0 ; eventlog->name[temp]!='\0' ; temp++ )
			if( !_strnicmp(&eventlog->name[temp], "Service", 7) )
			{
				iPriority = 8*16 ;
				break;
			}
		}

		_snprintf(buffer, REG_BUFFER_LEN, "%s\\%s", NTSYSLOG_REGISTRY_KEY, eventlog->name);
		buffer[REG_BUFFER_LEN-1] = '\0'	;
		rc = RegOpenKeyEx(HKEY_LOCAL_MACHINE, buffer, 0, KEY_READ, &hReg);
		if (rc == ERROR_SUCCESS)
		{
			rv = __ReadRegDword(hReg, "Default Facility");
			if( (rv >= 0) && (rv < 24) )
			iPriority = 8 * (int)rv	;
		}

		// Set up the structure with clean defaults.
		eventlog->captureFlags = EVENTLOG_NO_FLAGS;

		eventlog->informationPriority = iPriority + (EVENTLOG_DEFPRIO_INFORMATION & 0x07);
		eventlog->warningPriority = iPriority + (EVENTLOG_DEFPRIO_WARNING & 0x07);
		eventlog->errorPriority = iPriority + (EVENTLOG_DEFPRIO_ERROR & 0x07);
		eventlog->auditSuccessPriority = iPriority + (EVENTLOG_DEFPRIO_AUDITSUCCESS & 0x07);
		eventlog->auditFailurePriority = iPriority + (EVENTLOG_DEFPRIO_AUDITFAIL & 0x07);

		if( rc == ERROR_SUCCESS )
		{
			if( (rv = __ReadRegDword(hReg, "Default Filter")) > 0 )
			eventlog->captureFlags = (BYTE)rv ;

			rv = __eventlog_flags(hReg, "Information", eventlog, EVENTLOG_INFORMATION_FLAG) ;
			if( (0 <= rv) && (rv < 24*8) )
			eventlog->informationPriority = (int)rv ;

			rv = __eventlog_flags(hReg, "Warning", eventlog, EVENTLOG_WARNING_FLAG) ;
			if( (0 <= rv) && (rv < 24*8) )
			eventlog->warningPriority = (int)rv ;

			rv = __eventlog_flags(hReg, "Error", eventlog, EVENTLOG_ERROR_FLAG) ;
			if( (0 <= rv) && (rv < 24*8) )
			eventlog->errorPriority = (int)rv ;
	
			rv = __eventlog_flags(hReg, "Audit Success", eventlog, EVENTLOG_AUDIT_SUCCESS_FLAG);
			if( (0 <= rv) && (rv < 24*8) )
			eventlog->auditSuccessPriority = (int)rv;
	
			rv = __eventlog_flags(hReg, "Audit Failure", eventlog, EVENTLOG_AUDIT_FAILURE_FLAG);
			if( (0 <= rv) && (rv < 24*8) )
			eventlog->auditFailurePriority = (int)rv;
			RegCloseKey(hReg);
    	}
		else
		{
			eventlog->captureFlags = EVENTLOG_DEFAULT_FLAGS;

			// 登録が無いときは強制的にNTSL_DEFAULT_PRIORITYに設定
			eventlog->informationPriority = NTSL_DEFAULT_PRIORITY;
			eventlog->warningPriority = NTSL_DEFAULT_PRIORITY;
			eventlog->errorPriority = NTSL_DEFAULT_PRIORITY;
			eventlog->auditSuccessPriority = NTSL_DEFAULT_PRIORITY;
			eventlog->auditFailurePriority = NTSL_DEFAULT_PRIORITY;

			if( (iPriority / 8) == 0 || (iPriority / 8) == 4 )	// 'System' or 'Security'
			eventlog->captureFlags |= (EVENTLOG_AUDIT_FAILURE_FLAG);
#if	0
			ntsl_die(NTSL_ERROR_CONFIG_READ, buffer);
#endif
		}
    }
}

int	eventlog_init(void)
{
	int	i = 0;
	if (osvi.dwMajorVersion < EVT_OS_VERSION)
	{
		HKEY    hReg;
		int32   rc;
		eventlog_data* pTmp	;
		char    buffer[REG_BUFFER_LEN];

	    if( (rc = RegOpenKeyEx(HKEY_LOCAL_MACHINE,
		"SYSTEM\\CurrentControlSet\\Services\\Eventlog", 0, KEY_READ, &hReg)) == ERROR_SUCCESS )
	    {
		while( RegEnumKey(hReg, i++, buffer, sizeof(buffer)) == ERROR_SUCCESS )
		{
		    eventlog_data data;

		    ZeroMemory(&data, sizeof(data)) ;
		    __strncpy0(data.name, buffer, MAX_LOG_NAME_LEN);
		    eventlog_read_reg(&data);

		    if( (pTmp = (eventlog_data*)realloc(
			eventlog_list, (eventlog_entries + 1) * sizeof(data))) == NULL )
			ntsl_die(NTSL_ERROR_CONFIG_MALLOC, NULL);
		    (eventlog_list = pTmp)[eventlog_entries++] = data;
		}
		RegCloseKey(hReg);
	    }
	}
	// Windows Eventing 6.0対応
	else
	{
		eventlog_data* pTmp	;
	    eventlog_data data;

		EVT_HANDLE hReadEvent = NULL;
		DWORD dwBufferSize = 0;
		DWORD dwBufferUsed = 0;
		DWORD status = ERROR_SUCCESS;
	    LPWSTR pBuffer = NULL;

		EVT_HANDLE hChannels = EvtOpenChannelEnum(NULL, 0);

	    if (NULL == hChannels)
	    {
#ifdef _DEBUG
			printf ("EvtOpenChannelEnum failed with %lu.\n", GetLastError());
#endif
	    }

		while (true)
		{
			if (!EvtNextChannelPath(hChannels, dwBufferSize, pBuffer, &dwBufferUsed))
			{
				status = GetLastError();

				if (ERROR_NO_MORE_ITEMS == status)
				{
					break;
				}
				else if (ERROR_INSUFFICIENT_BUFFER == status)
				{
					LPWSTR pTemp = (LPWSTR)realloc(pBuffer, dwBufferUsed * sizeof(WCHAR));
					dwBufferSize = dwBufferUsed;
					if (pTemp)
					{
						pBuffer = pTemp;
						pTemp = NULL;
						EvtNextChannelPath(hChannels, dwBufferSize, pBuffer, &dwBufferUsed);
					}
					else
					{
#ifdef _DEBUG
						printf ("realloc failed\n");
#endif
						status = ERROR_OUTOFMEMORY;
						break;
					}
				}
				else
				{
#ifdef _DEBUG
					printf ("EvtNextChannelPath failed with %lu.\n", status);
#endif
				}
			}

			ZeroMemory(&data, sizeof(data)) ;
			WideCharToMultiByte(CP_THREAD_ACP,0,pBuffer, -1, data.name, MAX_LOG_NAME_LEN, NULL, NULL);
			eventlog_read_reg(&data);

			if( (pTmp = (eventlog_data*)realloc(eventlog_list, (eventlog_entries + 1) * sizeof(data))) == NULL )
				ntsl_die(NTSL_ERROR_CONFIG_MALLOC, NULL);
			(eventlog_list = pTmp)[eventlog_entries++] = data;

			i ++;
		}
	    if (hChannels) EvtClose(hChannels);
	    if (pBuffer) free(pBuffer);
	}
    return (i);
}

void	eventlog_shutdown(void)
{
    eventlog_entries = 0u;
    free( eventlog_list );			// free list contents
    eventlog_list = NULL ;
}	

/*--------------------------[ eventlog_parse_libs ]--------------------------
 * Parse different libraries from string 
 *
 *	Returns:
 *		success		(0)
 *		error		(-1)	
 *----------------------------------------------------------------------------*/
int eventlog_parse_libs(char *dllIn, char *dllOut, uint32 sequence)
{
    int start = 0;
    int target = 0;
	
    while( sequence > 0 && dllIn[start] != '\0' )
    {
	if( dllIn[start] == ';' )
		sequence--;
	start++;
    }

    if (dllIn[start] == '\0')
	return (-1);

    while( dllIn[start] != '\0' && dllIn[start] != ';' )
    {
	dllOut[target] = dllIn[start];
	target++;
	start++;
    }
    dllOut[target] = '\0';
    return (0);
}

/*----------------------------[ eventlog_set_user ]---------------------------
 * Obtains the user name associated with a event 
 *
 *	Returns:
 *		success		(0)
 *		error		(-1)	
 *----------------------------------------------------------------------------*/
static int eventlog_set_user(ntsl_event *pEvent, PSID sid, DWORD len)
{
    char    domain[NTSL_SYS_LEN];
    DWORD   ds =  sizeof(domain) - 1;
    char    user[NTSL_SYS_LEN];
    DWORD   us =  sizeof(user) - 1;
    SID_NAME_USE pe;

    if (pEvent == NULL)
	return (-1);

    domain[0] = '\0';
    user[0] = '\0';
        pEvent->szUser[0] = '\0';
    if( (len > 0) && LookupAccountSid(NULL, sid, user, &us, domain, &ds, &pe) )
    {
	if( user[0] != '\0' )
	{
	       _snprintf(pEvent->szUser, sizeof(pEvent->szUser), "%s%s%s",
		domain,
		( (domain[0] == '\0')? "" : "\\" ),
                user);
	       pEvent->szUser[sizeof(pEvent->szUser)-1] = '\0';
	}
        return (0);
    }
// Failure: Couldn't evaluate Credentials
    return (-1);
}
