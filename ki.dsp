# Microsoft Developer Studio Project File - Name="ki" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=ki - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "ki.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "ki.mak" CFG="ki - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "ki - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "ki - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "ki - Win32 Release"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /G6 /MD /W3 /GX /O2 /Ob2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FAcs /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 /nologo /subsystem:windows /machine:I386

!ELSEIF  "$(CFG)" == "ki - Win32 Debug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FAcs /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept

!ENDIF 

# Begin Target

# Name "ki - Win32 Release"
# Name "ki - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\2100dasm.cpp
# End Source File
# Begin Source File

SOURCE=.\About.cpp
# End Source File
# Begin Source File

SOURCE=.\adsp2100.cpp
# End Source File
# Begin Source File

SOURCE=.\AdvSettings.cpp
# End Source File
# Begin Source File

SOURCE=.\AsmList.cpp
# End Source File
# Begin Source File

SOURCE=.\CaptureMap.cpp
# End Source File
# Begin Source File

SOURCE=.\ControlConfig.cpp
# End Source File
# Begin Source File

SOURCE=.\d3dmath.cpp
# End Source File
# Begin Source File

SOURCE=.\dasmMain.cpp
# End Source File
# Begin Source File

SOURCE=.\Debugger.cpp
# End Source File
# Begin Source File

SOURCE=.\DIPSettings.cpp
# End Source File
# Begin Source File

SOURCE=.\dynaBranch.cpp
# End Source File
# Begin Source File

SOURCE=.\dynaBranchSP.cpp
# End Source File
# Begin Source File

SOURCE=.\DynaCompiler.cpp
# End Source File
# Begin Source File

SOURCE=.\dynaFastMem.cpp
# End Source File
# Begin Source File

SOURCE=.\dynaFP.cpp
# End Source File
# Begin Source File

SOURCE=.\dynaGeneral.cpp
# End Source File
# Begin Source File

SOURCE=.\dynaMemHelpers.cpp
# End Source File
# Begin Source File

SOURCE=.\dynaMemory.cpp
# End Source File
# Begin Source File

SOURCE=.\dynaNative.cpp
# End Source File
# Begin Source File

SOURCE=.\dynaSmartMem.cpp
# End Source File
# Begin Source File

SOURCE=.\EmuObject1.cpp
# End Source File
# Begin Source File

SOURCE=.\hleDSP.cpp
# End Source File
# Begin Source File

SOURCE=.\hleMain.cpp
# End Source File
# Begin Source File

SOURCE=.\iATA.cpp
# End Source File
# Begin Source File

SOURCE=.\iBranchOps.cpp
# End Source File
# Begin Source File

SOURCE=.\iCPU.cpp
# End Source File
# Begin Source File

SOURCE=.\iFPOps.cpp
# End Source File
# Begin Source File

SOURCE=.\iGeneralOps.cpp
# End Source File
# Begin Source File

SOURCE=.\iMain.cpp
# End Source File
# Begin Source File

SOURCE=.\iMemory.cpp
# End Source File
# Begin Source File

SOURCE=.\iMemoryOps.cpp
# End Source File
# Begin Source File

SOURCE=.\iRom.cpp
# End Source File
# Begin Source File

SOURCE=.\ki.cpp
# End Source File
# Begin Source File

SOURCE=.\ki.rc
# End Source File
# Begin Source File

SOURCE=.\KIDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\mmDirectInputDevice.cpp
# End Source File
# Begin Source File

SOURCE=.\mmDirectSoundDevice.cpp
# End Source File
# Begin Source File

SOURCE=.\mmDisplay.cpp
# End Source File
# Begin Source File

SOURCE=.\mmSoundBuffer.cpp
# End Source File
# Begin Source File

SOURCE=.\RomSet.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\TComboBox.cpp
# End Source File
# Begin Source File

SOURCE=.\wave.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\About.h
# End Source File
# Begin Source File

SOURCE=.\AdvSettings.h
# End Source File
# Begin Source File

SOURCE=.\AsmList.h
# End Source File
# Begin Source File

SOURCE=.\CaptureMap.h
# End Source File
# Begin Source File

SOURCE=.\ControlConfig.h
# End Source File
# Begin Source File

SOURCE=.\dasmBranch.h
# End Source File
# Begin Source File

SOURCE=.\dasmFP.h
# End Source File
# Begin Source File

SOURCE=.\dasmGeneral.h
# End Source File
# Begin Source File

SOURCE=.\dasmIns.h
# End Source File
# Begin Source File

SOURCE=.\dasmLookups.h
# End Source File
# Begin Source File

SOURCE=.\dasmMain.h
# End Source File
# Begin Source File

SOURCE=.\dasmMemory.h
# End Source File
# Begin Source File

SOURCE=.\dasmVector.h
# End Source File
# Begin Source File

SOURCE=.\Debugger.h
# End Source File
# Begin Source File

SOURCE=.\DIPSettings.h
# End Source File
# Begin Source File

SOURCE=.\dynaMemHelpers.h
# End Source File
# Begin Source File

SOURCE=.\dynaSmartMem.h
# End Source File
# Begin Source File

SOURCE=.\dynaSPCheat.h
# End Source File
# Begin Source File

SOURCE=.\hleDSP.h
# End Source File
# Begin Source File

SOURCE=.\iATA.h
# End Source File
# Begin Source File

SOURCE=.\iCPU.h
# End Source File
# Begin Source File

SOURCE=.\iDMA.h
# End Source File
# Begin Source File

SOURCE=.\iFPOps.h
# End Source File
# Begin Source File

SOURCE=.\iGeneral.h
# End Source File
# Begin Source File

SOURCE=.\iGeneralOps.h
# End Source File
# Begin Source File

SOURCE=.\iIns.h
# End Source File
# Begin Source File

SOURCE=.\iMain.h
# End Source File
# Begin Source File

SOURCE=.\iMemory.h
# End Source File
# Begin Source File

SOURCE=.\iMemoryOps.h
# End Source File
# Begin Source File

SOURCE=.\ki.h
# End Source File
# Begin Source File

SOURCE=.\KIDlg.h
# End Source File
# Begin Source File

SOURCE=.\NetPlay.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\RomSet.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\TComboBox.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\bitmap1.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bmp00001.bmp
# End Source File
# Begin Source File

SOURCE=.\res\ki.ico
# End Source File
# Begin Source File

SOURCE=.\res\ki.rc2
# End Source File
# End Group
# Begin Source File

SOURCE=.\rcdecoder.h
# End Source File
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# Begin Source File

SOURCE="C:\Program Files\Microsoft Visual Studio\VC98\Lib\WINMM.LIB"
# End Source File
# Begin Source File

SOURCE=..\..\mssdk\lib\dsound.lib
# End Source File
# Begin Source File

SOURCE=..\..\mssdk\lib\ddraw.lib
# End Source File
# Begin Source File

SOURCE=..\..\mssdk\lib\dinput.lib
# End Source File
# Begin Source File

SOURCE=..\..\mssdk\lib\d3dim.lib
# End Source File
# Begin Source File

SOURCE=..\..\mssdk\lib\dxguid.lib
# End Source File
# End Target
# End Project
