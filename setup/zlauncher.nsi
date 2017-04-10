!define PRODUCT_NAME "ZLauncher"
!define PRODUCT_PUBLISHER "tkashkin"
!define PRODUCT_WEB_SITE "https://github.com/tkashkin/ZLauncher"
!define PRODUCT_README "${PRODUCT_WEB_SITE}"
!define MUI_ICON "..\zlauncher.ico"

!include "MUI2.nsh"

Name "${PRODUCT_NAME}"
OutFile "..\bin\Release\ZLauncher_setup.exe"

InstallDir "$LOCALAPPDATA\ZLauncher"

RequestExecutionLevel user

!define MUI_PAGE_HEADER_TEXT "${PRODUCT_NAME}"
!define MUI_PAGE_HEADER_SUBTEXT "The Legend of Zelda: Breath of the Wild custom launcher"

!define MUI_WELCOMEPAGE_TITLE "${MUI_PAGE_HEADER_TEXT}"
!define MUI_WELCOMEPAGE_TEXT "${MUI_PAGE_HEADER_SUBTEXT}$\r$\n$\r$\nFeatures:$\r$\n- Launch game (both with user and admin priveleges)$\r$\n- Launch speedhack (CheatEngine standalone trainer)$\r$\n- Supports launching from Steam library as Non-Steam game$\r$\n- Launch game through Steam with admin priveleges$\r$\n- Set CPU affinity (CPU cores) for Cemu$\r$\n- Video cutscenes playback with mpv"

!define MUI_FINISHPAGE_LINK "ZLauncher github page"
!define MUI_FINISHPAGE_LINK_LOCATION "${PRODUCT_WEB_SITE}"
!define MUI_FINISHPAGE_NOREBOOTSUPPORT

!define MUI_WELCOMEFINISHPAGE_BITMAP "wizard.bmp"

Var cemupath
Var cemuelevate
Var cemuhookchecked
Var gamepath
Var shinst
Var shenable
Var vpinst
Var vpenable

!include "config.nsdinc"

!insertmacro MUI_PAGE_WELCOME
!insertmacro MUI_PAGE_DIRECTORY
!insertmacro MUI_PAGE_INSTFILES

Page custom preConfigPage configPageFinish

!define MUI_FINISHPAGE_RUN
!define MUI_FINISHPAGE_RUN_TEXT "Open ZLauncher directory"
!define MUI_FINISHPAGE_RUN_FUNCTION openInstallDir
!define MUI_PAGE_CUSTOMFUNCTION_PRE disableBackButton
!insertmacro MUI_PAGE_FINISH

!insertmacro MUI_LANGUAGE "English"

Section "MainSection" SEC01
	SetOutPath "$INSTDIR"
	
	BringToFront
	
	SetOverwrite on
	
	File "..\bin\Release\zlauncher.exe"
SectionEnd

Function preConfigPage
	IfFileExists "$INSTDIR\zlauncher.ini" cfgexists
		File "zlauncher.ini"
		Call fnc_config_Show
	cfgexists:
FunctionEnd

Function cemuElevateCheck
	Pop $0
	${NSD_GetState} $hCtl_config_cemuElevateCheckbox $cemuelevate
FunctionEnd
Function shInstCheck
	Pop $0
	${NSD_GetState} $hCtl_config_speedhackInstallCheckbox $shinst
FunctionEnd
Function shEnableCheck
	Pop $0
	${NSD_GetState} $hCtl_config_speedhackEnableCheckbox $shenable
FunctionEnd
Function vpInstCheck
	Pop $0
	${NSD_GetState} $hCtl_config_vpInstallCheckbox $vpinst
FunctionEnd
Function vpEnableCheck
	Pop $0
	${NSD_GetState} $hCtl_config_vpEnableCheckbox $vpenable
FunctionEnd

Function cemuElevateChange
	Call cemuElevateCheck
FunctionEnd
Function speedhackInstallChange
	Call shInstCheck
	${If} $shinst == ${BST_UNCHECKED}
		EnableWindow $hCtl_config_speedhackEnableCheckbox 0
	${Else}
		EnableWindow $hCtl_config_speedhackEnableCheckbox 1
	${EndIf}
FunctionEnd
Function vpInstallChange
	Call vpInstCheck
	${If} $vpinst == ${BST_UNCHECKED}
		EnableWindow $hCtl_config_vpEnableCheckbox 0
	${Else}
		EnableWindow $hCtl_config_vpEnableCheckbox 1
		
		${If} $cemuhookchecked == true
			Goto return
		${EndIf}
		
		IfFileExists "$cemupath\dbghelp.dll" cemuhook
			Goto return
		cemuhook:
			StrCpy $cemuhookchecked true
			MessageBox MB_YESNO "Cemuhook is installed. Do you still want to use ZLauncher's video playback?" IDYES return IDNO novideo
			novideo:
				${NSD_SetState} $hCtl_config_vpInstallCheckbox ${BST_UNCHECKED}
				Call vpInstallChange
		return:
	${EndIf}
FunctionEnd

Function cemuDirPicked
	${NSD_GetText} $hCtl_config_cemuDirPicker_Txt $cemupath
	IfFileExists "$cemupath\Cemu.exe" cemuok
		MessageBox MB_ICONSTOP|MB_OK "Invalid Cemu directory. It must contain Cemu.exe"
		Abort
	cemuok:
	Call vpInstallChange
FunctionEnd
Function gameDirPicked
	${NSD_GetText} $hCtl_config_gameDirPicker_Txt $gamepath
	IfFileExists "$gamepath\code\U-King.rpx" gameok
		MessageBox MB_ICONSTOP|MB_OK "Invalid game directory. It must contain code\U-King.rpx"
		Abort
	gameok:
FunctionEnd

Function configPageFinish
	Call cemuDirPicked
	Call gameDirPicked
	
	Call cemuElevateCheck
	Call shInstCheck
	Call shEnableCheck
	Call vpInstCheck
	Call vpEnableCheck
	
	WriteINIStr "$INSTDIR\zlauncher.ini" cemu path "$cemupath"
	WriteINIStr "$INSTDIR\zlauncher.ini" game path "$gamepath"
	
	WriteINIStr "$INSTDIR\zlauncher.ini" game elevate false
	WriteINIStr "$INSTDIR\zlauncher.ini" speedhack enabled false
	WriteINIStr "$INSTDIR\zlauncher.ini" video enabled false
	
	${If} $cemuelevate != ${BST_UNCHECKED}
		WriteINIStr "$INSTDIR\zlauncher.ini" game elevate true
	${EndIf}
	
	${If} $shinst != ${BST_UNCHECKED}
		File "..\bin\Release\speedhack.exe"
		${If} $shenable != ${BST_UNCHECKED}
			WriteINIStr "$INSTDIR\zlauncher.ini" speedhack enabled true
		${EndIf}
	${EndIf}
	
	${If} $vpinst != ${BST_UNCHECKED}
		File "..\bin\Release\mpv.exe"
		${If} $vpenable != ${BST_UNCHECKED}
			WriteINIStr "$INSTDIR\zlauncher.ini" video enabled true
		${EndIf}
	${EndIf}
	
FunctionEnd

Function openInstallDir
	Pop $0
	ExecShell "open" "$INSTDIR"
FunctionEnd
Function openConfigReadme
	Pop $0
	ExecShell "open" "${PRODUCT_WEB_SITE}#configuration"
FunctionEnd
Function disableBackButton 
	GetDlgItem $R0 $HWNDPARENT 3 
	EnableWindow $R0 0 
FunctionEnd