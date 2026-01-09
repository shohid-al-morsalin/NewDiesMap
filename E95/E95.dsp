# Microsoft Developer Studio Project File - Name="E95" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=E95 - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "E95.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "E95.mak" CFG="E95 - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "E95 - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "E95 - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "E95 - Win32 Release"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 5
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "C:\C2C\E95"
# PROP Intermediate_Dir "Z:\E95"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /I "C:\SC\C2C\E95" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 OmniDriver32.lib common32.lib /nologo /subsystem:windows /machine:I386

!ELSEIF  "$(CFG)" == "E95 - Win32 Debug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 5
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "C:\E95ASE"
# PROP Intermediate_Dir "C:\RAM\E95H"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /Zp1 /MTd /W3 /Gm /GX /ZI /Od /I ".\\" /I ".\..\413\OceanOp\include" /D "DEV_GCLIB" /D "_E95" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_OLE32_" /D "PAUSEPGM" /FR /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 Ws2_32.lib opengl32.lib glu32.lib glaux.lib /nologo /subsystem:windows /debug /machine:I386 /nodefaultlib:"LIBCMT.lib" /pdbtype:sept /libpath:".\\"
# SUBTRACT LINK32 /pdb:none

!ENDIF 

# Begin Target

# Name "E95 - Win32 Release"
# Name "E95 - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\413App.cpp
# End Source File
# Begin Source File

SOURCE=.\413AppBase.cpp
# End Source File
# Begin Source File

SOURCE=.\413AppCam.cpp
# End Source File
# Begin Source File

SOURCE=.\413AppEcho.cpp
# End Source File
# Begin Source File

SOURCE=.\413AppMtr.cpp
# End Source File
# Begin Source File

SOURCE=.\413AppPatt.cpp
# End Source File
# Begin Source File

SOURCE=.\413AppStep.cpp
# End Source File
# Begin Source File

SOURCE=.\413AppThinF.cpp
# End Source File
# Begin Source File

SOURCE=.\413AppTrim.cpp
# End Source File
# Begin Source File

SOURCE=.\413AppTrimStep.cpp
# End Source File
# Begin Source File

SOURCE=.\413Confg.cpp
# End Source File
# Begin Source File

SOURCE=.\413SVar.cpp
# End Source File
# Begin Source File

SOURCE=.\acroaxdocshim.cpp
# End Source File
# Begin Source File

SOURCE=.\AdvanceOption.cpp
# End Source File
# Begin Source File

SOURCE=.\AdvanceSetup.cpp
# End Source File
# Begin Source File

SOURCE=.\AdvanceSetup6.cpp
# End Source File
# Begin Source File

SOURCE=.\AlarmAllDlg.cpp
# End Source File
# Begin Source File

SOURCE=..\ONLINE\MFC\AlarmListDlg.cpp
# End Source File
# Begin Source File

SOURCE=..\ALRM\AlarmSystem.cpp
# End Source File
# Begin Source File

SOURCE=.\Alignment.cpp
# End Source File
# Begin Source File

SOURCE=.\AlrmItem.cpp
# End Source File
# Begin Source File

SOURCE=.\AlrmList.cpp
# End Source File
# Begin Source File

SOURCE=..\ONLINE\GEM\Annotator.cpp
# End Source File
# Begin Source File

SOURCE=..\SRC\Arams.cpp
# End Source File
# Begin Source File

SOURCE=.\AreUSure.cpp
# End Source File
# Begin Source File

SOURCE=..\FEM\Ldp\AsystFoup.cpp
# End Source File
# Begin Source File

SOURCE=.\AttachDelete.cpp
# End Source File
# Begin Source File

SOURCE=..\SRC\BCMenu.cpp
# End Source File
# Begin Source File

SOURCE=..\SRC\BMake.cpp
# End Source File
# Begin Source File

SOURCE=..\FEM\Rbt\BrkUnit.cpp
# End Source File
# Begin Source File

SOURCE=..\GRID\BtnST.cpp
# End Source File
# Begin Source File

SOURCE=..\SRC\BtnST1.cpp
# End Source File
# Begin Source File

SOURCE=.\CamBase.cpp
# End Source File
# Begin Source File

SOURCE=.\CamLumi.cpp
# End Source File
# Begin Source File

SOURCE=.\CamSi.cpp
# End Source File
# Begin Source File

SOURCE=.\CarrierObj.cpp
# End Source File
# Begin Source File

SOURCE=..\128\CAS\CasMgr.cpp
# End Source File
# Begin Source File

SOURCE=.\CBatchDoc.cpp
# End Source File
# Begin Source File

SOURCE=.\CCcrPatt.cpp
# End Source File
# Begin Source File

SOURCE=..\ONLINE\GEM\CediListCtroller.cpp
# End Source File
# Begin Source File

SOURCE=..\ONLINE\GEM\CEID_ITEM.cpp
# End Source File
# Begin Source File

SOURCE=..\ONLINE\GEM\CEID_LIST.cpp
# End Source File
# Begin Source File

SOURCE=..\FEM\Ldp\CFoup.cpp
# End Source File
# Begin Source File

SOURCE=.\CGridPatt.cpp
# End Source File
# Begin Source File

SOURCE=.\ChartWnd.cpp
# End Source File
# Begin Source File

SOURCE=..\PropList\ChkComboBox.cpp
# End Source File
# Begin Source File

SOURCE=.\ChuckMask.cpp
# End Source File
# Begin Source File

SOURCE=..\ONLINE\ShareMemory\Cidrw.cpp
# End Source File
# Begin Source File

SOURCE=.\CLine.cpp
# End Source File
# Begin Source File

SOURCE=..\LOG\CLogMsgItem.cpp
# End Source File
# Begin Source File

SOURCE=.\CLPXChDat.cpp
# End Source File
# Begin Source File

SOURCE=.\CMPList.cpp
# End Source File
# Begin Source File

SOURCE=..\ONLINE\GEM\Cms.cpp
# End Source File
# Begin Source File

SOURCE=..\ONLINE\MFC\ColorButton.cpp
# End Source File
# Begin Source File

SOURCE=..\ONLINE\MFC\ColorEdit.cpp
# End Source File
# Begin Source File

SOURCE=..\ONLINE\MFC\ColorList.cpp
# End Source File
# Begin Source File

SOURCE=..\ONLINE\MFC\ColorStatic.cpp
# End Source File
# Begin Source File

SOURCE=..\FEM\Ldp\ComBase.cpp
# End Source File
# Begin Source File

SOURCE=..\FEM\Ldp\ComFL300.cpp
# End Source File
# Begin Source File

SOURCE=..\ONLINE\TCPIP\ComLog.cpp
# End Source File
# Begin Source File

SOURCE=..\SRC\Comm.cpp
# End Source File
# Begin Source File

SOURCE=..\SRC\CommSvr.cpp
# End Source File
# Begin Source File

SOURCE=..\FEM\Ldp\ComSMIF.cpp
# End Source File
# Begin Source File

SOURCE=..\FEM\Ldp\ComTDK.cpp
# End Source File
# Begin Source File

SOURCE=..\FEM\Ldp\ComThread.cpp
# End Source File
# Begin Source File

SOURCE=.\Configuration.cpp
# End Source File
# Begin Source File

SOURCE=.\ContentMap.cpp
# End Source File
# Begin Source File

SOURCE=.\Coor.cpp
# End Source File
# Begin Source File

SOURCE=.\CRdlPatt.cpp
# End Source File
# Begin Source File

SOURCE=.\CRing.cpp
# End Source File
# Begin Source File

SOURCE=.\CrossHair.cpp
# End Source File
# Begin Source File

SOURCE=.\CrossPattern.cpp
# End Source File
# Begin Source File

SOURCE=.\CStat.cpp
# End Source File
# Begin Source File

SOURCE=..\ONLINE\GEM\CtrlJob.cpp
# End Source File
# Begin Source File

SOURCE=..\ONLINE\GEM\CtrlJobMgr.cpp
# End Source File
# Begin Source File

SOURCE=..\SYS\CUMMsg.cpp
# End Source File
# Begin Source File

SOURCE=.\CurveFit.cpp
# End Source File
# Begin Source File

SOURCE=..\SRC\customfiledlg.cpp
# End Source File
# Begin Source File

SOURCE=.\CustPatt.cpp
# End Source File
# Begin Source File

SOURCE=.\CWaferParam.cpp
# End Source File
# Begin Source File

SOURCE=..\TCP\CWinSock.cpp
# End Source File
# Begin Source File

SOURCE=.\Data.cpp
# End Source File
# Begin Source File

SOURCE=..\ONLINE\ShareMemory\DataBank.cpp
# End Source File
# Begin Source File

SOURCE=..\ONLINE\GEM\DataConverter.cpp
# End Source File
# Begin Source File

SOURCE=.\DataDesc413.cpp
# End Source File
# Begin Source File

SOURCE=.\DataList.cpp
# End Source File
# Begin Source File

SOURCE=.\DefGrid2Dlg.cpp
# End Source File
# Begin Source File

SOURCE=.\DefineCircular.cpp
# End Source File
# Begin Source File

SOURCE=.\DefineCircularSegment.cpp
# End Source File
# Begin Source File

SOURCE=.\DefineGrid.cpp
# End Source File
# Begin Source File

SOURCE=.\DefineRadial.cpp
# End Source File
# Begin Source File

SOURCE=.\DefineSimuPeaks.cpp
# End Source File
# Begin Source File

SOURCE=..\MFC\DelimiterDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\DiceLoc.cpp
# End Source File
# Begin Source File

SOURCE=.\DiceView.cpp
# End Source File
# Begin Source File

SOURCE=..\SRC\DirSys.cpp
# End Source File
# Begin Source File

SOURCE=..\413\DMCMotor.cpp
# End Source File
# Begin Source File

SOURCE=..\413\DMCMotorBase.cpp
# End Source File
# Begin Source File

SOURCE=.\DosName.cpp
# End Source File
# Begin Source File

SOURCE=..\SRC\DosUtil.cpp
# End Source File
# Begin Source File

SOURCE=.\Dump.cpp
# End Source File
# Begin Source File

SOURCE=.\DuplicateDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\E95.cpp
# End Source File
# Begin Source File

SOURCE=.\E95.rc
# End Source File
# Begin Source File

SOURCE=.\E95Doc.cpp
# End Source File
# Begin Source File

SOURCE=.\E95View.cpp
# End Source File
# Begin Source File

SOURCE=.\EchoData.cpp
# End Source File
# Begin Source File

SOURCE=.\EchoDraw.cpp
# End Source File
# Begin Source File

SOURCE=..\413\EchoFinder.cpp
# End Source File
# Begin Source File

SOURCE=..\413\EchoScope\EchoPeak.cpp
# End Source File
# Begin Source File

SOURCE=..\413\EchoScope\EchoProbe.cpp
# End Source File
# Begin Source File

SOURCE=..\413\EchoScope\EchoProbeBase.cpp
# End Source File
# Begin Source File

