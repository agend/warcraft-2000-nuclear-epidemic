# Microsoft Developer Studio Project File - Name="ddexam" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 5.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=ddexam - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "w2000.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "w2000.mak" CFG="ddexam - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "ddexam - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "ddexam - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "ddexam - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o NUL /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o NUL /win32
# ADD BASE RSC /l 0x419 /d "NDEBUG"
# ADD RSC /l 0x419 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386

!ELSEIF  "$(CFG)" == "ddexam - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 1
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o NUL /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o NUL /win32
# ADD BASE RSC /l 0x419 /d "_DEBUG"
# ADD RSC /l 0x419 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# SUBTRACT BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept

!ENDIF 

# Begin Target

# Name "ddexam - Win32 Release"
# Name "ddexam - Win32 Debug"
# Begin Group "ўоб очi не мул€ло"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Ddini.h
# End Source File
# Begin Source File

SOURCE=.\Dplay.h
# End Source File
# Begin Source File

SOURCE=.\event.h
# End Source File
# Begin Source File

SOURCE=.\Lines.cpp
# End Source File
# Begin Source File

SOURCE=.\Lines.h
# End Source File
# Begin Source File

SOURCE=.\menu.cpp
# End Source File
# Begin Source File

SOURCE=.\menu.h
# End Source File
# Begin Source File

SOURCE=.\mgraph.h
# End Source File
# Begin Source File

SOURCE=.\mode.h
# End Source File
# Begin Source File

SOURCE=.\MoveToXY.cpp
# End Source File
# Begin Source File

SOURCE=.\Resfile.cpp
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# End Group
# Begin Group "Headers"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\AntiBug.h
# End Source File
# Begin Source File

SOURCE=.\Dpchat.h
# End Source File
# Begin Source File

SOURCE=.\Dplobby.h
# End Source File
# Begin Source File

SOURCE=.\fog.h
# End Source File
# Begin Source File

SOURCE=.\Multipl.h
# End Source File
# Begin Source File

SOURCE=.\Nucl.h
# End Source File
# Begin Source File

SOURCE=.\Resfile.h
# End Source File
# End Group
# Begin Group "datas"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Nres.dat
# End Source File
# Begin Source File

SOURCE=.\Orcs.nds
# End Source File
# Begin Source File

SOURCE=.\Orcs.scr
# End Source File
# Begin Source File

SOURCE=.\Peoples.nds
# End Source File
# Begin Source File

SOURCE=.\SoundList.txt
# End Source File
# Begin Source File

SOURCE=.\Weapon.nds
# End Source File
# End Group
# Begin Source File

SOURCE=.\3DSurf.cpp
# End Source File
# Begin Source File

SOURCE=.\3DSurf.h
# End Source File
# Begin Source File

SOURCE=.\AntiBug.cpp
# End Source File
# Begin Source File

SOURCE=.\Build.cpp
# End Source File
# Begin Source File

SOURCE=.\Cdirsnd.cpp
# End Source File
# Begin Source File

SOURCE=.\Cdirsnd.h
# End Source File
# Begin Source File

SOURCE=.\Crowd.cpp
# End Source File
# Begin Source File

SOURCE=.\Cwave.cpp
# End Source File
# Begin Source File

SOURCE=.\Cwave.h
# End Source File
# Begin Source File

SOURCE=.\Ddini.cpp
# End Source File
# Begin Source File

SOURCE="C:\Program Files\DevStudio\Vc\Lib\Ddraw.lib"
# End Source File
# Begin Source File

SOURCE=.\Default.ai
# End Source File
# Begin Source File

SOURCE=.\Development.txt
# End Source File
# Begin Source File

SOURCE=.\Dialogs.cpp
# End Source File
# Begin Source File

SOURCE=.\Dialogs.h
# End Source File
# Begin Source File

SOURCE=.\dplayx.lib
# End Source File
# Begin Source File

SOURCE=.\dsound.lib
# End Source File
# Begin Source File

SOURCE=.\Fastdraw.cpp
# End Source File
# Begin Source File

SOURCE=.\Fastdraw.h
# End Source File
# Begin Source File

SOURCE=.\Find.cpp
# End Source File
# Begin Source File

