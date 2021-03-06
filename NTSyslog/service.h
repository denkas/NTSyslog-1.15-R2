/************************************************************************/
///@brief	Common definitions for Win32-Services (Startup-Code)
///@file
///@author	Ing. Markus Eisenmann (FRZEM)				
///@date	Created 2007-10-21 14:10
/// \n		Changed 2007-10-21 14:10
/************************************************************************/

#if !defined(_INC_WINDOWS) && !defined(_WINBASE_)
#define	STRICT	1			/* exakte Syntaxpr・ung */
#include <windows.h>
#endif

#ifndef	RC_INVOKED
#include <stdlib.h>
#include <tchar.h>

/************************************************************************/
/** Revision:	1.0	$						*/
/**	2007-10-21:	Initial Release					*/
/**									*/
/************************************************************************/

#ifndef _WIN32_WINNT
#error	"Must be a Windows-NT Project !"
#endif

#if !(defined(_MT) || defined(__MT__))
#error	"Must be a Multithread-Project!"
#endif

#ifdef	__cplusplus
extern "C" {
#endif

#ifndef __BLDSRVRCODE
extern	TCHAR*	_pszProgPath;
extern	TCHAR*	_pszLogFile ;
extern	TCHAR*	_pszSrvrName;
extern	BOOL	_bConsole;
extern	BOOL	_bDbgMode;

extern	CRITICAL_SECTION	_sSrvrCriSec;
extern	SERVICE_STATUS		_sSrvrStatus;
extern	SERVICE_STATUS_HANDLE	_hSrvrStatusHandle;
extern	volatile HANDLE		_hSrvrStopEvent;
#endif

/*
//  Standardparameter CreateService(...)
*/

#ifndef	CREATE_SERVICE_lpServiceName
#define	CREATE_SERVICE_lpServiceName	_pszSrvrName
#endif
#ifndef	CREATE_SERVICE_lpDisplayName
#define	CREATE_SERVICE_lpDisplayName	CREATE_SERVICE_lpServiceName
#endif
#ifndef	CREATE_SERVICE_dwDesiredAccess
#define	CREATE_SERVICE_dwDesiredAccess	SERVICE_ALL_ACCESS
#endif
#ifndef	CREATE_SERVICE_dwServiceType
#define	CREATE_SERVICE_dwServiceType	SERVICE_WIN32_OWN_PROCESS
#endif
#ifndef	CREATE_SERVICE_dwStartType 
#define	CREATE_SERVICE_dwStartType	SERVICE_AUTO_START
#endif
#ifndef	CREATE_SERVICE_dwErrorControl 
#define	CREATE_SERVICE_dwErrorControl	SERVICE_ERROR_NORMAL
#endif
#ifndef	CREATE_SERVICE_lpLoadOrderGroup
#define	CREATE_SERVICE_lpLoadOrderGroup	NULL
#endif
#ifndef	CREATE_SERVICE_lpdwTagId
#define	CREATE_SERVICE_lpdwTagId	NULL
#endif
#ifndef	CREATE_SERVICE_lpDependencies
#define	CREATE_SERVICE_lpDependencies	TEXT("\0")
#endif

#ifndef	CREATE_SERVICE_lpServiceAccount
#ifdef	CREATE_SERVICE_lpStartPassword
#undef	CREATE_SERVICE_lpStartPassword	// wird vorgegeben ...
#endif
//#if !defined(_WIN32_WINNT) || (_WIN32_WINNT <= 0x0500)
#define	CREATE_SERVICE_lpServiceAccount	NULL
#define	CREATE_SERVICE_lpStartPassword	NULL
//#else
//#define	CREATE_SERVICE_lpServiceAccount	TEXT("NT AUTHORITY\\NetworkService")
//#define	CREATE_SERVICE_lpStartPassword	TEXT("")
//#endif	/* _WIN32_WINNT */
#else
#ifndef	CREATE_SERVICE_lpStartPassword
#define	CREATE_SERVICE_lpStartPassword	TEXT("")
#endif
#endif

/************************************************************************/

#define	_APIENTRY   __stdcall
#define	_VARENTRY   __cdecl

void	_APIENTRY _vLogEventEx(WORD etype, DWORD dwId, unsigned nParams, va_list* args);
void	_VARENTRY _LogEventEx2(WORD etype, DWORD dwId, unsigned nParams, ... );
void	_VARENTRY LogMessage(TCHAR chTyp, DWORD dwId,
				const TCHAR *szFile, int iLine, const TCHAR *szFunc,
				const TCHAR *format, unsigned nParams, ... );

TCHAR*	_APIENTRY _GetErrorText(TCHAR* pszPuf, DWORD dwSize, DWORD dwErrCode);
TCHAR*	_APIENTRY GetLastErrorText(TCHAR* pszPuf, DWORD dwSize);

DWORD	_APIENTRY _LogWinError(TCHAR chTyp, DWORD dwId, const TCHAR* pszTxt,
				DWORD dwErrCode, const TCHAR *szFile, int iLine, const TCHAR *szFunc);
#define	__Win32RcLog(id, txt, err)  \
	_LogWinError(_T(-1), (id), (txt), (err), __FILE__, __LINE__, __FUNCTION__)
#define	__Win32Log(id, txt) __Win32RcLog((id), (txt), GetLastError())

BOOL	WINAPI	SrvrStatus(DWORD dwStatus, DWORD dwExitRC, DWORD dwWait);
INT	WINAPI	SrvrIsHalting(DWORD dwTim);
int	WINAPI	SrvrInit(int argc, TCHAR *argv[]);
int	WINAPI	SrvrRun(int argc, TCHAR *argv[]);
int	WINAPI	SrvrStop(DWORD dwCtrl);

/************************************************************************/
#ifdef	__cplusplus
	}
#endif
#endif	/* RC_INVOKED	*/