SOURCE=.\EchoProbeProp.cpp
# End Source File
# Begin Source File

SOURCE=.\EchoTest.cpp
# End Source File
# Begin Source File

SOURCE=.\EditBinDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\EditCoordinateDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\EditDocumentationDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\EditPoint.cpp
# End Source File
# Begin Source File

SOURCE=.\EditThickDlg.cpp
# End Source File
# Begin Source File

SOURCE=..\ONLINE\MFC\EditToolBar.cpp
# End Source File
# Begin Source File

SOURCE=.\EnlargeDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\EqpState.cpp
# End Source File
# Begin Source File

SOURCE=..\IPC\EqpStatus.cpp
# End Source File
# Begin Source File

SOURCE=..\ONLINE\MFC\EquipEvtThread.cpp
# End Source File
# Begin Source File

SOURCE=..\LOG\ErrorDlg.cpp
# End Source File
# Begin Source File

SOURCE=..\ONLINE\MFC\establishcomm.cpp
# End Source File
# Begin Source File

SOURCE=..\ONLINE\ShareMemory\Evt.cpp
# End Source File
# Begin Source File

SOURCE=..\413\OceanOp\FFreq.cpp
# End Source File
# Begin Source File

SOURCE=.\FFTPoint.cpp
# End Source File
# Begin Source File

SOURCE=.\FFUThread.cpp
# End Source File
# Begin Source File

SOURCE=..\SRC\FileVersion.cpp
# End Source File
# Begin Source File

SOURCE=..\PropList\FoldersDialog.cpp
# End Source File
# Begin Source File

SOURCE=..\413\OceanOp\Fourier.cpp
# End Source File
# Begin Source File

SOURCE=.\FPoint.cpp
# End Source File
# Begin Source File

SOURCE=..\ONLINE\TCPIP\FsmCoreSocket.cpp
# End Source File
# Begin Source File

SOURCE=..\LTW\FSMLightTower.cpp
# End Source File
# Begin Source File

SOURCE=.\GEM.cpp
# End Source File
# Begin Source File

SOURCE=.\GEMEvt.cpp
# End Source File
# Begin Source File

SOURCE=..\ONLINE\GEM\GemFactory.cpp
# End Source File
# Begin Source File

SOURCE=..\ONLINE\general_utils.cpp
# End Source File
# Begin Source File

SOURCE=..\SRC\GetFileNames.cpp
# End Source File
# Begin Source File

SOURCE=..\ONLINE\MFC\getpw.cpp
# End Source File
# Begin Source File

SOURCE=..\413\GLMotor.cpp
# End Source File
# Begin Source File

SOURCE=..\413\GLMotor_v1.cpp
# End Source File
# Begin Source File

SOURCE=.\Global413.cpp
# End Source File
# Begin Source File

SOURCE=.\GlobalDeviceContainer.cpp
# End Source File
# Begin Source File

SOURCE=.\GlobalUIContainer.cpp
# End Source File
# Begin Source File

SOURCE=..\SRC\HdrList.cpp
# End Source File
# Begin Source File

SOURCE=.\HelpProbes.cpp
# End Source File
# Begin Source File

SOURCE=.\HelpTrim.cpp
# End Source File
# Begin Source File

SOURCE=.\HistoDraw.cpp
# End Source File
# Begin Source File

SOURCE=..\ONLINE\MFC\HostMessage.cpp
# End Source File
# Begin Source File

SOURCE=..\PLT\HSL2RGB.cpp
# End Source File
# Begin Source File

SOURCE=..\ONLINE\TCPIP\HSMSComThread.cpp
# End Source File
# Begin Source File

SOURCE=..\ONLINE\TCPIP\HsmsParaMeter.cpp
# End Source File
# Begin Source File

SOURCE=..\FEM\IDR\IDRAsyst.cpp
# End Source File
# Begin Source File

SOURCE=..\FEM\IDR\IDRMgr.cpp
# End Source File
# Begin Source File

SOURCE=..\FEM\IDR\IDRSvr.cpp
# End Source File
# Begin Source File

SOURCE=..\FEM\IDR\IDRUnit.cpp
# End Source File
# Begin Source File

SOURCE=..\FEM\IDR\IDRUnitKey.cpp
# End Source File
# Begin Source File

SOURCE=..\FEM\IDR\IDRUnitOmron.cpp
# End Source File
# Begin Source File

SOURCE=.\Illumination.cpp
# End Source File
# Begin Source File

SOURCE=..\ONLINE\MFC\ImageStatic.cpp
# End Source File
# Begin Source File

SOURCE=.\ImgWnd.cpp
# End Source File
# Begin Source File

SOURCE=.\IncmplPrcsHdlr.cpp
# End Source File
# Begin Source File

SOURCE=.\InfoPanelAlarms.cpp
# End Source File
# Begin Source File

SOURCE=.\InfoPanelEqpMessage.cpp
# End Source File
# Begin Source File

SOURCE=.\InfoPanelHelp.cpp
# End Source File
# Begin Source File

SOURCE=.\InfoPanelHelpHTML.cpp
# End Source File
# Begin Source File

SOURCE=.\InfoPanelHostMsg.cpp
# End Source File
# Begin Source File

SOURCE=.\InfoPanelLogs.cpp
# End Source File
# Begin Source File

SOURCE=.\InfoPanelLogsMainDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\InfoPanelMain.cpp
# End Source File
# Begin Source File

SOURCE=.\InfoPanelRecipes.cpp
# End Source File
# Begin Source File

SOURCE=.\InfoPanelSetup.cpp
# End Source File
# Begin Source File

SOURCE=.\InfoPanelSystem.cpp
# End Source File
# Begin Source File

SOURCE=.\InfoPanelView.cpp
# End Source File
# Begin Source File

SOURCE=.\InfoPanelWelcome.cpp
# End Source File
# Begin Source File

SOURCE=..\SRC\IniFile.cpp
# End Source File
# Begin Source File

SOURCE=..\SRC\IniFileHandler.cpp
# End Source File
# Begin Source File

SOURCE=.\InputCalibWaferThickness.cpp
# End Source File
# Begin Source File

SOURCE=..\FEM\IDR\InputCarrierID.cpp
# End Source File
# Begin Source File

SOURCE=.\InputFloat.cpp
# End Source File
# Begin Source File

SOURCE=..\CurveFit\Interface.cpp

!IF  "$(CFG)" == "E95 - Win32 Release"

!ELSEIF  "$(CFG)" == "E95 - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\InvDistance.cpp
# End Source File
# Begin Source File

SOURCE=.\IssueAlarmDlg.cpp
# End Source File
# Begin Source File

SOURCE=..\128\CAS\JobInfo.cpp
# End Source File
# Begin Source File

SOURCE=..\MFC\Keyboard.cpp
# End Source File
# Begin Source File

SOURCE=..\MFC\KeyPad.cpp
# End Source File
# Begin Source File

SOURCE=..\SRC\Led.cpp
# End Source File
# Begin Source File

SOURCE=.\LineChart.cpp
# End Source File
# Begin Source File

SOURCE=..\SERIAL\LinkPort.cpp
# End Source File
# Begin Source File

SOURCE=..\SERIAL\Links.cpp
# End Source File
# Begin Source File

SOURCE=..\SERIAL\LinkSerial.cpp
# End Source File
# Begin Source File

SOURCE=..\SERIAL\LinkTCP.cpp
# End Source File
# Begin Source File

SOURCE=..\CurveFit\lm_eval.c

!IF  "$(CFG)" == "E95 - Win32 Release"

!ELSEIF  "$(CFG)" == "E95 - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\CurveFit\lmmin.c

!IF  "$(CFG)" == "E95 - Win32 Release"

!ELSEIF  "$(CFG)" == "E95 - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\LoadPortDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\LoadPos.cpp
# End Source File
# Begin Source File

SOURCE=.\LocationProp.cpp
# End Source File
# Begin Source File

SOURCE=.\LocDefineDice.cpp
# End Source File
# Begin Source File

SOURCE=..\IPC\Lock.cpp
# End Source File
# Begin Source File

SOURCE=..\SYS\Log.cpp
# End Source File
# Begin Source File

SOURCE=.\LogE.cpp
# End Source File
# Begin Source File

SOURCE=.\LogFEM.cpp
# End Source File
# Begin Source File

SOURCE=..\SRC\LogInDataNew.cpp
# End Source File
# Begin Source File

SOURCE=.\LogItem.cpp
# End Source File
# Begin Source File

SOURCE=..\ONLINE\TCPIP\LogMessage.cpp
# End Source File
# Begin Source File

SOURCE=..\LOG\LogMsgList.cpp
# End Source File
# Begin Source File

SOURCE=..\LOG\LogSvr.cpp
# End Source File
# Begin Source File

SOURCE=..\FEM\Ldp\LPMgr.cpp
# End Source File
# Begin Source File

SOURCE=.\LPObj.cpp
# End Source File
# Begin Source File

SOURCE=.\LPStatus.cpp
# End Source File
# Begin Source File

SOURCE=..\FEM\Ldp\LPSvr.cpp
# End Source File
# Begin Source File

SOURCE=.\LSF3D.cpp
# End Source File
# Begin Source File

SOURCE=..\LTW\LTMgrE95.cpp
# End Source File
# Begin Source File

SOURCE=..\LTW\LTWMgr.cpp
# End Source File
# Begin Source File

SOURCE=..\IPC\LTwrVar.cpp
# End Source File
# Begin Source File

SOURCE=.\MainFrm.cpp
# End Source File
# Begin Source File

SOURCE=.\ManualAlgnDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\MaskInfo.cpp
# End Source File
# Begin Source File

SOURCE=.\MaskPattern.cpp
# End Source File
# Begin Source File

SOURCE=.\MaxMin.cpp
# End Source File
# Begin Source File

SOURCE=.\MaxMinPa.cpp
# End Source File
# Begin Source File

SOURCE=..\413\Mc2.cpp
# End Source File
# Begin Source File

SOURCE=.\MC3.cpp
# End Source File
# Begin Source File

SOURCE=.\MeasurementProperties.cpp
# End Source File
# Begin Source File

SOURCE=.\MeParam.cpp
# End Source File
# Begin Source File

SOURCE=.\MeSetMore.cpp
# End Source File
# Begin Source File

SOURCE=..\SRC\MessageBoard.cpp
# End Source File
# Begin Source File

SOURCE=..\SRC\MessageBoardLog.cpp
# End Source File
# Begin Source File

SOURCE=..\SRC\MessageFormat.cpp
# End Source File
# Begin Source File

SOURCE=..\ONLINE\TCPIP\Monitor.cpp
# End Source File
# Begin Source File

SOURCE=.\MotionControl.cpp
# End Source File
# Begin Source File

SOURCE=..\413\MotorGL.cpp
# End Source File
# Begin Source File

SOURCE=..\413\MotorL2.cpp
# End Source File
# Begin Source File

SOURCE=..\413\MotorParam.cpp
# End Source File
# Begin Source File

