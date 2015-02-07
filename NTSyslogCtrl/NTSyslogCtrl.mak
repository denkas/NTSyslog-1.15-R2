# Microsoft Developer Studio Generated NMAKE File, Based on NTSyslogCtrl.dsp
!IF "$(CFG)" == ""
CFG=NTSyslogCtrl - Win32 Debug
!MESSAGE No configuration specified. Defaulting to NTSyslogCtrl - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "NTSyslogCtrl - Win32 Release" && "$(CFG)" != "NTSyslogCtrl - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "NTSyslogCtrl.mak" CFG="NTSyslogCtrl - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "NTSyslogCtrl - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "NTSyslogCtrl - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "NTSyslogCtrl - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\Release
# End Custom Macros

ALL : "$(OUTDIR)\NTSyslogCtrl.exe"


CLEAN :
	-@erase "$(INTDIR)\ApplicationEventLogDlg.obj"
	-@erase "$(INTDIR)\NTService.obj"
	-@erase "$(INTDIR)\NTSyslogCtrl.obj"
	-@erase "$(INTDIR)\NTSyslogCtrl.pch"
	-@erase "$(INTDIR)\NTSyslogCtrl.res"
	-@erase "$(INTDIR)\NTSyslogCtrlDlg.obj"
	-@erase "$(INTDIR)\SecurityEventLogDlg.obj"
	-@erase "$(INTDIR)\SelectServerDlg.obj"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\SyslogDaemonDlg.obj"
	-@erase "$(INTDIR)\SystemEventLogDlg.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\NTSyslogCtrl.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)\NTSyslogCtrl.pch" /Yu"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x40c /fo"$(INTDIR)\NTSyslogCtrl.res" /d "NDEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\NTSyslogCtrl.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=/nologo /subsystem:windows /incremental:no /pdb:"$(OUTDIR)\NTSyslogCtrl.pdb" /machine:I386 /out:"$(OUTDIR)\NTSyslogCtrl.exe" 
LINK32_OBJS= \
	"$(INTDIR)\ApplicationEventLogDlg.obj" \
	"$(INTDIR)\NTService.obj" \
	"$(INTDIR)\NTSyslogCtrl.obj" \
	"$(INTDIR)\NTSyslogCtrlDlg.obj" \
	"$(INTDIR)\SecurityEventLogDlg.obj" \
	"$(INTDIR)\StdAfx.obj" \
	"$(INTDIR)\SyslogDaemonDlg.obj" \
	"$(INTDIR)\SystemEventLogDlg.obj" \
	"$(INTDIR)\NTSyslogCtrl.res" \
	"$(INTDIR)\SelectServerDlg.obj"

"$(OUTDIR)\NTSyslogCtrl.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "NTSyslogCtrl - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

ALL : "$(OUTDIR)\NTSyslogCtrl.exe" "$(OUTDIR)\NTSyslogCtrl.bsc"


CLEAN :
	-@erase "$(INTDIR)\ApplicationEventLogDlg.obj"
	-@erase "$(INTDIR)\ApplicationEventLogDlg.sbr"
	-@erase "$(INTDIR)\NTService.obj"
	-@erase "$(INTDIR)\NTService.sbr"
	-@erase "$(INTDIR)\NTSyslogCtrl.obj"
	-@erase "$(INTDIR)\NTSyslogCtrl.pch"
	-@erase "$(INTDIR)\NTSyslogCtrl.res"
	-@erase "$(INTDIR)\NTSyslogCtrl.sbr"
	-@erase "$(INTDIR)\NTSyslogCtrlDlg.obj"
	-@erase "$(INTDIR)\NTSyslogCtrlDlg.sbr"
	-@erase "$(INTDIR)\SecurityEventLogDlg.obj"
	-@erase "$(INTDIR)\SecurityEventLogDlg.sbr"
	-@erase "$(INTDIR)\SelectServerDlg.obj"
	-@erase "$(INTDIR)\SelectServerDlg.sbr"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\StdAfx.sbr"
	-@erase "$(INTDIR)\SyslogDaemonDlg.obj"
	-@erase "$(INTDIR)\SyslogDaemonDlg.sbr"
	-@erase "$(INTDIR)\SystemEventLogDlg.obj"
	-@erase "$(INTDIR)\SystemEventLogDlg.sbr"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\NTSyslogCtrl.bsc"
	-@erase "$(OUTDIR)\NTSyslogCtrl.exe"
	-@erase "$(OUTDIR)\NTSyslogCtrl.ilk"
	-@erase "$(OUTDIR)\NTSyslogCtrl.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\NTSyslogCtrl.pch" /Yu"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x40c /fo"$(INTDIR)\NTSyslogCtrl.res" /d "_DEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\NTSyslogCtrl.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\ApplicationEventLogDlg.sbr" \
	"$(INTDIR)\NTService.sbr" \
	"$(INTDIR)\NTSyslogCtrl.sbr" \
	"$(INTDIR)\NTSyslogCtrlDlg.sbr" \
	"$(INTDIR)\SecurityEventLogDlg.sbr" \
	"$(INTDIR)\StdAfx.sbr" \
	"$(INTDIR)\SyslogDaemonDlg.sbr" \
	"$(INTDIR)\SystemEventLogDlg.sbr" \
	"$(INTDIR)\SelectServerDlg.sbr"

