# Microsoft Developer Studio Generated NMAKE File, Based on ntsyslog.dsp
!IF "$(CFG)" == ""
CFG=ntsyslog - Win32 Debug
!MESSAGE No configuration specified. Defaulting to ntsyslog - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "ntsyslog - Win32 Release" && "$(CFG)" != "ntsyslog - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "ntsyslog.mak" CFG="ntsyslog - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "ntsyslog - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "ntsyslog - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "ntsyslog - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\Release
# End Custom Macros

ALL : "$(OUTDIR)\ntsyslog.exe"


CLEAN :
	-@erase "$(INTDIR)\engine.obj"
	-@erase "$(INTDIR)\event.obj"
	-@erase "$(INTDIR)\eventlog.obj"
	-@erase "$(INTDIR)\list.obj"
	-@erase "$(INTDIR)\ntsl.obj"
	-@erase "$(INTDIR)\safestr.obj"
	-@erase "$(INTDIR)\service.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\ntsyslog.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /ML /W3 /GX /O1 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /D "DEBUG" /Fp"$(INTDIR)\ntsyslog.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\ntsyslog.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib ws2_32.lib libcmt.lib /nologo /subsystem:console /incremental:no /pdb:"$(OUTDIR)\ntsyslog.pdb" /machine:I386 /nodefaultlib:"LIBC" /out:"$(OUTDIR)\ntsyslog.exe" 
LINK32_OBJS= \
	"$(INTDIR)\engine.obj" \
	"$(INTDIR)\event.obj" \
	"$(INTDIR)\eventlog.obj" \
	"$(INTDIR)\list.obj" \
	"$(INTDIR)\ntsl.obj" \
	"$(INTDIR)\safestr.obj" \
	"$(INTDIR)\service.obj"

"$(OUTDIR)\ntsyslog.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "ntsyslog - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

ALL : "$(OUTDIR)\ntsyslog.exe"


CLEAN :
	-@erase "$(INTDIR)\engine.obj"
	-@erase "$(INTDIR)\event.obj"
	-@erase "$(INTDIR)\eventlog.obj"
	-@erase "$(INTDIR)\list.obj"
	-@erase "$(INTDIR)\ntsl.obj"
	-@erase "$(INTDIR)\safestr.obj"
	-@erase "$(INTDIR)\service.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\ntsyslog.exe"
	-@erase "$(OUTDIR)\ntsyslog.ilk"
	-@erase "$(OUTDIR)\ntsyslog.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MT /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /D "DEBUG" /Fp"$(INTDIR)\ntsyslog.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\ntsyslog.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib ws2_32.lib libcmtd.lib /nologo /subsystem:console /incremental:yes /pdb:"$(OUTDIR)\ntsyslog.pdb" /debug /machine:I386 /nodefaultlib:"libcmt" /out:"$(OUTDIR)\ntsyslog.exe" /pdbtype:sept 
LINK32_OBJS= \
	"$(INTDIR)\engine.obj" \
	"$(INTDIR)\event.obj" \
	"$(INTDIR)\eventlog.obj" \
	"$(INTDIR)\list.obj" \
	"$(INTDIR)\ntsl.obj" \
	"$(INTDIR)\safestr.obj" \
	"$(INTDIR)\service.obj"

"$(OUTDIR)\ntsyslog.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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
!IF EXISTS("ntsyslog.dep")
!INCLUDE "ntsyslog.dep"
!ELSE 
!MESSAGE Warning: cannot find "ntsyslog.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "ntsyslog - Win32 Release" || "$(CFG)" == "ntsyslog - Win32 Debug"
SOURCE=.\engine.c

"$(INTDIR)\engine.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\event.c

"$(INTDIR)\event.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\eventlog.c

"$(INTDIR)\eventlog.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\list.c

"$(INTDIR)\list.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\ntsl.c

"$(INTDIR)\ntsl.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\safestr.c

"$(INTDIR)\safestr.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\service.c

"$(INTDIR)\service.obj" : $(SOURCE) "$(INTDIR)"



!ENDIF 