SOURCE=..\413\MotorS2.cpp
# End Source File
# Begin Source File

SOURCE=..\413\MotorSi.cpp
# End Source File
# Begin Source File

SOURCE=.\MoveParam.cpp
# End Source File
# Begin Source File

SOURCE=.\MPoint.cpp
# End Source File
# Begin Source File

SOURCE=..\SERIAL\MsgItem.cpp
# End Source File
# Begin Source File

SOURCE=.\MsgItemProcessor.cpp
# End Source File
# Begin Source File

SOURCE=..\SRC\MYEDIT.CPP
# End Source File
# Begin Source File

SOURCE=..\SRC\MyTextButton.cpp
# End Source File
# Begin Source File

SOURCE=.\NavigationPanel.cpp
# End Source File
# Begin Source File

SOURCE=..\128\CAS\NCas.cpp
# End Source File
# Begin Source File

SOURCE=..\LOG\NoticeDlg.cpp
# End Source File
# Begin Source File

SOURCE=..\SERIAL\NThread.cpp
# End Source File
# Begin Source File

SOURCE=..\128\GEM\ObjectsContainer.cpp
# End Source File
# Begin Source File

SOURCE=..\413\OceanOp\OceanBase.cpp
# End Source File
# Begin Source File

SOURCE=..\413\OceanOp\OceanOpNIRQuest.cpp
# End Source File
# Begin Source File

SOURCE=..\128\GEM\OLnClt.cpp
# End Source File
# Begin Source File

SOURCE=.\OLnSvrObj.cpp
# End Source File
# Begin Source File

SOURCE=.\OnlineAlarmDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\OnlineCEIDDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\OnlineCJDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\OnLineDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\OnlineECIDDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\OnlineEventsDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\OnlineHSMSDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\OnlineLogsDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\OnlineMgr.cpp
# End Source File
# Begin Source File

SOURCE=.\OnlineMgrB.cpp
# End Source File
# Begin Source File

SOURCE=.\OnlineModeDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\OnlinePJDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\OnlineSubstrateDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\OnlineSVIDDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\OptimizeEchoDlg.cpp
# End Source File
# Begin Source File

SOURCE=..\ONLINE\GEM\Ossbase.cpp
# End Source File
# Begin Source File

SOURCE=..\IPC\OSSMemS.cpp
# End Source File
# Begin Source File

SOURCE=.\PackedDIB1.cpp
# End Source File
# Begin Source File

SOURCE=..\ONLINE\MFC\ParaEditDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\Pattern.cpp
# End Source File
# Begin Source File

SOURCE=.\PatternProp.cpp
# End Source File
# Begin Source File

SOURCE=.\PattMatch.cpp
# End Source File
# Begin Source File

SOURCE=.\PeakControl.cpp
# End Source File
# Begin Source File

SOURCE=.\PeakControlItem.cpp
# End Source File
# Begin Source File

SOURCE=.\PeakParam.cpp
# End Source File
# Begin Source File

SOURCE=..\413\EchoScope\Peaks.cpp
# End Source File
# Begin Source File

SOURCE=.\PeakShape.cpp
# End Source File
# Begin Source File

SOURCE=.\PersistFrameImpl.cpp
# End Source File
# Begin Source File

SOURCE=.\PersistSDIFrame.cpp
# End Source File
# Begin Source File

SOURCE=.\PolyParams.cpp
# End Source File
# Begin Source File

SOURCE=..\128\GEM\PortParam.cpp
# End Source File
# Begin Source File

SOURCE=.\PosHt.cpp
# End Source File
# Begin Source File

SOURCE=.\PProfile.cpp
# End Source File
# Begin Source File

SOURCE=..\FEM\Rbt\PriUnit.cpp
# End Source File
# Begin Source File

SOURCE=..\ONLINE\GEM\Prjob.cpp
# End Source File
# Begin Source File

SOURCE=..\128\GEM\PrJobList.cpp
# End Source File
# Begin Source File

SOURCE=..\ONLINE\GEM\PRJobMgr.cpp
# End Source File
# Begin Source File

SOURCE=.\ProbeDistParam.cpp
# End Source File
# Begin Source File

SOURCE=.\ProbeTest.cpp
# End Source File
# Begin Source File

SOURCE=.\ProcJob413ProcessingDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\ProcJobCancelCarrierDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\ProcJobCommon.cpp
# End Source File
# Begin Source File

SOURCE=.\ProcJobCompleteDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\ProcJobCompleteManual.cpp
# End Source File
# Begin Source File

SOURCE=.\ProcJobDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\ProcJobInfo.cpp
# End Source File
# Begin Source File

SOURCE=.\ProcJobManageDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\ProcJobManDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\ProcJobManual.cpp
# End Source File
# Begin Source File

SOURCE=.\ProcJobProcessingB.cpp
# End Source File
# Begin Source File

SOURCE=.\ProcJobPWC1DlgBase.cpp
# End Source File
# Begin Source File

SOURCE=.\ProcJobPWC2Dlg.cpp
# End Source File
# Begin Source File

SOURCE=.\ProcJobPWC413Dlg.cpp
# End Source File
# Begin Source File

SOURCE=.\ProcJobPWCRDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\ProcJobQue413Dlg.cpp
# End Source File
# Begin Source File

SOURCE=.\ProcJobSMICDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\ProcJobUnloadCarrierDlg.cpp
# End Source File
# Begin Source File

SOURCE=..\PropList\PropertyList.cpp
# End Source File
# Begin Source File

SOURCE=..\ONLINE\ShareMemory\PtrListEx.cpp
# End Source File
# Begin Source File

SOURCE=..\FEM\Ldp\QueOutMgr.cpp
# End Source File
# Begin Source File

SOURCE=..\SERIAL\Queue.cpp
# End Source File
# Begin Source File

SOURCE=.\QuickRecipeEditDlg.cpp
# End Source File
# Begin Source File

SOURCE=..\MFC\RangeDlg.cpp
# End Source File
# Begin Source File

SOURCE=..\FEM\Rbt\RbtBase.cpp
# End Source File
# Begin Source File

SOURCE=..\FEM\Rbt\RbtMgr.cpp
# End Source File
# Begin Source File

SOURCE=..\FEM\Rbt\RbtUnit.cpp
# End Source File
# Begin Source File

SOURCE=..\ONLINE\ShareMemory\ReadWriteHead.cpp
# End Source File
# Begin Source File

SOURCE=.\Recipe413.cpp
# End Source File
# Begin Source File

SOURCE=.\Recipe413NSMgr.cpp
# End Source File
# Begin Source File

SOURCE=.\RecipeCreateDlg413.cpp
# End Source File
# Begin Source File

SOURCE=.\RecipeCreateDlgBase.cpp
# End Source File
# Begin Source File

SOURCE=.\RecipeIO.cpp
# End Source File
# Begin Source File

SOURCE=.\RecipeManageDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\RecoveryDlg.cpp
# End Source File
# Begin Source File

SOURCE=..\SRC\ResizableDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\ResizableFormView.cpp
# End Source File
# Begin Source File

SOURCE=..\SRC\ResizableGrip.cpp
# End Source File
# Begin Source File

SOURCE=..\SRC\ResizableLayout.cpp
# End Source File
# Begin Source File

SOURCE=..\SRC\ResizableMinMax.cpp
# End Source File
# Begin Source File

SOURCE=..\SRC\ResizableState.cpp
# End Source File
# Begin Source File

SOURCE=.\RGB3.cpp
# End Source File
# Begin Source File

SOURCE=.\PZT\Roll.cpp
# End Source File
# Begin Source File

SOURCE=.\RunRecipe.cpp
# End Source File
# Begin Source File

SOURCE=..\ONLINE\SECS\S2F23.cpp
# End Source File
# Begin Source File

SOURCE=..\ONLINE\SECS\S2F31.cpp
# End Source File
# Begin Source File

SOURCE=..\ONLINE\SECS\S2F41.cpp
# End Source File
# Begin Source File

SOURCE=..\ONLINE\SECS\S2Msg.cpp
# End Source File
# Begin Source File

SOURCE=..\IPC\SArams.cpp
# End Source File
# Begin Source File

SOURCE=..\413\EchoScope\ScanLine.cpp
# End Source File
# Begin Source File

SOURCE=.\ScanLineChart.cpp
# End Source File
# Begin Source File

SOURCE=..\413\EchoScope\ScanPoint.cpp
# End Source File
# Begin Source File

SOURCE=..\ONLINE\GEM\SecsItem.cpp
# End Source File
# Begin Source File

SOURCE=..\ONLINE\GEM\SecsMessage.cpp
# End Source File
# Begin Source File

SOURCE=.\SecsParam.cpp
# End Source File
# Begin Source File

SOURCE=.\SecsParamItem.cpp
# End Source File
# Begin Source File

SOURCE=..\IFC\SecsSvrF.cpp
# End Source File
# Begin Source File

SOURCE=.\SelectDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\SelectMeasurementDlg.cpp
# End Source File
# Begin Source File

SOURCE=..\ONLINE\TCPIP\SelectMode.cpp
# End Source File
# Begin Source File

SOURCE=.\SelectResol.cpp
# End Source File
# Begin Source File

SOURCE=.\SelectVCamera.cpp
# End Source File
# Begin Source File

SOURCE=..\FEM\Rbt\selport.cpp
# End Source File
# Begin Source File

SOURCE=.\SelScanNumDlg.cpp
# End Source File
# Begin Source File

SOURCE=..\SERIAL\SerialICC.cpp
# End Source File
# Begin Source File

SOURCE=..\SERIAL\SerialTh.cpp
# End Source File
# Begin Source File

SOURCE=.\SetupProb4Dlg.cpp
# End Source File
# Begin Source File

SOURCE=.\SetupProp10Dlg.cpp
# End Source File
# Begin Source File

SOURCE=.\SetupProp11Dlg.cpp
# End Source File
# Begin Source File

SOURCE=.\SetupProp12Dlg.cpp
# End Source File
# Begin Source File

SOURCE=.\SetupProp13Dlg.cpp
# End Source File
# Begin Source File

SOURCE=.\SetupProp1Dlg.cpp
# End Source File
# Begin Source File

SOURCE=.\SetupProp2Dlg.cpp
# End Source File
# Begin Source File

SOURCE=.\SetupProp31Dlg.cpp
# End Source File
# Begin Source File

SOURCE=.\SetupProp3Dlg.cpp
# End Source File
# Begin Source File

SOURCE=.\SetupProp41Dlg.cpp
# End Source File
# Begin Source File

SOURCE=.\SetupProp42Dlg.cpp
# End Source File
# Begin Source File

SOURCE=.\SetupProp43Dlg.cpp
# End Source File
# Begin Source File

SOURCE=.\SetupProp51Dlg.cpp
# End Source File
# Begin Source File

SOURCE=.\SetupProp52Dlg.cpp
# End Source File
# Begin Source File

SOURCE=.\SetupProp5Dlg.cpp
# End Source File
# Begin Source File

