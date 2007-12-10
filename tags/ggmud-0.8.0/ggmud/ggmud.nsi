; GGMUD.nsi
;
; This script is based on example2.nsi.
;
; It will install GGMud into a directory that the user selects,
;
; example launch from linux with cxoffice
; /usr/local/cxoffice/bin/wine /usr/local/cxoffice/support/dotwine/fake_windows/Program\ Files/NSIS/makensis.exe ggmud.nsi
;--------------------------------

; The name of the installer
Name "GGMud"

; The file to write
OutFile "GGMud-setup.exe"

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
  SetOutPath "$INSTDIR\bin"
  
  ; Put file there
  File "ggmud.exe"
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

Section "GTK runtime (required)"

  SectionIn RO
  SetOutPath $INSTDIR

  File /r "runtime\lib" 
  File /r "runtime\bin" 
  File /r "runtime\etc" 
  File /r "runtime\share" 
SectionEnd

; Optional section (can be disabled by the user)
Section "Start Menu Shortcuts"

  CreateDirectory "$SMPROGRAMS\GGMud"
  CreateShortCut "$SMPROGRAMS\GGMud\Uninstall.lnk" "$INSTDIR\uninstall.exe" "" "$INSTDIR\bin\uninstall.exe" 0
  CreateShortCut "$SMPROGRAMS\GGMud\Launch GGMud.lnk" "$INSTDIR\bin\ggmud.exe"
  
SectionEnd

Section "Desktop Shortcut"
  CreateShortCut "$DESKTOP\GGMud.lnk" "$INSTDIR\bin\ggmud.exe"   
SectionEnd

;--------------------------------

; Uninstaller

Section "Uninstall"
  
  ; Remove registry keys
  DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\GGMud"
  DeleteRegKey HKLM SOFTWARE\GGMud

  ; Remove files and uninstaller
  Delete $INSTDIR\bin\*.exe
  Delete $INSTDIR\bin\*.dll
  Delete $INSTDIR\uninstall.exe
  Delete $INSTDIR\bin\gg_help.txt
  
  RMDir /r "$INSTDIR\lib"
  RMDir /r "$INSTDIR\share"
  RMDir /r "$INSTDIR\etc"
  RMDir "$INSTDIR\bin"

  ; Remove shortcuts, if any
  Delete "$SMPROGRAMS\GGMud\*.*"
  Delete "$DESKTOP\GGMud.lnk"
  ; Remove directories used
  RMDir "$SMPROGRAMS\GGMud"
  
  RMDir "$INSTDIR"

SectionEnd
