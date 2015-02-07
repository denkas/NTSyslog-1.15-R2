SeverityNames=(Success=0x0:STATUS_SEVERITY_SUCCESS
               Informational=0x1:STATUS_SEVERITY_INFORMATIONAL
               Warning=0x2:STATUS_SEVERITY_WARNING
               Error=0x3:STATUS_SEVERITY_ERROR
              )

FacilityNames=(System=0x0:FACILITY_SYSTEM
               Runtime=0x2:FACILITY_RUNTIME
               Stubs=0x3:FACILITY_STUBS
               Io=0x4:FACILITY_IO_ERROR_CODE
              )

MessageIdTypedef=DWORD

LanguageNames=(German=0x407:MSG00407)
LanguageNames=(English=0x409:MSG00409)

;//	Service-Informationen
MessageId=0x01
Severity=Informational
Facility=Runtime
SymbolicName=MSGINF_SRVR_STARTING
Language=English
Start of Service '%1' in progress
.
Language=German
Service '%1' wird gestartet
.

MessageId=+1
SymbolicName=MSGINF_SRVR_STARTED
Language=English
Service '%1' started
.
Language=German
Service '%1' wurde gestartet
.

MessageId=+1
SymbolicName=MSGINF_SRVR_STOPPING
Language=English
Shutdown of Service '%1' in progress
.
Language=German
Service '%1' wird beendet
.

MessageId=+1
SymbolicName=MSGINF_SRVR_STOPPED
Language=English
Service '%1' stopped
.
Language=German
Service '%1' wurde beendet
.

;//	Service-Fehlermeldungen
MessageId=0x06
SymbolicName=MSGERR_SYSERROR6
Severity=Error
Facility=System
Language=English
%1 %n
Error: %2
.
Language=German
%1 %n
Fehler: %2
.

MessageId=0x07
SymbolicName=MSGERR_RTLERROR7
Severity=Error
Facility=Runtime
Language=English
%1 %n
Error: %2
.
Language=German
%1 %n
Fehler: %2
.

;//	Default-Message ...
MessageId=0x00
Severity=Informational
Facility=System
SymbolicName=MSGINF_DEFAULT
Language=English
%1
.
Language=German
%1
.