SOURCE=.\SetupProp6Dlg.cpp
# End Source File
# Begin Source File

SOURCE=.\SetupProp71Dlg.cpp
# End Source File
# Begin Source File

SOURCE=.\SetupProp72Dlg.cpp
# End Source File
# Begin Source File

SOURCE=.\SetupProp73Dlg.cpp
# End Source File
# Begin Source File

SOURCE=.\SetupProp7Dlg.cpp
# End Source File
# Begin Source File

SOURCE=.\SetupProp81Dlg.cpp
# End Source File
# Begin Source File

SOURCE=.\SetupProp82Dlg.cpp
# End Source File
# Begin Source File

SOURCE=.\SetupProp83Dlg.cpp
# End Source File
# Begin Source File

SOURCE=.\SetupProp84Dlg.cpp
# End Source File
# Begin Source File

SOURCE=.\SetupProp8Dlg.cpp
# End Source File
# Begin Source File

SOURCE=.\SetupProp91Dlg.cpp
# End Source File
# Begin Source File

SOURCE=.\SetupProp9Dlg.cpp
# End Source File
# Begin Source File

SOURCE=.\SetupPropComm.cpp
# End Source File
# Begin Source File

SOURCE=..\ONLINE\ShareMemory\SharedMemQue.cpp
# End Source File
# Begin Source File

SOURCE=..\ONLINE\ShareMemory\ShareMemory.cpp
# End Source File
# Begin Source File

SOURCE=..\ONLINE\ShareMemory\ShareQue.cpp
# End Source File
# Begin Source File

SOURCE=..\413\SIBase.cpp
# End Source File
# Begin Source File

SOURCE=.\SICam.cpp
# End Source File
# Begin Source File

SOURCE=.\SICamPoints.cpp
# End Source File
# Begin Source File

SOURCE=.\SiCamSettingDlg.cpp
# End Source File
# Begin Source File

SOURCE=..\413\SIMotor.cpp
# End Source File
# Begin Source File

SOURCE=.\SimulatePeaks.cpp
# End Source File
# Begin Source File

SOURCE=.\SimuSpecDlg.cpp
# End Source File
# Begin Source File

SOURCE=..\SRC\sinstance.cpp
# End Source File
# Begin Source File

SOURCE=..\IPC\SMCHdr.cpp
# End Source File
# Begin Source File

SOURCE=..\IPC\SMClt.cpp
# End Source File
# Begin Source File

SOURCE=..\IPC\SMHdr.cpp
# End Source File
# Begin Source File

SOURCE=..\IPC\SMIpc.cpp
# End Source File
# Begin Source File

SOURCE=..\IPC\Smol.cpp
# End Source File
# Begin Source File

SOURCE=..\IPC\SMSnd.cpp
# End Source File
# Begin Source File

SOURCE=..\IPC\SMSvr.cpp
# End Source File
# Begin Source File

SOURCE=..\SRC\SortHeaderCtrl.cpp
# End Source File
# Begin Source File

SOURCE=..\SRC\SortListCtrl.cpp
# End Source File
# Begin Source File

SOURCE=..\413\OceanOp\Spec.cpp
# End Source File
# Begin Source File

SOURCE=.\SpecPar.cpp
# End Source File
# Begin Source File

SOURCE=.\SpecPlot.cpp
# End Source File
# Begin Source File

SOURCE=..\413\OceanOp\SpecPoint.cpp
# End Source File
# Begin Source File

SOURCE=..\ONLINE\MFC\SpoolMan.cpp
# End Source File
# Begin Source File

SOURCE=.\ST_SplitterWnd.cpp
# End Source File
# Begin Source File

SOURCE=..\128\GEM\StateModels.cpp
# End Source File
# Begin Source File

SOURCE=.\StatusReq.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\StopDlg.cpp
# End Source File
# Begin Source File

SOURCE=..\TCP\StreamSocket.cpp
# End Source File
# Begin Source File

SOURCE=.\StrSOp.cpp
# End Source File
# Begin Source File

SOURCE=..\IPC\SubstObj.cpp
# End Source File
# Begin Source File

SOURCE=..\ONLINE\GEM\SVID_ITEM.cpp
# End Source File
# Begin Source File

SOURCE=..\ONLINE\GEM\SVID_LIST.cpp
# End Source File
# Begin Source File

SOURCE=..\IPC\SvrThread.cpp
# End Source File
# Begin Source File

SOURCE=..\128\CAS\SysInfo.cpp
# End Source File
# Begin Source File

SOURCE=.\System413Dlg.cpp
# End Source File
# Begin Source File

SOURCE=.\SystemLTowerDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\SystemRobotDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\SystemStatusDlg.cpp
# End Source File
# Begin Source File

SOURCE=..\LTW\TCtrlBase.cpp
# End Source File
# Begin Source File

SOURCE=..\LTW\TCtrlBrk.cpp
# End Source File
# Begin Source File

SOURCE=..\LTW\TCtrlDB.cpp
# End Source File
# Begin Source File

SOURCE=..\LTW\TCtrlDBNet.cpp
# End Source File
# Begin Source File

SOURCE=..\LTW\TCtrlPatLitSerial.cpp
# End Source File
# Begin Source File

SOURCE=..\IPC\TelnetClientSocket.cpp
# End Source File
# Begin Source File

SOURCE=..\SRC\TextFile.cpp
# End Source File
# Begin Source File

SOURCE=.\TFTestDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\ThinFilmProp.cpp
# End Source File
# Begin Source File

SOURCE=.\ThkValues.cpp
# End Source File
# Begin Source File

SOURCE=..\SRC\TimerMGR.cpp
# End Source File
# Begin Source File

SOURCE=..\SRC\Tips.cpp
# End Source File
# Begin Source File

SOURCE=.\TitlePanel.cpp
# End Source File
# Begin Source File

SOURCE=..\SRC\Tokenizer.cpp
# End Source File
# Begin Source File

SOURCE=..\SYS\Tool.cpp
# End Source File
# Begin Source File

SOURCE=..\IPC\Toolvar.cpp
# End Source File
# Begin Source File

SOURCE=..\ONLINE\GEM\TracTimer.cpp
# End Source File
# Begin Source File

SOURCE=..\413\TxR.cpp
# End Source File
# Begin Source File

SOURCE=..\SYS\Var.cpp
# End Source File
# Begin Source File

SOURCE=..\ONLINE\MFC\VarDisp.cpp
# End Source File
# Begin Source File

SOURCE=.\ViewBaseline.cpp
# End Source File
# Begin Source File

SOURCE=.\ViewCassDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\ViewContentDlg413.cpp
# End Source File
# Begin Source File

SOURCE=.\ViewContentDlgBase.cpp
# End Source File
# Begin Source File

SOURCE=.\ViewData.cpp
# End Source File
# Begin Source File

SOURCE=.\ViewFileDlg.cpp
# End Source File
# Begin Source File

SOURCE=..\128\CAS\WaferInfo.cpp
# End Source File
# Begin Source File

SOURCE=..\128\CAS\WaferInfoMgr.cpp
# End Source File
# Begin Source File

SOURCE=.\WaferMap.cpp
# End Source File
# Begin Source File

SOURCE=.\WaferMapDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\WaferMapSetting.cpp
# End Source File
# Begin Source File

SOURCE=.\WaferViewOptions.cpp
# End Source File
# Begin Source File

SOURCE=..\ONLINE\ShareMemory\WaitMsgList.cpp
# End Source File
# Begin Source File

SOURCE=..\LOG\WarningDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\webbrowser2.cpp
# End Source File
# Begin Source File

SOURCE=.\WelcomeDlg.cpp
# End Source File
# Begin Source File

SOURCE=..\SRC\xSkinButton.cpp
# End Source File
# Begin Source File

SOURCE=.\XTabCtrl.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\413App.h
# End Source File
# Begin Source File

SOURCE=.\413AppBase.h
# End Source File
# Begin Source File

SOURCE=.\413AppCam.h
# End Source File
# Begin Source File

SOURCE=.\413AppEcho.h
# End Source File
# Begin Source File

SOURCE=.\413AppMtr.h
# End Source File
# Begin Source File

SOURCE=.\413AppPatt.h
# End Source File
# Begin Source File

SOURCE=.\413AppStep.h
# End Source File
# Begin Source File

SOURCE=.\413AppThinF.h
# End Source File
# Begin Source File

SOURCE=.\413AppTrim.h
# End Source File
# Begin Source File

SOURCE=.\413AppTrimStep.h
# End Source File
# Begin Source File

SOURCE=.\413Confg.h
# End Source File
# Begin Source File

SOURCE=.\413SVar.h
# End Source File
# Begin Source File

SOURCE=.\acroaxdocshim.h
# End Source File
# Begin Source File

SOURCE=.\AdvanceOption.h
# End Source File
# Begin Source File

SOURCE=.\AdvanceSetup.h
# End Source File
# Begin Source File

SOURCE=.\AdvanceSetup6.h
# End Source File
# Begin Source File

SOURCE=.\AlarmAllDlg.h
# End Source File
# Begin Source File

SOURCE=..\ONLINE\MFC\AlarmListDlg.h
# End Source File
# Begin Source File

SOURCE=..\ALRM\AlarmSystem.h
# End Source File
# Begin Source File

SOURCE=.\Alignment.h
# End Source File
# Begin Source File

SOURCE=.\AlrmItem.h
# End Source File
# Begin Source File

SOURCE=.\AlrmList.h
# End Source File
# Begin Source File

SOURCE=..\ONLINE\GEM\Annotator.h
# End Source File
# Begin Source File

SOURCE=..\SRC\Arams.h
# End Source File
# Begin Source File

SOURCE=.\AreUSure.h
# End Source File
# Begin Source File

SOURCE=..\FEM\Ldp\AsystFoup.h
# End Source File
# Begin Source File

SOURCE=.\AttachDelete.h
# End Source File
# Begin Source File

SOURCE=..\SRC\BCMenu.h
# End Source File
# Begin Source File

SOURCE=..\FEM\Rbt\BrkUnit.h
# End Source File
# Begin Source File

SOURCE=..\GRID\BtnST.h
# End Source File
# Begin Source File

SOURCE=..\SRC\BtnST1.h
# End Source File
# Begin Source File

SOURCE=.\CamBase.h
# End Source File
# Begin Source File

SOURCE=.\CamLumi.h
# End Source File
# Begin Source File

SOURCE=.\CamSi.h
# End Source File
# Begin Source File

SOURCE=.\CarrierObj.h
# End Source File
# Begin Source File

SOURCE=..\128\CAS\CasMgr.h
# End Source File
# Begin Source File

SOURCE=.\CBatchDoc.h
# End Source File
# Begin Source File

SOURCE=..\413\EchoScope\cbw.h
# End Source File
# Begin Source File

SOURCE=.\CCcrPatt.h
# End Source File
# Begin Source File

SOURCE=..\ONLINE\GEM\CediListCtroller.h
# End Source File
# Begin Source File

