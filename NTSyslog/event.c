/*-----------------------------------------------------------------------------
 *
 *  event.c - Event module
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
 *  $Id: event.c,v 1.3 2007/10/25 18:20:59 frzem Exp $
 *
 *  Revision history:
 *    01-Dec-1999  JRR  Removed superfluous gethostbyaddr
 *    28-Sep-1999  JRR  Added support for secondary syslog host
 *    18-Aug-1998  JRR  Module completed
 *
 *    21-Oct-2007 EM
 *	- Changed registry key for evaluate the IP-address of syslog-server(s)
 *		Instead of HKLM\SOFTWARE\Sabernet\Syslog or HKLM\SOFTWARE\Sabernet\Syslog1 (REG_SZ)
 *		HKLM\SOFTWARE\Sabernet\Syslog\Server and HKLM\SOFTWARE\Sabernet\Syslog\Backup are used.
 *		If no registry key (of them) exists, the default value "syslog-server" is used.
 *	- Enhanced Message-Format. The message starts with NT-information:
 *		[NT:Eventlog-source;CNumber;User-Account] Messag-text
 *		Eventlog-source: for example security or NtFrs
 *		CNumber: Code (E=Error, W=Warning, F=Failure, S=Success, I=Information) + Event-ID
 *		User-Account: Credentials of the initiating user (maybe Builtin\System)
 *
 *----------------------------------------------------------------------------*/
#include <stdio.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include "eventlog.h"
#include "convert.h"

/*-------------------------------[ static data ]------------------------------*/
#define SYSLOG_NAME	"syslog"
#define SYSLOG_PORT	 514
#define REG_BUFFER_LEN	1024
#define	SYSLOG_SRV_KEY0	"Syslog"
#define	SYSLOG_SRV_KEY1	"Syslog1"

//#define _USE_SJIS	// SJISモードにする場合コメントを外す
#define _USE_UTF8	// UTF8モードにする場合コメントを外す

static const char *syslog_host(char *host, int len, const char *regkey);
static int _iWSAStartup = SOCKET_ERROR;

extern OSVERSIONINFO osvi;

/*------------------------------[ event_output ]------------------------------
 *  Output the event
 *
 *  Parameters:
 *	event	    event to format
 *	fp	    file pointer
 *
 *  Return value:
 *	success	    (0)
 *	failure	    (-1)
 *
 *----------------------------------------------------------------------------*/
static const char *syslog_host(char *host, int len, const char *regkey)
{
    HKEY hReg;
    int32 rc, rv;

    if( host != NULL )
    {
		rc = RegOpenKeyEx(HKEY_LOCAL_MACHINE, NTSYSLOG_SOFTWARE_KEY, 0, KEY_READ, &hReg);
		if( rc == ERROR_SUCCESS )
		{
		    rv = RegQueryValueEx(hReg, regkey, NULL, NULL, (BYTE*)host, &len);
		    RegCloseKey(hReg);
		    if( rv == ERROR_SUCCESS )
			return (host);
		}
		memset((BYTE*)host, 0x00, (size_t)len);
	}
    return (const char*)NULL;
}

