; GGMUD.nsi
;
; This script is based on example2.nsi.
;
; It will install GGMud into a directory that the user selects,

;--------------------------------

; The name of the installer
Name "GGMud"

; The file to write
OutFile "GG-Setup.exe"

; The default installation directory
InstallDir $PROGRAMFILES\GGMud

; Registry key to check for directory (so if you install again, it will 
; overwrite the old one automatically)
InstallDirRegKey HKLM "Software\GGMud" "Install_Dir"

;--------------------------------

; Pages

Page components
Page directory
Page instfiles

UninstPage uninstConfirm
UninstPage instfiles

;--------------------------------

; The stuff to install
Section "GGMud (required)"

  SectionIn RO
  
  ; Set output path to the installation directory.
  SetOutPath $INSTDIR
  
  ; Put file there
  File "ggmud.exe"
  File "libgdk-0.dll"
  File "libgtk-0.dll"
  File "libglib-2.0-0.dll"
  File "libgobject-2.0-0.dll"
  File "libgthread-2.0-0.dll"
  File "iconv.dll"
  File "charset.dll"
  File "libintl-1.dll"
  File "gg_help.txt"
  
  ; Write the installation path into the registry
  WriteRegStr HKLM SOFTWARE\GGMud "Install_Dir" "$INSTDIR"
  
  ; Write the uninstall keys for Windows
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\GGMud" "DisplayName" "GGMud"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\GGMud" "UninstallString" '"$INSTDIR\uninstall.exe"'
  WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\GGMud" "NoModify" 1
  WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\GGMud" "NoRepair" 1
  WriteUninstaller "uninstall.exe"
  
SectionEnd

; Optional section (can be disabled by the user)
Section "Start Menu Shortcuts"

  CreateDirectory "$SMPROGRAMS\GGMud"
  CreateShortCut "$SMPROGRAMS\GGMud\Uninstall.lnk" "$INSTDIR\uninstall.exe" "" "$INSTDIR\uninstall.exe" 0
  CreateShortCut "$SMPROGRAMS\GGMud\Launch GGMud.lnk" "$INSTDIR\ggmud.exe"
  
SectionEnd

Section "Desktop Shortcut"
  CreateShortCut "$DESKTOP\GGMud.lnk" "$INSTDIR\ggmud.exe"   
SectionEnd

;--------------------------------

; Uninstaller

Section "Uninstall"
  
  ; Remove registry keys
  DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\GGMud"
  DeleteRegKey HKLM SOFTWARE\GGMud

  ; Remove files and uninstaller
  Delete $INSTDIR\ggmud.exe
  Delete $INSTDIR\*.dll
  Delete $INSTDIR\uninstall.exe
  Delete $INSTDIR\gg_help.txt
  
  ; Remove shortcuts, if any
  Delete "$SMPROGRAMS\GGMud\*.*"
  Delete "$DESKTOP\GGMud.lnk"
  ; Remove directories used
  RMDir "$SMPROGRAMS\GGMud"
  
  RMDir "$INSTDIR"

SectionEnd