SOURCE=..\ONLINE\GEM\CEID_ITEM.h
# End Source File
# Begin Source File

SOURCE=..\ONLINE\GEM\CEID_LIST.h
# End Source File
# Begin Source File

SOURCE=..\GRID\CellRange.h
# End Source File
# Begin Source File

SOURCE=..\FEM\Ldp\CFoup.h
# End Source File
# Begin Source File

SOURCE=.\CGridPatt.h
# End Source File
# Begin Source File

SOURCE=.\ChartWnd.h
# End Source File
# Begin Source File

SOURCE=..\PropList\ChkComboBox.h
# End Source File
# Begin Source File

SOURCE=.\ChuckMask.h
# End Source File
# Begin Source File

SOURCE=..\ONLINE\ShareMemory\Cidrw.h
# End Source File
# Begin Source File

SOURCE=.\CLine.h
# End Source File
# Begin Source File

SOURCE=..\LOG\CLogMsgItem.h
# End Source File
# Begin Source File

SOURCE=.\CLPXChDat.h
# End Source File
# Begin Source File

SOURCE=.\CMPList.h
# End Source File
# Begin Source File

SOURCE=..\ONLINE\GEM\Cms.h
# End Source File
# Begin Source File

SOURCE=..\ONLINE\MFC\ColorButton.h
# End Source File
# Begin Source File

SOURCE=..\ONLINE\MFC\ColorEdit.h
# End Source File
# Begin Source File

SOURCE=..\ONLINE\MFC\ColorList.h
# End Source File
# Begin Source File

SOURCE=..\ONLINE\MFC\ColorStatic.h
# End Source File
# Begin Source File

SOURCE=..\FEM\Ldp\ComBase.h
# End Source File
# Begin Source File

SOURCE=..\FEM\Ldp\ComFL300.h
# End Source File
# Begin Source File

SOURCE=..\ONLINE\TCPIP\ComLog.h
# End Source File
# Begin Source File

SOURCE=..\SRC\Comm.h
# End Source File
# Begin Source File

SOURCE=..\SRC\CommSvr.h
# End Source File
# Begin Source File

SOURCE=..\FEM\Ldp\ComSMIF.h
# End Source File
# Begin Source File

SOURCE=..\FEM\Ldp\ComTDK.h
# End Source File
# Begin Source File

SOURCE=..\FEM\Ldp\ComThread.h
# End Source File
# Begin Source File

SOURCE=.\Configuration.h
# End Source File
# Begin Source File

SOURCE=.\Const.h
# End Source File
# Begin Source File

SOURCE=..\IPC\Constant.h
# End Source File
# Begin Source File

SOURCE=.\ContentMap.h
# End Source File
# Begin Source File

SOURCE=.\Coor.h
# End Source File
# Begin Source File

SOURCE=.\CRdlPatt.h
# End Source File
# Begin Source File

SOURCE=.\CRing.h
# End Source File
# Begin Source File

SOURCE=.\CrossHair.h
# End Source File
# Begin Source File

SOURCE=.\CrossPattern.h
# End Source File
# Begin Source File

SOURCE=.\CStat.h
# End Source File
# Begin Source File

SOURCE=..\ONLINE\GEM\CtrlJob.h
# End Source File
# Begin Source File

SOURCE=..\ONLINE\GEM\CtrlJobMgr.h
# End Source File
# Begin Source File

SOURCE=..\SYS\CUMMsg.h
# End Source File
# Begin Source File

SOURCE=.\CurveFit.h
# End Source File
# Begin Source File

SOURCE=..\SRC\customfiledlg.h
# End Source File
# Begin Source File

SOURCE=.\CustPatt.h
# End Source File
# Begin Source File

SOURCE=.\CWaferParam.h
# End Source File
# Begin Source File

SOURCE=..\TCP\CWinSock.h
# End Source File
# Begin Source File

SOURCE=.\Data.h
# End Source File
# Begin Source File

SOURCE=..\ONLINE\ShareMemory\DataBank.h
# End Source File
# Begin Source File

SOURCE=..\ONLINE\GEM\DataConverter.h
# End Source File
# Begin Source File

SOURCE=.\DataDesc413.h
# End Source File
# Begin Source File

SOURCE=.\DataList.h
# End Source File
# Begin Source File

SOURCE=.\DefGrid2Dlg.h
# End Source File
# Begin Source File

SOURCE=..\ONLINE\INC\Define.h
# End Source File
# Begin Source File

SOURCE=.\DefineCircular.h
# End Source File
# Begin Source File

SOURCE=.\DefineCircularSegment.h
# End Source File
# Begin Source File

SOURCE=.\DefineGrid.h
# End Source File
# Begin Source File

SOURCE=.\DefineRadial.h
# End Source File
# Begin Source File

SOURCE=.\DefineSimuPeaks.h
# End Source File
# Begin Source File

SOURCE=..\MFC\DelimiterDlg.h
# End Source File
# Begin Source File

SOURCE=.\DiceLoc.h
# End Source File
# Begin Source File

SOURCE=.\DiceView.h
# End Source File
# Begin Source File

SOURCE=..\SRC\DirSys.h
# End Source File
# Begin Source File

SOURCE=..\413\Dmccom.h
# End Source File
# Begin Source File

SOURCE=..\413\dmcdrc.h
# End Source File
# Begin Source File

SOURCE=..\413\dmcdrco.h
# End Source File
# Begin Source File

SOURCE=..\413\DMCMLIB.H
# End Source File
# Begin Source File

SOURCE=..\413\DMCMotor.h
# End Source File
# Begin Source File

SOURCE=..\413\DMCMotorBase.h
# End Source File
# Begin Source File

SOURCE=.\DosName.h
# End Source File
# Begin Source File

SOURCE=..\SRC\DOSUtil.h
# End Source File
# Begin Source File

SOURCE=.\Dump.h
# End Source File
# Begin Source File

SOURCE=.\DuplicateDlg.h
# End Source File
# Begin Source File

SOURCE="..\ONLINE\INC\E5-ErrCodes.h"
# End Source File
# Begin Source File

SOURCE=.\E95.h
# End Source File
# Begin Source File

SOURCE=.\E95Doc.h
# End Source File
# Begin Source File

SOURCE=.\E95View.h
# End Source File
# Begin Source File

SOURCE=.\EchoData.h
# End Source File
# Begin Source File

SOURCE=.\EchoDraw.h
# End Source File
# Begin Source File

SOURCE=..\413\EchoFinder.h
# End Source File
# Begin Source File

SOURCE=..\413\EchoScope\EchoPeak.h
# End Source File
# Begin Source File

SOURCE=..\413\EchoScope\EchoProbe.h
# End Source File
# Begin Source File

SOURCE=..\413\EchoScope\EchoProbeBase.h
# End Source File
# Begin Source File

SOURCE=.\EchoProbeProp.h
# End Source File
# Begin Source File

SOURCE=.\EchoTest.h
# End Source File
# Begin Source File

SOURCE=.\EditBinDlg.h
# End Source File
# Begin Source File

SOURCE=.\EditCoordinateDlg.h
# End Source File
# Begin Source File

SOURCE=.\EditDocumentationDlg.h
# End Source File
# Begin Source File

SOURCE=.\EditPoint.h
# End Source File
# Begin Source File

SOURCE=.\EditThickDlg.h
# End Source File
# Begin Source File

SOURCE=..\ONLINE\MFC\EditToolBar.h
# End Source File
# Begin Source File

SOURCE=.\EnlargeDlg.h
# End Source File
# Begin Source File

SOURCE=.\EqpState.h
# End Source File
# Begin Source File

SOURCE=..\IPC\EqpStatus.h
# End Source File
# Begin Source File

SOURCE=..\ONLINE\MFC\EquipEvtThread.h
# End Source File
# Begin Source File

SOURCE=..\LOG\ErrorDlg.h
# End Source File
# Begin Source File

SOURCE=..\ONLINE\MFC\establishcomm.h
# End Source File
# Begin Source File

SOURCE=..\ONLINE\INC\Event.h
# End Source File
# Begin Source File

SOURCE=..\ONLINE\ShareMemory\Evt.h
# End Source File
# Begin Source File

SOURCE=..\413\OceanOp\FFreq.h
# End Source File
# Begin Source File

SOURCE=..\413\OceanOp\FFTINC.h
# End Source File
# Begin Source File

SOURCE=.\FFTPoint.h
# End Source File
# Begin Source File

SOURCE=.\FFUThread.h
# End Source File
# Begin Source File

SOURCE=..\SRC\FileVersion.h
# End Source File
# Begin Source File

SOURCE=..\PropList\FoldersDialog.h
# End Source File
# Begin Source File

SOURCE=..\SRC\FolderUtils.h
# End Source File
# Begin Source File

SOURCE=..\413\OceanOp\Fourier.h
# End Source File
# Begin Source File

SOURCE=.\FPoint.h
# End Source File
# Begin Source File

SOURCE=..\ONLINE\TCPIP\FsmCoreSocket.h
# End Source File
# Begin Source File

SOURCE=..\LTW\FSMLightTower.h
# End Source File
# Begin Source File

SOURCE=.\GEM.h
# End Source File
# Begin Source File

SOURCE=.\GEMEvt.h
# End Source File
# Begin Source File

SOURCE=..\ONLINE\GEM\GemFactory.h
# End Source File
# Begin Source File

SOURCE=..\ONLINE\ShareMemory\GemHead.h
# End Source File
# Begin Source File

SOURCE=..\ONLINE\general_utils.h
# End Source File
# Begin Source File

SOURCE=..\ONLINE\MFC\getpw.h
# End Source File
# Begin Source File

SOURCE=..\413\GLMotor.h
# End Source File
# Begin Source File

SOURCE=..\413\GLMotor_v1.h
# End Source File
# Begin Source File

SOURCE=.\Global413.h
# End Source File
# Begin Source File

SOURCE=.\GlobalDeviceContainer.h
# End Source File
# Begin Source File

SOURCE=.\GlobalUIContainer.h
# End Source File
# Begin Source File

SOURCE=.\PZT\Graphs.h
# End Source File
# Begin Source File

SOURCE=..\SRC\HdrList.h
# End Source File
# Begin Source File

SOURCE=.\HelpProbes.h
# End Source File
# Begin Source File

SOURCE=.\HelpTrim.h
# End Source File
# Begin Source File

SOURCE=.\HistoDraw.h
# End Source File
# Begin Source File

SOURCE=..\ONLINE\MFC\HostMessage.h
# End Source File
# Begin Source File

SOURCE=..\PLT\HSL2RGB.h
# End Source File
# Begin Source File

SOURCE=..\ONLINE\TCPIP\HSMSComThread.h
# End Source File
# Begin Source File

SOURCE=..\ONLINE\TCPIP\HsmsParaMeter.h
# End Source File
# Begin Source File

SOURCE=..\ONLINE\INC\IDlist.h
# End Source File
# Begin Source File

