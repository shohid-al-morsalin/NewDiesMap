================================================================================
    MICROSOFT FOUNDATION CLASS LIBRARY : E95 Project Overview
===============================================================================

The application wizard has created this E95 application for 
you.  This application not only demonstrates the basics of using the Microsoft 
Foundation Classes but is also a starting point for writing your application.

This file contains a summary of what you will find in each of the files that
make up your E95 application.

E95.vcproj
    This is the main project file for VC++ projects generated using an application wizard. 
    It contains information about the version of Visual C++ that generated the file, and 
    information about the platforms, configurations, and project features selected with the
    application wizard.

E95.h
    This is the main header file for the application.  It includes other
    project specific headers (including Resource.h) and declares the
    CE95App application class.

E95.cpp
    This is the main application source file that contains the application
    class CE95App.

E95.rc
    This is a listing of all of the Microsoft Windows resources that the
    program uses.  It includes the icons, bitmaps, and cursors that are stored
    in the RES subdirectory.  This file can be directly edited in Microsoft
    Visual C++. Your project resources are in 1033.

res\E95.ico
    This is an icon file, which is used as the application's icon.  This
    icon is included by the main resource file E95.rc.

res\E95.rc2
    This file contains resources that are not edited by Microsoft 
    Visual C++. You should place all resources not editable by
    the resource editor in this file.
/////////////////////////////////////////////////////////////////////////////

For the main frame window:
    The project includes a standard MFC interface.
MainFrm.h, MainFrm.cpp
    These files contain the frame class CMainFrame, which is derived from
    CFrameWnd and controls all SDI frame features.
/////////////////////////////////////////////////////////////////////////////

The application wizard creates one document type and one view:

E95Doc.h, E95Doc.cpp - the document
    These files contain your CE95Doc class.  Edit these files to
    add your special document data and to implement file saving and loading
    (via CE95Doc::Serialize).
E95View.h, E95View.cpp - the view of the document
    These files contain your CE95View class.
    CE95View objects are used to view CE95Doc objects.
/////////////////////////////////////////////////////////////////////////////

Other Features:

ActiveX Controls
    The application includes support to use ActiveX controls.
/////////////////////////////////////////////////////////////////////////////

Other standard files:

StdAfx.h, StdAfx.cpp
    These files are used to build a precompiled header (PCH) file
    named E95.pch and a precompiled types file named StdAfx.obj.

Resource.h
    This is the standard header file, which defines new resource IDs.
    Microsoft Visual C++ reads and updates this file.

E95.manifest
	Application manifest files are used by Windows XP to describe an applications 
	dependency on specific versions of Side-by-Side assemblies. The loader uses this 
	information to load the appropriate assembly from the assembly cache or private 
	from the application. The Application manifest  maybe included for redistribution 
	as an external .manifest file that is installed in the same folder as the application 
	executable or it may be included in the executable in the form of a resource. 
/////////////////////////////////////////////////////////////////////////////

Other notes:

The application wizard uses "TODO:" to indicate parts of the source code you
should add to or customize.

If your application uses MFC in a shared DLL, and your application is in a 
language other than the operating system's current language, you will need 
to copy the corresponding localized resources MFC70XXX.DLL from the Microsoft
Visual C++ CD-ROM under the Win\System directory to your computer's system or 
system32 directory, and rename it to be MFCLOC.DLL.  ("XXX" stands for the 
language abbreviation.  For example, MFC70DEU.DLL contains resources 
translated to German.)  If you don't do this, some of the UI elements of 
your application will remain in the language of the operating system.

/////////////////////////////////////////////////////////////////////////////
TODO:

Recovery at start

/////////////////////////////////////////////////////////////////////////////

Comm Port assignment
Motors
X - com 1 : X motor
Y - com 2 ; Y motor
Z1 - com3 ; Echo probe
Z2 - com4 ; Thin film probe
P - com5  ; Probe switcher

SICamera requires correct lib and dll files to operate

Motor PID
P=0.13;3
I=0;0.3
D=8.75;2

KP9.25
KD75.13
KI2.23


TODO:
1. Reset Z stages at start of program
2. Robot should be homed before closing load port