SOURCE=.\FlyObj.cpp
# End Source File
# Begin Source File

SOURCE=.\FlyObj.h
# End Source File
# Begin Source File

SOURCE=.\fog.cpp
# End Source File
# Begin Source File

SOURCE=.\Fonts.h
# End Source File
# Begin Source File

SOURCE=.\GameSound.cpp
# End Source File
# Begin Source File

SOURCE=.\GSound.h
# End Source File
# Begin Source File

SOURCE=.\GVClass.cpp
# End Source File
# Begin Source File

SOURCE=.\help.txt
# End Source File
# Begin Source File

SOURCE=.\Hint.cpp
# End Source File
# Begin Source File

SOURCE=.\Icon.ico
# End Source File
# Begin Source File

SOURCE=.\Icons.h
# End Source File
# Begin Source File

SOURCE=.\Info.dat
# End Source File
# Begin Source File

SOURCE=.\Interface.cpp
# End Source File
# Begin Source File

SOURCE=.\Legion.cpp
# End Source File
# Begin Source File

SOURCE=.\LoadSave.cpp
# End Source File
# Begin Source File

SOURCE=.\mapa.cpp
# End Source File
# Begin Source File

SOURCE=.\MapDiscr.h
# End Source File
# Begin Source File

SOURCE=.\MapEdit.cpp
# End Source File
# Begin Source File

SOURCE=.\MapEdit.h
# End Source File
# Begin Source File

SOURCE=.\Megapolis.cpp
# End Source File
# Begin Source File

SOURCE=.\Megapolis.h
# End Source File
# Begin Source File

SOURCE=.\mode.dat
# End Source File
# Begin Source File

SOURCE=.\mouse.h
# End Source File
# Begin Source File

SOURCE=.\Mouse_X.cpp
# End Source File
# Begin Source File

SOURCE=.\MousZone.cpp
# End Source File
# Begin Source File

SOURCE=.\MPlayer.cpp
# End Source File
# Begin Source File

SOURCE=.\Multi.cpp
# End Source File
# Begin Source File

SOURCE=.\Nation.cpp
# End Source File
# Begin Source File

SOURCE=.\Nature.cpp
# End Source File
# Begin Source File

SOURCE=.\Nature.h
# End Source File
# Begin Source File

SOURCE=.\Navorots.cpp
# End Source File
# Begin Source File

SOURCE=.\Nuclear.cpp
# End Source File
# Begin Source File

SOURCE=.\Select.cpp
# End Source File
# Begin Source File

SOURCE=.\SelProp.cpp
# End Source File
# Begin Source File

SOURCE=.\Strange.cpp
# End Source File
# Begin Source File

SOURCE=.\Strange.h
# End Source File
# Begin Source File

SOURCE=.\stRecog.cpp
# End Source File
# Begin Source File

SOURCE=.\tcomp.TXT
# End Source File
# Begin Source File

SOURCE=.\TopZones.cpp
# End Source File
# Begin Source File

SOURCE=.\TopZones.h
# End Source File
# Begin Source File

SOURCE=.\Transp.cpp
# End Source File
# Begin Source File

SOURCE=.\Upgrade.cpp
# End Source File
# Begin Source File

SOURCE=.\Upgrade.h
# End Source File
# Begin Source File

SOURCE=.\Video.cpp
# End Source File
# Begin Source File

SOURCE=.\Video.h
# End Source File
# Begin Source File

SOURCE=.\Wall.h
# End Source File
# Begin Source File

SOURCE=.\walls.cpp
# End Source File
# Begin Source File

SOURCE=.\walls.h
# End Source File
# Begin Source File

SOURCE=.\War2000.cpp
# End Source File
# Begin Source File

SOURCE=.\Water.cpp
# End Source File
# Begin Source File

SOURCE=.\Water_AI.cpp
# End Source File
# Begin Source File

SOURCE=.\Weapon.cpp
# End Source File
# Begin Source File

SOURCE=.\WeaponID.h
# End Source File
# Begin Source File

SOURCE="C:\Program Files\DevStudio\Vc\Lib\Winmm.lib"
# End Source File
# End Target
# End Project