SOURCE=..\FEM\IDR\IDRAsyst.h
# End Source File
# Begin Source File

SOURCE=..\FEM\IDR\IDRMgr.h
# End Source File
# Begin Source File

SOURCE=..\FEM\IDR\IDRSvr.h
# End Source File
# Begin Source File

SOURCE=..\FEM\IDR\IDRUnit.h
# End Source File
# Begin Source File

SOURCE=..\FEM\IDR\IDRUnitKey.h
# End Source File
# Begin Source File

SOURCE=..\FEM\IDR\IDRUnitOmron.h
# End Source File
# Begin Source File

SOURCE=.\Illumination.h
# End Source File
# Begin Source File

SOURCE=..\ONLINE\MFC\ImageStatic.h
# End Source File
# Begin Source File

SOURCE=.\ImgWnd.h
# End Source File
# Begin Source File

SOURCE=.\IncmplPrcsHdlr.h
# End Source File
# Begin Source File

SOURCE=.\InfoPanelAlarms.h
# End Source File
# Begin Source File

SOURCE=.\InfoPanelEqpMessage.h
# End Source File
# Begin Source File

SOURCE=.\InfoPanelHelp.h
# End Source File
# Begin Source File

SOURCE=.\InfoPanelHelpHTML.h
# End Source File
# Begin Source File

SOURCE=.\InfoPanelHostMsg.h
# End Source File
# Begin Source File

SOURCE=.\InfoPanelLogs.h
# End Source File
# Begin Source File

SOURCE=.\InfoPanelLogsMainDlg.h
# End Source File
# Begin Source File

SOURCE=.\InfoPanelMain.h
# End Source File
# Begin Source File

SOURCE=.\InfoPanelRecipes.h
# End Source File
# Begin Source File

SOURCE=.\InfoPanelSetup.h
# End Source File
# Begin Source File

SOURCE=.\InfoPanelSystem.h
# End Source File
# Begin Source File

SOURCE=.\InfoPanelView.h
# End Source File
# Begin Source File

SOURCE=.\InfoPanelWelcome.h
# End Source File
# Begin Source File

SOURCE=..\SRC\IniFile.h
# End Source File
# Begin Source File

SOURCE=..\SRC\IniFileHandler.h
# End Source File
# Begin Source File

SOURCE=.\InputCalibWaferThickness.h
# End Source File
# Begin Source File

SOURCE=..\FEM\IDR\InputCarrierID.h
# End Source File
# Begin Source File

SOURCE=.\InputFloat.h
# End Source File
# Begin Source File

SOURCE=..\CurveFit\Interface.h
# End Source File
# Begin Source File

SOURCE=.\InvDistance.h
# End Source File
# Begin Source File

SOURCE=.\IssueAlarmDlg.h
# End Source File
# Begin Source File

SOURCE=..\128\CAS\JobInfo.h
# End Source File
# Begin Source File

SOURCE=..\MFC\Keyboard.h
# End Source File
# Begin Source File

SOURCE=..\MFC\KeyPad.h
# End Source File
# Begin Source File

SOURCE=..\FEM\Ldp\LdPTOut.h
# End Source File
# Begin Source File

SOURCE=..\SRC\Led.h
# End Source File
# Begin Source File

SOURCE=..\SRC\LedRC.h
# End Source File
# Begin Source File

SOURCE=.\LineChart.h
# End Source File
# Begin Source File

SOURCE=..\SERIAL\LinkPort.h
# End Source File
# Begin Source File

SOURCE=..\SERIAL\Links.h
# End Source File
# Begin Source File

SOURCE=..\SERIAL\LinkSerial.h
# End Source File
# Begin Source File

SOURCE=..\SERIAL\LinkTCP.h
# End Source File
# Begin Source File

SOURCE=..\CurveFit\lm_eval.h
# End Source File
# Begin Source File

SOURCE=..\CurveFit\lmmin.h
# End Source File
# Begin Source File

SOURCE=.\LoadPortDlg.h
# End Source File
# Begin Source File

SOURCE=.\LoadPos.h
# End Source File
# Begin Source File

SOURCE=.\LocationProp.h
# End Source File
# Begin Source File

SOURCE=.\LocDefineDice.h
# End Source File
# Begin Source File

SOURCE=..\IPC\Lock.h
# End Source File
# Begin Source File

SOURCE=..\SYS\Log.h
# End Source File
# Begin Source File

SOURCE=.\LogE.h
# End Source File
# Begin Source File

SOURCE=.\LogFEM.h
# End Source File
# Begin Source File

SOURCE=..\SRC\LogInDataNew.h
# End Source File
# Begin Source File

SOURCE=.\LogItem.h
# End Source File
# Begin Source File

SOURCE=..\ONLINE\TCPIP\LogMessage.h
# End Source File
# Begin Source File

SOURCE=..\LOG\LogMsgList.h
# End Source File
# Begin Source File

SOURCE=..\LOG\LogSvr.h
# End Source File
# Begin Source File

SOURCE=..\FEM\Ldp\LPMgr.h
# End Source File
# Begin Source File

SOURCE=.\LPObj.h
# End Source File
# Begin Source File

SOURCE=.\LPStatus.h
# End Source File
# Begin Source File

SOURCE=..\FEM\Ldp\LPSvr.h
# End Source File
# Begin Source File

SOURCE=.\LSF3D.h
# End Source File
# Begin Source File

SOURCE=..\LTW\LTMgrE95.h
# End Source File
# Begin Source File

SOURCE=..\LTW\LTWMgr.h
# End Source File
# Begin Source File

SOURCE=..\IPC\LTwrVar.h
# End Source File
# Begin Source File

SOURCE=.\MainFrm.h
# End Source File
# Begin Source File

SOURCE=.\ManualAlgnDlg.h
# End Source File
# Begin Source File

SOURCE=.\MaskInfo.h
# End Source File
# Begin Source File

SOURCE=.\MaskPattern.h
# End Source File
# Begin Source File

SOURCE=.\MaxMin.h
# End Source File
# Begin Source File

SOURCE=.\MaxMinPa.h
# End Source File
# Begin Source File

SOURCE=..\413\Mc2.h
# End Source File
# Begin Source File

SOURCE=.\MC3.h
# End Source File
# Begin Source File

SOURCE=.\MeasurementProperties.h
# End Source File
# Begin Source File

SOURCE=.\MeParam.h
# End Source File
# Begin Source File

SOURCE=.\MeSetMore.h
# End Source File
# Begin Source File

SOURCE=..\SRC\MessageBoard.h
# End Source File
# Begin Source File

SOURCE=..\SRC\MessageBoardLog.h
# End Source File
# Begin Source File

SOURCE=..\SRC\MessageFormat.h
# End Source File
# Begin Source File

SOURCE=..\ONLINE\TCPIP\Monitor.h
# End Source File
# Begin Source File

SOURCE=.\MotionControl.h
# End Source File
# Begin Source File

SOURCE=..\413\MotorGL.h
# End Source File
# Begin Source File

SOURCE=..\413\MotorL2.h
# End Source File
# Begin Source File

SOURCE=..\413\MotorParam.h
# End Source File
# Begin Source File

SOURCE=..\413\MotorS2.h
# End Source File
# Begin Source File

SOURCE=..\413\MotorSi.h
# End Source File
# Begin Source File

SOURCE=.\MoveParam.h
# End Source File
# Begin Source File

SOURCE=.\MParam.h
# End Source File
# Begin Source File

SOURCE=.\MPoint.h
# End Source File
# Begin Source File

SOURCE=..\SERIAL\MsgItem.h
# End Source File
# Begin Source File

SOURCE=.\MsgItemProcessor.h
# End Source File
# Begin Source File

SOURCE=..\SRC\MYEDIT.H
# End Source File
# Begin Source File

SOURCE=..\SRC\MyTextButton.h
# End Source File
# Begin Source File

SOURCE=.\NavigationPanel.h
# End Source File
# Begin Source File

SOURCE=..\128\CAS\NCas.h
# End Source File
# Begin Source File

SOURCE=..\LOG\noticedlg.h
# End Source File
# Begin Source File

SOURCE=..\SERIAL\NThread.h
# End Source File
# Begin Source File

SOURCE=..\Lib3D\Object3d.h
# End Source File
# Begin Source File

SOURCE=..\128\GEM\ObjectsContainer.h
# End Source File
# Begin Source File

SOURCE=..\413\OceanOp\OceanBase.h
# End Source File
# Begin Source File

SOURCE=..\413\OceanOp\OceanOpNIRQuest.h
# End Source File
# Begin Source File

SOURCE=.\PZT\OLDAAPI.H
# End Source File
# Begin Source File

SOURCE=.\PZT\OLDADEFS.H
# End Source File
# Begin Source File

SOURCE=.\PZT\OLERRORS.H
# End Source File
# Begin Source File

SOURCE=.\PZT\Olmem.h
# End Source File
# Begin Source File

SOURCE=..\128\GEM\OLnClt.h
# End Source File
# Begin Source File

SOURCE=.\OLnSvrObj.h
# End Source File
# Begin Source File

SOURCE=.\PZT\OLTYPES.H
# End Source File
# Begin Source File

SOURCE=.\OnlineAlarmDlg.h
# End Source File
# Begin Source File

SOURCE=.\OnlineCEIDDlg.h
# End Source File
# Begin Source File

SOURCE=.\OnlineCJDlg.h
# End Source File
# Begin Source File

SOURCE=.\OnLineDlg.h
# End Source File
# Begin Source File

SOURCE=.\OnlineECIDDlg.h
# End Source File
# Begin Source File

SOURCE=.\OnlineEventsDlg.h
# End Source File
# Begin Source File

SOURCE=.\OnlineHSMSDlg.h
# End Source File
# Begin Source File

SOURCE=.\OnlineLogsDlg.h
# End Source File
# Begin Source File

SOURCE=.\OnlineMgr.h
# End Source File
# Begin Source File

SOURCE=.\OnlineMgrB.h
# End Source File
# Begin Source File

SOURCE=.\OnlineModeDlg.h
# End Source File
# Begin Source File

SOURCE=.\OnlinePJDlg.h
# End Source File
# Begin Source File

SOURCE=.\OnlineSubstrateDlg.h
# End Source File
# Begin Source File

SOURCE=.\OnlineSVIDDlg.h
# End Source File
# Begin Source File

SOURCE=..\413\OceanOp\OOIDrv32.h
# End Source File
# Begin Source File

SOURCE=.\OptimizeEchoDlg.h
# End Source File
# Begin Source File

SOURCE=..\ONLINE\GEM\OSSBASE.H
# End Source File
# Begin Source File

SOURCE=..\IPC\OSSMemS.h
# End Source File
# Begin Source File

SOURCE=.\PackedDIB1.h
# End Source File
# Begin Source File

SOURCE=..\ONLINE\MFC\ParaEditDlg.h
# End Source File
# Begin Source File

SOURCE=.\Pattern.h
# End Source File
# Begin Source File