"$(OUTDIR)\NTSyslogCtrl.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
LINK32_FLAGS=/nologo /subsystem:windows /incremental:yes /pdb:"$(OUTDIR)\NTSyslogCtrl.pdb" /debug /machine:I386 /out:"$(OUTDIR)\NTSyslogCtrl.exe" /pdbtype:sept 
LINK32_OBJS= \
	"$(INTDIR)\ApplicationEventLogDlg.obj" \
	"$(INTDIR)\NTService.obj" \
	"$(INTDIR)\NTSyslogCtrl.obj" \
	"$(INTDIR)\NTSyslogCtrlDlg.obj" \
	"$(INTDIR)\SecurityEventLogDlg.obj" \
	"$(INTDIR)\StdAfx.obj" \
	"$(INTDIR)\SyslogDaemonDlg.obj" \
	"$(INTDIR)\SystemEventLogDlg.obj" \
	"$(INTDIR)\NTSyslogCtrl.res" \
	"$(INTDIR)\SelectServerDlg.obj"

"$(OUTDIR)\NTSyslogCtrl.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 

.c{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("NTSyslogCtrl.dep")
!INCLUDE "NTSyslogCtrl.dep"
!ELSE 
!MESSAGE Warning: cannot find "NTSyslogCtrl.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "NTSyslogCtrl - Win32 Release" || "$(CFG)" == "NTSyslogCtrl - Win32 Debug"
SOURCE=.\ApplicationEventLogDlg.cpp

!IF  "$(CFG)" == "NTSyslogCtrl - Win32 Release"


"$(INTDIR)\ApplicationEventLogDlg.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\NTSyslogCtrl.pch"


!ELSEIF  "$(CFG)" == "NTSyslogCtrl - Win32 Debug"


"$(INTDIR)\ApplicationEventLogDlg.obj"	"$(INTDIR)\ApplicationEventLogDlg.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\NTSyslogCtrl.pch"


!ENDIF 

SOURCE=.\NTService.cpp

!IF  "$(CFG)" == "NTSyslogCtrl - Win32 Release"


"$(INTDIR)\NTService.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\NTSyslogCtrl.pch"


!ELSEIF  "$(CFG)" == "NTSyslogCtrl - Win32 Debug"


"$(INTDIR)\NTService.obj"	"$(INTDIR)\NTService.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\NTSyslogCtrl.pch"


!ENDIF 

SOURCE=.\NTSyslogCtrl.cpp

!IF  "$(CFG)" == "NTSyslogCtrl - Win32 Release"


"$(INTDIR)\NTSyslogCtrl.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\NTSyslogCtrl.pch"


!ELSEIF  "$(CFG)" == "NTSyslogCtrl - Win32 Debug"


"$(INTDIR)\NTSyslogCtrl.obj"	"$(INTDIR)\NTSyslogCtrl.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\NTSyslogCtrl.pch"


!ENDIF 

SOURCE=.\NTSyslogCtrl.rc

"$(INTDIR)\NTSyslogCtrl.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)


SOURCE=.\NTSyslogCtrlDlg.cpp

!IF  "$(CFG)" == "NTSyslogCtrl - Win32 Release"


"$(INTDIR)\NTSyslogCtrlDlg.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\NTSyslogCtrl.pch"


!ELSEIF  "$(CFG)" == "NTSyslogCtrl - Win32 Debug"


"$(INTDIR)\NTSyslogCtrlDlg.obj"	"$(INTDIR)\NTSyslogCtrlDlg.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\NTSyslogCtrl.pch"


!ENDIF 

SOURCE=.\SecurityEventLogDlg.cpp

!IF  "$(CFG)" == "NTSyslogCtrl - Win32 Release"


"$(INTDIR)\SecurityEventLogDlg.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\NTSyslogCtrl.pch"


!ELSEIF  "$(CFG)" == "NTSyslogCtrl - Win32 Debug"


"$(INTDIR)\SecurityEventLogDlg.obj"	"$(INTDIR)\SecurityEventLogDlg.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\NTSyslogCtrl.pch"


!ENDIF 

SOURCE=.\SelectServerDlg.cpp

!IF  "$(CFG)" == "NTSyslogCtrl - Win32 Release"


"$(INTDIR)\SelectServerDlg.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\NTSyslogCtrl.pch"


!ELSEIF  "$(CFG)" == "NTSyslogCtrl - Win32 Debug"


"$(INTDIR)\SelectServerDlg.obj"	"$(INTDIR)\SelectServerDlg.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\NTSyslogCtrl.pch"


!ENDIF 

SOURCE=.\StdAfx.cpp

!IF  "$(CFG)" == "NTSyslogCtrl - Win32 Release"

CPP_SWITCHES=/nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)\NTSyslogCtrl.pch" /Yc"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\StdAfx.obj"	"$(INTDIR)\NTSyslogCtrl.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "NTSyslogCtrl - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\NTSyslogCtrl.pch" /Yc"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\StdAfx.obj"	"$(INTDIR)\StdAfx.sbr"	"$(INTDIR)\NTSyslogCtrl.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\SyslogDaemonDlg.cpp

!IF  "$(CFG)" == "NTSyslogCtrl - Win32 Release"


"$(INTDIR)\SyslogDaemonDlg.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\NTSyslogCtrl.pch"


!ELSEIF  "$(CFG)" == "NTSyslogCtrl - Win32 Debug"


"$(INTDIR)\SyslogDaemonDlg.obj"	"$(INTDIR)\SyslogDaemonDlg.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\NTSyslogCtrl.pch"


!ENDIF 

SOURCE=.\SystemEventLogDlg.cpp

!IF  "$(CFG)" == "NTSyslogCtrl - Win32 Release"


"$(INTDIR)\SystemEventLogDlg.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\NTSyslogCtrl.pch"


!ELSEIF  "$(CFG)" == "NTSyslogCtrl - Win32 Debug"


"$(INTDIR)\SystemEventLogDlg.obj"	"$(INTDIR)\SystemEventLogDlg.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\NTSyslogCtrl.pch"


!ENDIF 


!ENDIF 

