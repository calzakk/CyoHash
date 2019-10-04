;---------------------------------------------------------------------
; CyoHash.nsi - part of the CyoHash application
;
; Copyright (c) Graham Bull. All rights reserved.
;
; Redistribution and use in source and binary forms, with or without
; modification, are permitted provided that the following conditions are met:
;
;  * Redistributions of source code must retain the above copyright notice, this
;    list of conditions and the following disclaimer.
;
;  * Redistributions in binary form must reproduce the above copyright notice,
;    this list of conditions and the following disclaimer in the documentation
;    and/or other materials provided with the distribution.
;
; THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
; ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
; WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
; DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
; FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
; DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
; SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
; CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
; OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
; OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
;---------------------------------------------------------------------

!addplugindir plugin\Release

!include "MUI2.nsh"
!include "LogicLib.nsh"

SetCompressor /SOLID /FINAL LZMA

Name "CyoHash"
OutFile "CyoHash.exe"

!define PRODUCT_VERSION 2.4.0
!define FILE_VERSION    2.4.0.0

VIProductVersion "${FILE_VERSION}"
VIAddVersionKey "FileVersion" "${FILE_VERSION}"
VIAddVersionKey "ProductVersion" "${PRODUCT_VERSION}"
VIAddVersionKey "ProductName" "CyoHash"
VIAddVersionKey "Comments" ""
VIAddVersionKey "CompanyName" ""
VIAddVersionKey "LegalCopyright" "(c) Graham Bull. All rights reserved."
VIAddVersionKey "LegalTrademarks" ""
VIAddVersionKey "FileDescription" "CyoHash"
VIAddVersionKey "OriginalFilename" "CyoHash.exe"

RequestExecutionLevel admin ;for Windows Vista+

Var MACHINE_ARCHITECTURE
Var ALREADYINSTALLED

;---------------------------------------------------------------------
;Installation

ShowInstDetails show

!define MUI_ABORTWARNING

!insertmacro MUI_PAGE_WELCOME

ReserveFile "InstallNotes.txt"
Page custom NotesPage

!define MUI_PAGE_CUSTOMFUNCTION_PRE SkipPageIfAlreadyInstalled
!insertmacro MUI_PAGE_DIRECTORY

!define MUI_COMPONENTSPAGE_NODESC
!insertmacro MUI_PAGE_COMPONENTS

!insertmacro MUI_PAGE_INSTFILES

!define MUI_FINISHPAGE_NOAUTOCLOSE
!define MUI_FINISHPAGE_REBOOTLATER_DEFAULT
!insertmacro MUI_PAGE_FINISH

Section "CyoHash" MainSection
    SectionIn RO

    WriteRegStr HKLM "Software\CyoHash" "" "$INSTDIR"

    SetOutPath "$INSTDIR"
    WriteUninstaller "$INSTDIR\Uninstall.exe"
    SetOverwrite try
    ${If} $MACHINE_ARCHITECTURE == "x64"
        ;x64
        File "..\source\x64\Release\CyoHash.exe"
        ClearErrors
        File "..\source\ShellExtension\x64\Release\CyoHash.dll"
        ${If} ${Errors}
            ;cannot overwrite dll
            MessageBox MB_OK|MB_ICONEXCLAMATION "The existing CyoHash DLL is in use, and will be replaced when the machine is rebooted."
            File "/oname=CyoHash0.dll" "..\source\ShellExtension\x64\Release\CyoHash.dll"
            CyoHashInstallerPlugin::SwitchDllsOnReboot "$INSTDIR"
            Pop $0
            SetRebootFlag true
        ${Else}
            ExecWait 'regsvr32 -s "$INSTDIR\CyoHash.dll"'
        ${EndIf}
    ${Else}
        ;x86
        File "..\source\Release\CyoHash.exe"
        ClearErrors
        File "..\source\ShellExtension\Release\CyoHash.dll"
        ${If} ${Errors}
            ;cannot overwrite dll
            MessageBox MB_OK|MB_ICONEXCLAMATION "The existing CyoHash DLL is in use, and will be replaced when the machine is rebooted."
            File "/oname=CyoHash0.dll" "..\source\ShellExtension\Release\CyoHash.dll"
            CyoHashInstallerPlugin::SwitchDllsOnReboot "$INSTDIR"
            Pop $0
            SetRebootFlag true
        ${Else}
            RegDll "$INSTDIR\CyoHash.dll"
        ${EndIf}
    ${EndIf}

    WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\CyoHash" "DisplayName" "CyoHash"
    WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\CyoHash" "DisplayVersion" "${FILE_VERSION}"
    WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\CyoHash" "HelpLink" "https://github.com/calzakk/CyoHash"
    WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\CyoHash" "Publisher" "Graham Bull"
    WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\CyoHash" "UninstallString" "$INSTDIR\Uninstall.exe"
    WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\CyoHash" "NoModify" 1
    WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\CyoHash" "NoRepair" 1
SectionEnd

Function SkipPageIfAlreadyInstalled
    ${If} $ALREADYINSTALLED == "1"
        Abort
    ${EndIf}
FunctionEnd