SOURCE=.\PatternProp.h
# End Source File
# Begin Source File

SOURCE=.\PattMatch.h
# End Source File
# Begin Source File

SOURCE=.\PeakControl.h
# End Source File
# Begin Source File

SOURCE=.\PeakControlItem.h
# End Source File
# Begin Source File

SOURCE=.\PeakParam.h
# End Source File
# Begin Source File

SOURCE=..\413\EchoScope\Peaks.h
# End Source File
# Begin Source File

SOURCE=.\PeakShape.h
# End Source File
# Begin Source File

SOURCE=.\PersistFrameImpl.h
# End Source File
# Begin Source File

SOURCE=.\PersistFrames.h
# End Source File
# Begin Source File

SOURCE=.\PersistSDIFrame.h
# End Source File
# Begin Source File

SOURCE=.\PolyParams.h
# End Source File
# Begin Source File

SOURCE=..\128\GEM\PortParam.h
# End Source File
# Begin Source File

SOURCE=.\PosHt.h
# End Source File
# Begin Source File

SOURCE=.\PProfile.h
# End Source File
# Begin Source File

SOURCE=..\FEM\Rbt\PriUnit.h
# End Source File
# Begin Source File

SOURCE=..\ONLINE\GEM\PRJOB.H
# End Source File
# Begin Source File

SOURCE=..\128\GEM\PrJobList.h
# End Source File
# Begin Source File

SOURCE=..\ONLINE\GEM\PRJobMgr.h
# End Source File
# Begin Source File

SOURCE=.\ProbeDistParam.h
# End Source File
# Begin Source File

SOURCE=.\ProbeTest.h
# End Source File
# Begin Source File

SOURCE=.\ProcJob413ProcessingDlg.h
# End Source File
# Begin Source File

SOURCE=.\ProcJobCancelCarrierDlg.h
# End Source File
# Begin Source File

SOURCE=.\ProcJobCommon.h
# End Source File
# Begin Source File

SOURCE=.\ProcJobCompleteDlg.h
# End Source File
# Begin Source File

SOURCE=.\ProcJobCompleteManual.h
# End Source File
# Begin Source File

SOURCE=.\ProcJobDlg.h
# End Source File
# Begin Source File

SOURCE=.\ProcJobInfo.h
# End Source File
# Begin Source File

SOURCE=.\ProcJobManageDlg.h
# End Source File
# Begin Source File

SOURCE=.\ProcJobManDlg.h
# End Source File
# Begin Source File

SOURCE=.\ProcJobManual.h
# End Source File
# Begin Source File

SOURCE=.\ProcJobProcessingB.h
# End Source File
# Begin Source File

SOURCE=.\ProcJobPWC1DlgBase.h
# End Source File
# Begin Source File

SOURCE=.\ProcJobPWC2Dlg.h
# End Source File
# Begin Source File

SOURCE=.\ProcJobPWC413Dlg.h
# End Source File
# Begin Source File

SOURCE=.\ProcJobPWCRDlg.h
# End Source File
# Begin Source File

SOURCE=.\ProcJobQue413Dlg.h
# End Source File
# Begin Source File

SOURCE=.\ProcJobSMICDlg.h
# End Source File
# Begin Source File

SOURCE=.\ProcJobUnloadCarrierDlg.h
# End Source File
# Begin Source File

SOURCE=..\SRC\Procss.h
# End Source File
# Begin Source File

SOURCE=..\PropList\PropertyList.h
# End Source File
# Begin Source File

SOURCE=..\ONLINE\ShareMemory\PtrListEx.h
# End Source File
# Begin Source File

SOURCE=..\FEM\Ldp\QueOutMgr.h
# End Source File
# Begin Source File

SOURCE=..\SERIAL\Queue.h
# End Source File
# Begin Source File

SOURCE=.\QuickRecipeEditDlg.h
# End Source File
# Begin Source File

SOURCE=..\MFC\RangeDlg.h
# End Source File
# Begin Source File

SOURCE=..\FEM\Rbt\RbtBase.h
# End Source File
# Begin Source File

SOURCE=..\FEM\Rbt\RbtMgr.h
# End Source File
# Begin Source File

SOURCE=..\FEM\Rbt\RbtUnit.h
# End Source File
# Begin Source File

SOURCE=..\ONLINE\ShareMemory\ReadWriteHead.h
# End Source File
# Begin Source File

SOURCE=.\Recipe413.h
# End Source File
# Begin Source File

SOURCE=.\Recipe413NSMgr.h
# End Source File
# Begin Source File

SOURCE=.\RecipeCreateDlg413.h
# End Source File
# Begin Source File

SOURCE=.\RecipeCreateDlgBase.h
# End Source File
# Begin Source File

SOURCE=.\RecipeIO.h
# End Source File
# Begin Source File

SOURCE=.\RecipeManageDlg.h
# End Source File
# Begin Source File

SOURCE=.\RecoveryDlg.h
# End Source File
# Begin Source File

SOURCE=..\SRC\ResizableDialog.h
# End Source File
# Begin Source File

SOURCE=.\ResizableFormView.h
# End Source File
# Begin Source File

SOURCE=..\SRC\ResizableGrip.h
# End Source File
# Begin Source File

SOURCE=..\SRC\ResizableLayout.h
# End Source File
# Begin Source File

SOURCE=..\SRC\ResizableMinMax.h
# End Source File
# Begin Source File

SOURCE=..\SRC\ResizableState.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\RGB3.h
# End Source File
# Begin Source File

SOURCE=.\PZT\Roll.h
# End Source File
# Begin Source File

SOURCE=.\RunRecipe.h
# End Source File
# Begin Source File

SOURCE=..\ONLINE\SECS\S2F23.h
# End Source File
# Begin Source File

SOURCE=..\ONLINE\SECS\S2F31.h
# End Source File
# Begin Source File

SOURCE=..\ONLINE\SECS\S2F41.h
# End Source File
# Begin Source File

SOURCE=..\ONLINE\SECS\S2Msg.h
# End Source File
# Begin Source File

SOURCE=..\IPC\SArams.h
# End Source File
# Begin Source File

SOURCE=..\413\EchoScope\ScanLine.h
# End Source File
# Begin Source File

SOURCE=.\ScanLineChart.h
# End Source File
# Begin Source File

SOURCE=..\413\EchoScope\ScanPoint.h
# End Source File
# Begin Source File

SOURCE=..\ONLINE\INC\Secshead.h
# End Source File
# Begin Source File

SOURCE=..\ONLINE\GEM\SecsItem.h
# End Source File
# Begin Source File

SOURCE=..\ONLINE\GEM\SecsMessage.h
# End Source File
# Begin Source File

SOURCE=.\SecsParam.h
# End Source File
# Begin Source File

SOURCE=.\SecsParamItem.h
# End Source File
# Begin Source File

SOURCE=..\IFC\SecsSvrF.h
# End Source File
# Begin Source File

SOURCE=.\SelectDlg.h
# End Source File
# Begin Source File

SOURCE=.\SelectMeasurementDlg.h
# End Source File
# Begin Source File

SOURCE=..\ONLINE\TCPIP\SelectMode.h
# End Source File
# Begin Source File

SOURCE=.\SelectResol.h
# End Source File
# Begin Source File

SOURCE=.\SelectVCamera.h
# End Source File
# Begin Source File

SOURCE=..\FEM\Rbt\selport.h
# End Source File
# Begin Source File

SOURCE=.\SelScanNumDlg.h
# End Source File
# Begin Source File

SOURCE=..\SERIAL\SerialICC.h
# End Source File
# Begin Source File

SOURCE=..\SERIAL\SerialTh.h
# End Source File
# Begin Source File

SOURCE=.\SetupProb4Dlg.h
# End Source File
# Begin Source File

SOURCE=.\SetupProp10Dlg.h
# End Source File
# Begin Source File

SOURCE=.\SetupProp11Dlg.h
# End Source File
# Begin Source File

SOURCE=.\SetupProp12Dlg.h
# End Source File
# Begin Source File

SOURCE=.\SetupProp13Dlg.h
# End Source File
# Begin Source File

SOURCE=.\SetupProp1Dlg.h
# End Source File
# Begin Source File

SOURCE=.\SetupProp2Dlg.h
# End Source File
# Begin Source File

SOURCE=.\SetupProp31Dlg.h
# End Source File
# Begin Source File

SOURCE=.\SetupProp3Dlg.h
# End Source File
# Begin Source File

SOURCE=.\SetupProp41Dlg.h
# End Source File
# Begin Source File

SOURCE=.\SetupProp42Dlg.h
# End Source File
# Begin Source File

SOURCE=.\SetupProp43Dlg.h
# End Source File
# Begin Source File

SOURCE=.\SetupProp51Dlg.h
# End Source File
# Begin Source File

SOURCE=.\SetupProp52Dlg.h
# End Source File
# Begin Source File

SOURCE=.\SetupProp5Dlg.h
# End Source File
# Begin Source File

SOURCE=.\SetupProp6Dlg.h
# End Source File
# Begin Source File

SOURCE=.\SetupProp71Dlg.h
# End Source File
# Begin Source File

SOURCE=.\SetupProp72Dlg.h
# End Source File
# Begin Source File

SOURCE=.\SetupProp73Dlg.h
# End Source File
# Begin Source File

SOURCE=.\SetupProp7Dlg.h
# End Source File
# Begin Source File

SOURCE=.\SetupProp81Dlg.h
# End Source File
# Begin Source File

SOURCE=.\SetupProp82Dlg.h
# End Source File
# Begin Source File

SOURCE=.\SetupProp83Dlg.h
# End Source File
# Begin Source File

SOURCE=.\SetupProp84Dlg.h
# End Source File
# Begin Source File

SOURCE=.\SetupProp8Dlg.h
# End Source File
# Begin Source File

SOURCE=.\SetupProp91Dlg.h
# End Source File
# Begin Source File

SOURCE=.\SetupProp9Dlg.h
# End Source File
# Begin Source File

SOURCE=.\SetupPropComm.h
# End Source File
# Begin Source File

SOURCE=..\ONLINE\ShareMemory\SharedMemQue.h
# End Source File
# Begin Source File

SOURCE=..\ONLINE\ShareMemory\ShareMemory.h
# End Source File
# Begin Source File

SOURCE=..\ONLINE\ShareMemory\ShareQue.h
# End Source File
# Begin Source File

SOURCE=..\413\SIBase.h
# End Source File
# Begin Source File

SOURCE=.\SICam.h
# End Source File
# Begin Source File

SOURCE=.\SICamPoints.h
# End Source File
# Begin Source File

SOURCE=.\SiCamSettingDlg.h
# End Source File
# Begin Source File

SOURCE=.\sicap.h
# End Source File
# Begin Source File

SOURCE=.\sidefs.h
# End Source File
# Begin Source File

SOURCE=.\siipl.h
# End Source File
# Begin Source File

SOURCE=..\413\SIMotor.h
# End Source File
# Begin Source File