static int _event_output(ntsl_event *pEvent, const char *server_name)
{
    char buffer[NTSL_EVENT_FORMAT_LEN];
    int port = htons(SYSLOG_PORT);
    SOCKET sock;
    size_t len;
    char *pszMsgText;

	/* IPv6対応 */
	ADDRINFO hints;
    LPADDRINFO ai;
	char dst_port[16];
	int e;

	/* 日本語対応用 */
	char				 euc_buffer[NTSL_EVENT_FORMAT_LEN*3];
	int					 euc_point = 0;
	int					 i;
	int					 ret,utf_ret;
	unsigned int		 convtmp;
	char				 hanzenbuf[4];
	extern int           iCharCode;

//#ifdef _USE_UTF8
#if 1
	char				 tmp_buffer[8];
	char				 uni_buffer[4];
	char				 utf_buffer[8];
#endif

    if ( (pEvent == NULL) || (server_name == NULL) || (server_name[0] == 0) )
	return (-1);

    // Compile the syslog message; truncates pEvent->msg as needed
    pszMsgText = pEvent->msg;
    while( (*pszMsgText == ':') || (*pszMsgText == ' ') )
	++pszMsgText;
    _snprintf(buffer, sizeof(buffer),
	"<%d>%s %s NT: <%s;%c%u;%s> %s",
	pEvent->priority,
	pEvent->date,
	AnsiUpper( pEvent->host ),
	pEvent->szSource,
	((pEvent->eType == NULL)? ' ' : pEvent->eType[0]),
	(unsigned)pEvent->dwCode,
	pEvent->szUser,
	pszMsgText);
    buffer[sizeof(buffer) - 1] = '\0';
    len = strlen(buffer);

// 日本語変換ルーチン -- ここから --
	memset(euc_buffer,0,NTSL_EVENT_FORMAT_LEN*3);
	for (i=0;(unsigned int)i<len;i++)
	{
		switch (whatKanji((unsigned char *)(buffer+i)) & 3)
		{
			case 0:
#ifdef _DEBUG0
	printf( "ASCII:0x%x\n",*(buffer+i) );
#endif
				if (buffer[i] < 0x20)
					euc_buffer[euc_point] = 0x20;
				else
					euc_buffer[euc_point] = buffer[i];
				euc_point ++;
				break;

			case 1:
				memset(hanzenbuf, 0, 4);
				hanzenbuf[0] = *(buffer+i);
				if ((unsigned int)i+1 < len)
					hanzenbuf[1] = *(buffer+i+1);
				else
					hanzenbuf[1] = 0;
#ifdef _DEBUG0
	printf( "KANA1:0x%x\n",(LOBYTE(*(WORD*)(buffer+i))*0x100)+HIBYTE(*(WORD*)(buffer+i)) );
	printf( "KANA2:0x%x\n",(LOBYTE(*(WORD*)(hanzenbuf))*0x100)+HIBYTE(*(WORD*)(hanzenbuf)) );
#endif
				ret = han2zen( &convtmp ,(unsigned char *)hanzenbuf);

//#ifndef _USE_UTF8
				if (iCharCode != 0)
				{
			// for EUC and SJIS
//#ifndef _USE_SJIS
			// for EUC
					if (iCharCode == 1) convtmp = convtmp | 0x8080;
//#else
			// for SJIS
					if (iCharCode == 2) convtmp = jis2sjis(convtmp);
//#endif

					if (ret == 2)
					{
						euc_buffer[euc_point  ] = HIBYTE(convtmp);
						euc_buffer[euc_point+1] = LOBYTE(convtmp);
						euc_point += 2;
						i ++;
					}
					else
					{
						euc_buffer[euc_point  ] = HIBYTE(convtmp);
						euc_buffer[euc_point+1] = LOBYTE(convtmp);
						euc_point += 2;
					}
				}
//#else
			// for UTF8
				else
				{
					memset(tmp_buffer,0,8);
					memset(uni_buffer,0,4);
					memset(utf_buffer,0,8);

					convtmp = jis2sjis(convtmp);
					tmp_buffer[0] = HIBYTE(convtmp);
					tmp_buffer[1] = LOBYTE(convtmp);

					MultiByteToWideChar
						(CP_OEMCP, 0, tmp_buffer, 2, (LPWSTR)uni_buffer, 4);
					utf_ret = WideCharToMultiByte
						(CP_UTF8, 0, (LPWSTR)uni_buffer, (int)wcslen((LPWSTR)uni_buffer), utf_buffer, 8, NULL, NULL);

					euc_buffer[euc_point  ] = utf_buffer[0];
					euc_buffer[euc_point+1] = utf_buffer[1];
					if (utf_ret == 3) euc_buffer[euc_point+2] = utf_buffer[2];
#ifdef _DEBUG0
	 	printf( "ASCII:0x%02x%02x%02x\n"
			,*((unsigned char*)(euc_buffer+euc_point  ))
			,*((unsigned char*)(euc_buffer+euc_point+1))
			,*((unsigned char*)(euc_buffer+euc_point+2)) );
#endif
					if (ret == 2) i ++;
					euc_point += utf_ret;
				}
//#endif


#ifdef _DEBUG0
	printf( "Code:0x%x\n",convtmp);
#endif
				break;
			case 2:
#ifdef _DEBUG0
	printf( "KANJI:0x%x\n",(LOBYTE(*(WORD*)(buffer+i))*0x100)+HIBYTE(*(WORD*)(buffer+i)) );
	printf("to--> 0x%x\n",
		sjis2euc( (LOBYTE(*(WORD*)(buffer+i))*0x100)+HIBYTE(*(WORD*)(buffer+i))  )	);
#endif

//#ifndef _USE_UTF8
			// for EUC and SJIS
				if (iCharCode != 0)
				{
//#ifndef _USE_SJIS
			// for EUC
					if (iCharCode == 1)
					{
							euc_buffer[euc_point  ] = (char)
							(
								HIBYTE(sjis2euc( (LOBYTE(*(WORD*)(buffer+i))*0x100)+HIBYTE(*(WORD*)(buffer+i)) ))
							);
						euc_buffer[euc_point+1] = (char)
							(
								LOBYTE(sjis2euc( (LOBYTE(*(WORD*)(buffer+i))*0x100)+HIBYTE(*(WORD*)(buffer+i)) ))
							);
					}
//#else
			// for SJIS
					else if (iCharCode == 2)
					{
						euc_buffer[euc_point  ] = buffer[i  ];
						euc_buffer[euc_point+1] = buffer[i+1];
					}
//#endif
					euc_point += 2;
					i ++;
				}
//#else
			// for UTF8
				else
				{
					memset(tmp_buffer,0,8);
					memset(uni_buffer,0,4);
					memset(utf_buffer,0,8);

					tmp_buffer[0] = buffer[i  ];
					tmp_buffer[1] = buffer[i+1];

					MultiByteToWideChar
						(CP_OEMCP, 0, tmp_buffer, 2, (LPWSTR)uni_buffer, 4);
					utf_ret = WideCharToMultiByte
						(CP_UTF8, 0, (LPWSTR)uni_buffer, (int)wcslen((LPWSTR)uni_buffer), utf_buffer, 8, NULL, NULL);

					euc_buffer[euc_point  ] = utf_buffer[0];
					euc_buffer[euc_point+1] = utf_buffer[1];
					if (utf_ret == 3) euc_buffer[euc_point+2] = utf_buffer[2];
	#ifdef _DEBUG0
	 	printf( "ASCII:0x%02x%02x%02x\n"
			,*((unsigned char*)(euc_buffer+euc_point  ))
			,*((unsigned char*)(euc_buffer+euc_point+1))
			,*((unsigned char*)(euc_buffer+euc_point+2)) );
	#endif
					euc_point += utf_ret;
					i ++;
				}
//#endif
				break;
			default:
				break;
#ifdef _DEBUG0
	printf( "Default:0x%x\n",(LOBYTE(*(WORD*)(buffer+i))*0x100)+HIBYTE(*(WORD*)(buffer+i)) );
#endif
		}
	}
	len = strlen(euc_buffer);
	// 日本語変換ルーチン -- ここまで --

	if (osvi.dwMajorVersion < 5 || (osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 0))
	{
	    struct servent *serv;
	    struct sockaddr_in server;

		/* lookup service port from services file */
	    if( (serv = getservbyname(SYSLOG_NAME, "udp")) != NULL )
	    {
			port = serv->s_port;
		}

		if( isalpha(server_name[0]) ) 
		{   
			struct hostent *hp = gethostbyname(server_name);
			if( hp != NULL ) 
				memcpy(&(server.sin_addr),hp->h_addr,hp->h_length);
		}
	    else  
	    { 
			server.sin_addr.s_addr = inet_addr(server_name);
	    }

		if( server.sin_addr.s_addr == 0 )
		{
			return (-1);
		}

		/* setup sockaddr_in structure */
		server.sin_family = AF_INET;
		server.sin_port   = port;

		/* open socket */
		sock = socket(AF_INET, SOCK_DGRAM, 0);
		if( sock == INVALID_SOCKET )
		{
			ntsl_log_error(NTSL_ERROR_SOCKET_INIT, WSAGetLastError());
			return -1;
		}

		/* connect to syslog host */
		if( connect(sock, (struct sockaddr*)&server, sizeof(server)) == SOCKET_ERROR )
		{
			ntsl_log_error(NTSL_ERROR_SOCKET_SEND, WSAGetLastError());
			return -1;
		}

	    /* send syslog message */
	    if( send(sock, euc_buffer, (int)len, 0) == SOCKET_ERROR )
	    {
			ntsl_log_error(NTSL_ERROR_SOCKET_SEND, WSAGetLastError());
			return -1;
		}
	}
	else
	{
	    /* lookup service port from services file */
	    memset(&hints, 0, sizeof(hints));
	    hints.ai_family = AF_UNSPEC;
	    hints.ai_socktype = SOCK_DGRAM;
	    if (e = getaddrinfo(server_name, _itoa(port,dst_port,10), &hints, &ai))
		{
			return -1;
		}
	
		/* open socket */
		sock = socket(ai->ai_family, ai->ai_socktype, ai->ai_protocol);
		if( sock == INVALID_SOCKET )
		{
			ntsl_log_error(NTSL_ERROR_SOCKET_INIT, WSAGetLastError());
			return -1;
		}

		/* connect to syslog host */
		if( connect(sock, ai->ai_addr, (int)ai->ai_addrlen) == SOCKET_ERROR )
		{
			ntsl_log_error(NTSL_ERROR_SOCKET_SEND, WSAGetLastError());
			return -1;
		}

		/* send syslog message */
		if( send(sock, euc_buffer, (int)len, 0) == SOCKET_ERROR )
		{
			ntsl_log_error(NTSL_ERROR_SOCKET_SEND, WSAGetLastError());
			return -1;
		}
	}

#ifdef _DEBUG0
    printf("%d: \"%s\"\n\n", len, buffer);
#endif

    closesocket(sock);
    return (0);
}

int event_output(ntsl_event *pEvent)
{
    static char host[2][NTSL_SYS_LEN] = { "", "" };
    int rc[2] = {-1, -1};

    if( _iWSAStartup != 0 )
    {
		WSADATA wsaData;

		if( (_iWSAStartup = WSAStartup(0x202, &wsaData)) != 0 )
		{
		    ntsl_log_error(NTSL_ERROR_WSASTARTUP, _iWSAStartup);
			return (-1);
		}
    }

    if( host[0][0] == 0 )
    {
		syslog_host(host[0], NTSL_SYS_LEN, SYSLOG_SRV_KEY0);
		syslog_host(host[1], NTSL_SYS_LEN, SYSLOG_SRV_KEY1);

		if( host[1][0] == '\0' )
		{
		   if( host[0][0] == '\0' )	// part. DNS-Name
			strncpy(host[0], "syslog-server", sizeof(host[0]));
		}
    }

    rc[0] = _event_output(pEvent, host[0]);
    rc[1] = _event_output(pEvent, host[1]);

    _iWSAStartup = SOCKET_ERROR;	// WSock2 closed ...
    WSACleanup();

    return(rc[0] == 0 || rc[1] == 0 ? 0 : -1);
}