Function .onInit
    SetPluginUnload alwaysoff

    InitPluginsDir
    File "/oname=$PLUGINSDIR\InstallNotes.txt" "InstallNotes.txt"

    SetRegView 64

    ;Is a CyoHash (un)installer already running?
    System::Call 'kernel32::CreateMutexA(i 0, i 0, t "CyoHash installer") i .r1 ?e'
    Pop $0
    ${If} $0 != "0"
        ;yes - abort
        MessageBox MB_OK|MB_ICONSTOP "A CyoHash (un)installer is already running."
        Abort
    ${EndIf}

    ;Is the user an administrator?
    UserInfo::GetAccountType
    Pop $0
    ${If} $0 != "Admin"
        MessageBox MB_OK|MB_ICONSTOP "You must have administrator privileges to use this installer."
        Abort
    ${EndIf}

    ;Is this a supported platform?
    CyoHashInstallerPlugin::ValidatePlatform
    Pop $0
    ${If} $0 != "1"
        MessageBox MB_OK|MB_ICONSTOP "CyoHash requires Windows 2000/XP/2003/Vista/7/2008 or newer operating systems"
        Abort
    ${EndIf}

    ;Is this an x64 machine?
    CyoHashInstallerPlugin::IsWindowsX64
    Pop $0
    ${If} $0 == "1"
        StrCpy $MACHINE_ARCHITECTURE "x64"
        SectionSetText ${MainSection} "CyoHash (x64 edition)"
    ${Else}
        StrCpy $MACHINE_ARCHITECTURE "x86"
        SectionSetText ${MainSection} "CyoHash"
    ${EndIf}

    ;Already installed?
    ReadRegStr $INSTDIR HKLM "Software\CyoHash" ""
    ${If} $INSTDIR == ""
        StrCpy $ALREADYINSTALLED "0"
        StrCpy $INSTDIR "$PROGRAMFILES64\CyoHash"
    ${Else}
        StrCpy $ALREADYINSTALLED "1"
    ${EndIf}
FunctionEnd

Var HWND_NOTES_DIALOG
Var HWND_NOTES_BOX

Function NotesPage
    !insertmacro MUI_HEADER_TEXT "Installation Notes" "Please read before continuing..."

    nsDialogs::Create /NOUNLOAD 1018
    Pop $HWND_NOTES_DIALOG
    ${If} $HWND_NOTES_DIALOG == error
        Abort
    ${EndIf}

    Push $0 ;save $0

    !define STYLES ${DEFAULT_STYLES}|${WS_VSCROLL}|${ES_READONLY}|${ES_MULTILINE}

    !define EX_STYLES ${WS_EX_WINDOWEDGE}|${WS_EX_CLIENTEDGE}

    nsDialogs::CreateControl "RICHEDIT20A" ${STYLES} ${EX_STYLES} 0u 0u 100% 100% ""
    Pop $HWND_NOTES_BOX

    CyoHashInstallerPlugin::InitRichEditControl $HWND_NOTES_BOX "$PLUGINSDIR\InstallNotes.txt"
    Pop $0
    ${If} $0 != "1"
        Pop $0 ;restore $0
        Abort
    ${EndIf}

    ${NSD_OnNotify} $HWND_NOTES_BOX NotifyHandler

    nsDialogs::Show

    Pop $0 ;restore $0
FunctionEnd

Function NotifyHandler
    Pop $0
    Pop $1
    Pop $2
    CyoHashInstallerPlugin::OnNotify $0 $1 $2
FunctionEnd

;---------------------------------------------------------------------
;Uninstallation

ShowUninstDetails show

!define MUI_UNABORTWARNING

!insertmacro MUI_UNPAGE_WELCOME

!insertmacro MUI_UNPAGE_CONFIRM

!insertmacro MUI_UNPAGE_INSTFILES

!define MUI_UNFINISHPAGE_NOAUTOCLOSE
!insertmacro MUI_UNPAGE_FINISH

Section "Uninstall"
    Delete "$INSTDIR\Uninstall.exe"
    ${If} $MACHINE_ARCHITECTURE == "x64"
        ExecWait 'regsvr32 -s -u "$INSTDIR\CyoHash.dll"'
    ${Else}
        UnRegDll "$INSTDIR\CyoHash.dll"
    ${EndIf}
    Delete /REBOOTOK "$INSTDIR\CyoHash.dll"
    Delete /REBOOTOK "$INSTDIR\CyoHash.exe"
    RMDir /REBOOTOK "$INSTDIR"

    DeleteRegKey HKLM "Software\CyoHash"
    DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\CyoHash"
SectionEnd

Function un.onInit
    SetPluginUnload alwaysoff

    SetRegView 64

    ;Is a CyoHash (un)installer already running?
    System::Call 'kernel32::CreateMutexA(i 0, i 0, t "CyoHash installer") i .r1 ?e'
    Pop $0
    ${If} $0 != "0"
        ;yes - abort
        MessageBox MB_OK|MB_ICONSTOP "A CyoHash (un)installer is already running."
        Abort
    ${EndIf}

    ;Is this an x64 machine?
    CyoHashInstallerPlugin::IsWindowsX64
    Pop $0
    ${If} $0 == "1"
        StrCpy $MACHINE_ARCHITECTURE "x64"
    ${Else}
        StrCpy $MACHINE_ARCHITECTURE "x86"
    ${EndIf}
FunctionEnd

;---------------------------------------------------------------------

!insertmacro MUI_LANGUAGE "English"