SOURCE=.\SimulatePeaks.h
# End Source File
# Begin Source File

SOURCE=.\SimuSpecDlg.h
# End Source File
# Begin Source File

SOURCE=..\SRC\sinstance.h
# End Source File
# Begin Source File

SOURCE=.\sirun.h
# End Source File
# Begin Source File

SOURCE=.\siusb.h
# End Source File
# Begin Source File

SOURCE=..\IPC\SLPInit.h
# End Source File
# Begin Source File

SOURCE=..\IPC\SMCHdr.h
# End Source File
# Begin Source File

SOURCE=..\IPC\SMClt.h
# End Source File
# Begin Source File

SOURCE=..\IPC\SMHdr.h
# End Source File
# Begin Source File

SOURCE=..\IPC\SMIpc.h
# End Source File
# Begin Source File

SOURCE=..\IPC\Smol.h
# End Source File
# Begin Source File

SOURCE=..\IPC\SMSnd.h
# End Source File
# Begin Source File

SOURCE=..\IPC\SMSvr.h
# End Source File
# Begin Source File

SOURCE=..\SRC\SortHeaderCtrl.h
# End Source File
# Begin Source File

SOURCE=..\SRC\SortListCtrl.h
# End Source File
# Begin Source File

SOURCE=..\413\OceanOp\Spec.h
# End Source File
# Begin Source File

SOURCE=.\SpecPar.h
# End Source File
# Begin Source File

SOURCE=.\SpecPlot.h
# End Source File
# Begin Source File

SOURCE=..\413\OceanOp\SpecPoint.h
# End Source File
# Begin Source File

SOURCE=..\ONLINE\MFC\SpoolMan.h
# End Source File
# Begin Source File

SOURCE=.\ST_SplitterWnd.h
# End Source File
# Begin Source File

SOURCE=..\128\GEM\StateModels.h
# End Source File
# Begin Source File

SOURCE=.\StatusReq.h
# End Source File
# Begin Source File

SOURCE=..\IPC\StdAction.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\StopDlg.h
# End Source File
# Begin Source File

SOURCE=..\TCP\StreamSocket.h
# End Source File
# Begin Source File

SOURCE=.\strSOp.h
# End Source File
# Begin Source File

SOURCE=..\IPC\SubstObj.h
# End Source File
# Begin Source File

SOURCE=..\ONLINE\GEM\SVID_ITEM.h
# End Source File
# Begin Source File

SOURCE=..\ONLINE\GEM\SVID_LIST.h
# End Source File
# Begin Source File

SOURCE=..\IPC\SvrThread.h
# End Source File
# Begin Source File

SOURCE=..\128\CAS\SysInfo.h
# End Source File
# Begin Source File

SOURCE=.\System413Dlg.h
# End Source File
# Begin Source File

SOURCE=.\SystemLTowerDlg.h
# End Source File
# Begin Source File

SOURCE=.\SystemRobotDlg.h
# End Source File
# Begin Source File

SOURCE=.\SystemStatusDlg.h
# End Source File
# Begin Source File

SOURCE=..\LTW\TCtrlBase.h
# End Source File
# Begin Source File

SOURCE=..\LTW\TCtrlBrk.h
# End Source File
# Begin Source File

SOURCE=..\LTW\TCtrlDB.h
# End Source File
# Begin Source File

SOURCE=..\LTW\TCtrlDBNet.h
# End Source File
# Begin Source File

SOURCE=..\LTW\TCtrlPatLitSerial.h
# End Source File
# Begin Source File

SOURCE=..\IPC\TelnetClientSocket.h
# End Source File
# Begin Source File

SOURCE=..\SRC\TextFile.h
# End Source File
# Begin Source File

SOURCE=.\TFTestDlg.h
# End Source File
# Begin Source File

SOURCE=.\ThinFilmProp.h
# End Source File
# Begin Source File

SOURCE=.\ThkValues.h
# End Source File
# Begin Source File

SOURCE=..\SRC\TimerMGR.h
# End Source File
# Begin Source File

SOURCE=..\SRC\Tips.h
# End Source File
# Begin Source File

SOURCE=.\TitlePanel.h
# End Source File
# Begin Source File

SOURCE=..\SRC\Tokenizer.h
# End Source File
# Begin Source File

SOURCE=..\SYS\Tool.h
# End Source File
# Begin Source File

SOURCE=..\IPC\Toolvar.h
# End Source File
# Begin Source File

SOURCE=..\ONLINE\GEM\TracTimer.h
# End Source File
# Begin Source File

SOURCE=.\tRGB.h
# End Source File
# Begin Source File

SOURCE=..\413\TxR.h
# End Source File
# Begin Source File

SOURCE=..\ONLINE\MFC\ur_resource.h
# End Source File
# Begin Source File

SOURCE=..\SYS\Var.h
# End Source File
# Begin Source File

SOURCE=..\ONLINE\MFC\vardisp.h
# End Source File
# Begin Source File

SOURCE=.\ViewBaseline.h
# End Source File
# Begin Source File

SOURCE=.\ViewCassDlg.h
# End Source File
# Begin Source File

SOURCE=.\ViewContentDlg413.h
# End Source File
# Begin Source File

SOURCE=.\ViewContentDlgBase.h
# End Source File
# Begin Source File

SOURCE=.\ViewData.h
# End Source File
# Begin Source File

SOURCE=.\ViewFileDlg.h
# End Source File
# Begin Source File

SOURCE=..\128\CAS\WaferInfo.h
# End Source File
# Begin Source File

SOURCE=..\128\CAS\WaferInfoMgr.h
# End Source File
# Begin Source File

SOURCE=.\WaferMap.h
# End Source File
# Begin Source File

SOURCE=.\WaferMapDlg.h
# End Source File
# Begin Source File

SOURCE=.\WaferMapSetting.h
# End Source File
# Begin Source File

SOURCE=.\WaferViewOptions.h
# End Source File
# Begin Source File

SOURCE=..\ONLINE\ShareMemory\WaitMsgList.h
# End Source File
# Begin Source File

SOURCE=..\LOG\WarningDlg.h
# End Source File
# Begin Source File

SOURCE=.\webbrowser2.h
# End Source File
# Begin Source File

SOURCE=.\WelcomeDlg.h
# End Source File
# Begin Source File

SOURCE=..\SRC\xSkinButton.h
# End Source File
# Begin Source File

SOURCE=.\XTabCtrl.h
# End Source File
# Begin Source File

SOURCE=.\XYPair.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\amberoff.bmp
# End Source File
# Begin Source File

SOURCE=.\res\amberon.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bitmap1.bmp
# End Source File
# Begin Source File

SOURCE=.\res\blueoff.bmp
# End Source File
# Begin Source File

SOURCE=.\res\blueon.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bmp00001.bmp
# End Source File
# Begin Source File

SOURCE=.\res\centering.bmp
# End Source File
# Begin Source File

SOURCE=.\res\checkboxes.bmp
# End Source File
# Begin Source File

SOURCE=..\PropList\checkboxes.bmp
# End Source File
# Begin Source File

SOURCE=.\res\detail.ico
# End Source File
# Begin Source File

SOURCE=.\res\E95.ico
# End Source File
# Begin Source File

SOURCE=.\res\E95.rc2
# End Source File
# Begin Source File

SOURCE=.\res\E95Doc.ico
# End Source File
# Begin Source File

SOURCE=.\res\greenoff.bmp
# End Source File
# Begin Source File

SOURCE=.\res\greenon.bmp
# End Source File
# Begin Source File

SOURCE=.\res\HOUR.BMP
# End Source File
# Begin Source File

SOURCE=.\res\icon.ico
# End Source File
# Begin Source File

SOURCE=.\res\leds.bmp
# End Source File
# Begin Source File

SOURCE=.\res\list.ico
# End Source File
# Begin Source File

SOURCE=.\res\mask.bmp
# End Source File
# Begin Source File

SOURCE=.\res\Min.bmp
# End Source File
# Begin Source File

SOURCE=.\res\ParaDesc.bmp
# End Source File
# Begin Source File

SOURCE=.\res\ParaMax.bmp
# End Source File
# Begin Source File

SOURCE=.\res\PARARANGE.bmp
# End Source File
# Begin Source File

SOURCE=.\res\redoff.bmp
# End Source File
# Begin Source File

SOURCE=.\res\redon.bmp
# End Source File
# Begin Source File

SOURCE=.\res\Toolbar.bmp
# End Source File
# Begin Source File

SOURCE=.\res\tree_bmp.bmp
# End Source File
# Begin Source File

SOURCE=.\res\YEAR.BMP
# End Source File
# Begin Source File

SOURCE=.\res\zoom.cur
# End Source File
# Begin Source File

SOURCE=.\res\zoom1.cur
# End Source File
# Begin Source File

SOURCE=.\res\zoom2.cur
# End Source File
# Begin Source File

SOURCE=.\res\zoom3.cur
# End Source File
# End Group
# Begin Source File

SOURCE="..\Change List.doc"
# End Source File
# Begin Source File

SOURCE="..\ONLINE\HSMS Protocol.txt"
# End Source File
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# Begin Source File

SOURCE=..\ToDo.txt
# End Source File
# Begin Source File

SOURCE=.\siusb.lib
# End Source File
# Begin Source File

SOURCE=.\sicap.lib
# End Source File
# Begin Source File

SOURCE=.\siipl.lib
# End Source File
# Begin Source File

SOURCE=..\413\DMCMLIB.lib
# End Source File
# Begin Source File

SOURCE=..\413\DMC32.lib
# End Source File
# Begin Source File

SOURCE=..\413\EchoScope\AqDrv4.lib
# End Source File
# Begin Source File

SOURCE=..\413\EchoScope\cbw32.lib
# End Source File
# Begin Source File

SOURCE=..\413\OceanOp\OOI_HOME\OmniDriver32.lib
# End Source File
# Begin Source File

SOURCE=..\413\OceanOp\OOI_HOME\common32.lib
# End Source File
# Begin Source File

SOURCE=..\413\Lucam\lucamapi.lib
# End Source File
# End Target
# End Project
# Section E95 : {8856F961-340A-11D0-A96B-00C04FD705A2}
# 	2:21:DefaultSinkHeaderFile:webbrowser2.h
# 	2:16:DefaultSinkClass:CWebBrowser2
# End Section
# Section E95 : {25959BED-E700-11D2-A7AF-00C04F806200}
# 	2:5:Class:CMsie
# 	2:10:HeaderFile:msie.h
# 	2:8:ImplFile:msie.cpp
# End Section
# Section E95 : {1339B54C-3453-11D2-93B9-000000000000}
# 	2:21:DefaultSinkHeaderFile:webbrowser2.h
# 	2:16:DefaultSinkClass:CWebBrowser2
# End Section
# Section E95 : {CA8A9780-280D-11CF-A24D-444553540000}
# 	2:21:DefaultSinkHeaderFile:acroaxdocshim.h
# 	2:16:DefaultSinkClass:CAcroAXDocShim
# End Section
