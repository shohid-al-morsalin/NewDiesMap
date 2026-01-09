#pragma once
/*
//20210316/ZHIMING CHANGED CONTRAST STRETCH
//20210423/ZHIMING ELLIMINATE NOISE OF FIRST TRANSITION
//20220616/HAQUE UPDATE BEVEL WIDTH MEASUREMENT ALGORITHM FOR VERY NARROW BEVEL
//20221110/ZHIMING ADDED CONDITION FOR DOUBLE ENTRANCE TO LATCH FOUP CAUSE LATCHTIMEOUT IN AUTO MODE

//20221108/Sabbir/yukchiu Added code for confocal integration
//11222022/ZHIMING ADDED CODE FOR CONFOCAL MEASUREMENT ROUTINE
//11252022/ZHIMING ADDED CONDITION FOR CONFOCAL WARP MEASUREMENT
//20231227 / TUSHAR ADDED CODE FOR CONFOCAL TOTAL THICKNESS MEASUREMENT
//01312024/yukchiu/add setteling time for confocal measurement
//03202024 / MAHEDI added roughness measurement
//04182024 / TUSHAR / Implemented feature to turn on vacuum by wafer size automatically
//07102024 / MAHEDI added Ra/Rms option for roughness in recipe
//02082024/MORSALIN/Probe Switching
//08092024/ MAHEDI / Added tab for Ring width(Taiko)
//20240820/ Arup /operation for 13 slot
//20240814 / MASUM / Added FFU Galil I/O
//20243008/Masum/Changes for new requirement on foup 13 slot
//08302024/Masum/Secs/Gem 13 Slots
//08132024/ MAHEDI / added Ring width (Taiko) probe structure
//08142024/MORTUJA/ADD RING WIDTH MEASUREMENT
//21082024/Tushar/Added Logosol Pre-Aligner
//09092024/Tushar/Added door interlock safety for logosol pre-aligner
//10/3/24 /Masum /temporary bypassed auth
//031024/Mortuja/Add Add Offset With Result
//20240726/ ALVI/	VIEW ERROR FIXED
//10152024 / MAHEDI KAMAL / BUMP with confocal 
//16102024/MORTUJA/Modify Microscope And Spectroscope Light Source I/O
//10172024 / MAHEDI KAMAL / added RS232 communication for Confocal / Probe switching update by MORSALIN
//10182024 / MAHEDI KAMAL / Added 2nd CWL class
//24102024/MORSALIN / LTV, STIR, TIR probe
//06112024/MORSALIN / Added CWL Z-axis and modify Microscope Z-axis
//12120224/MAHEDI KAMAL + ARIF/ adding Roughness with confocal
//23102024/TUSHAR / CONFOCAL DUAL WARP Added
//20241217/ Sihab / Fixed wafer map numbering in confocal tab
//12172024 / Tushar / added condition for range & TTV in ViewContentDlg413
// 12262024 / Mahedi Kamal / monitor for 040, New filter for roughness
//30122024_1 / Tushar / Added Title in 3D map
//MORSALIN//01012025 Remove extra probe from help
//01022025 / Tushar / Fixed Mark button failure in Motion control
//MORSALIN//01022024 / Confocal tab "Z Pos THK" and "Z Pos Rough" btn updated
//13012025 /Tushar / Added wafer dice number show using tooltip
//27022025 /Tushar/ Dual probe bow warp bug fix.
//20250225/Tushar/ Skiping Bad data in normalization in dual probe warp measurement.
//03052025 /Tushar/ search around trial number read for Thickness and warp from INI file .
//03062025 /Tushar/ On closing XY stage move to original position feature, On start reset all feature added
//20250306 /ARIF / Roughness unit in nm and angstrom
//10032025 /Tushar / Added auto measurement set 1 selection while adding point
//10032025_2 / Tushar / Disabled Z hieght in Point list
//10032025_3//Tushar/ Added search around number for dual warp, thickness measurement from UI and recipe
// 03132025_1/ Tushar / added new measuremt path for Bow War + Thickness , modified search around for Bow War & Thickness
//03142025_1/ Tushar / fixed wafer map not updating issue after  using location button in dialog 14.
//03142025_2/ Tushar / Added menu and option for saving bavel image in trim tab.
//03142025_3/ Tushar / Added auto measurement set 1 selection while creating points for LTV-STIR Create points .
//03172025/ Tushar / added countious thickness showing
//20250317_1/MASRUR/ ADDED DOUBLE DATA TAKING AND DISCARD INITIAL NOISE FOR TT SEARCH AROUND
//20250317_2/MASRUR/ ADDED FOR ROUGHNESS MEASUREMENT STABILITY
//03182025/ Tushar / Added Confocal calibration auto foucs.
//03182025_1/ Tushar / added user confirmation for reset stage for reset all.
//03192025/Tushar / Added Check box in UI and function for calibration auto foucus uptp 4 decimal point for 003
//20250319// MASRUR// BYPASS FOR ROUGHNESS
//20251320_1/MASRUR/ AVOID GO TO LOAD POS
//20250320_2/MASRUR/ NEW RESET FOR TOPZ CWL AND SET SPEED 4 FOR TOPZ CWL
//20250811/Shamim/ Changed the Y-direction sign
//20250513/ ARIF/ UI and Recipe conflict of roughness NM/A unit change issue solved.
//20250812/Sihab/Solve CWL Bot Probe Value not showing under probe 55
//08152025 / MAHEDI / Added wafer repaet count status and do confocal initialization according to it
//// 04092025 / Tushar / modification to  use same data for total, top, bottom.
//09232025 / MAHEDI / line scan averaging option for CWl thickness mes and warp
//23092025 / MAHEDI / GET CWL moving avg
//09232025_Shamim / Shamim / Added Thickness fast scan
//09242025 / MAHEDI /  save data while thickness line scan, and add LSQ
//20250926_Mohir/ Mohir / Added a thicknessScanLength on Confocal tab and configure it to read/write in INI file...
//09292025/ Saving TTK value of each mes
//10082025/ Reducing TTK line scan measurement time
//10102025_1/ Tushar / use same line scan data  for top, bot thk, warp
//20251013_Mohir/Mohir/Added export file functionalities while run recipe from job same as winbond software.
//20251014_Mohir/Mohir/Added wafer size, type, station on job process manual UI and export on csv file when run a recipe
//10122024 / Tushar / Dice map for LTV STIR 
//10132025_1 / Tushar / Enlarge functionalities added for Dicemap 
//10132025_MAHEDI check if Any thickness Mesasurement is there, then to remove repetitive move we check this variable
// 10202025_1 MORSALIN always check on for lsf
// 10202025 MORSALIN add fitted median value after solve tilt issue
//21102025_1 / Tushar / 2D map auto save after each measurement .
//20251022_Mohir/ Mohir/ Added wafer type,size and station in data export file when run recipe from job
// 10222025 MORSALIN add LTV min, max and avg
// 10232025 MORSALIN apply lsf on final LTV, STIR result
// 10292025_MH // roughness Out of range value check
//  10292025 MORSALIN Make All Scan data positive
// 10312025 MORSALIN adjust SD for LTV-STIR
// 10312025_1 MORSALIN add checkbox for min, max, avg, LTV and STIR
//20251030_Mohir / Mohir / Added two parameter Start and end time on export file when measuring from job
// 10242025_M // MAHEDI // read scan len from ini, focus 000 issue, calibratiom wron pos issue
// 11042025 MORSALIN modify some UI on Motion Control and add "Enable/Disable Modify" btn
//20251011_Mohir/Mohir/Added export pdf functionalites when measurement from job 
 // 11102025 MORSALIN enable 3x3 die
 // 12012025/HAQUE/Die Map deley solved
// 20112025_Tushar // Tushar // Stoped CWL reset in SW start and measurement start to remove offset of 1st repeat measurement
// 12112025_Tushar // Tushar // Modified code to make interpolation same for 2D 3D
// 12112025_Mahedi / Wafer Map percentage issue
//20251215_Mohir/ Mohir // Added PDF all functionalites for automatic export..
//20251219_Mohir/ Mohir // Added functionalites for closing csv
//20251219_Mahmudul / Added Dicemap to pdf that auto generate while run recipe from job.
 //20260101 Mahmudul / Added New Dicemap tab(IDD_SETUPPROP92) and DiceMap class 
NOTE:
===================================================
Echo					: IDD_SETUPPROP3
MeSets					: IDD_SETUPPROP7
Echo Setting			: IDD_SETUPPROP72
Bow Warp				: IDD_SETUPPROP91
Stress					: IDD_SETUPPROP71
Trim					: IDD_SETUPPROP81
Trim Setting			: IDD_SETUPPROP83
Thin Film				: IDD_SETUPPROP4
Thin Film Setting		: IDD_SETUPPROP43
Positions				: IDD_SETUPPROP73
Patterns				: IDD_SETUPPROP5
Matching Setting		: IDD_SETUPPROP52
Pattern Matching		: IDD_SETUPPROP51
SPC						: IDD_SETUPPROP2
Motor Setting			: IDD_SETUPPROP61
Screen Setting			: IDD_SETUPPROP8
HSMS					: IDD_ONLINEHSMSDLG
System Setting			: IDD_SETUPPROP9
Check Stage				: IDD_SETUPPROP12
Center Stage			: IDD_SETUPPROP1
Wafer Transfer			: IDD_SETUPPROP10
Stage Setting			: IDD_SETUPPROP11
About					: IDD_SETUPPROP82
Wafer Align				: IDD_SETUPPROP84
Camera Analyzer			: IDD_SETUPPROP41
Thin Film Analyzer		: IDD_SETUPPROP42
Echo Analyzer			: IDD_SETUPPROP31
Edge Analyzer			: IDD_SETUPPROP13
Confocal				: IDD_SETUPPROP14
Roughness				: IDD_SETUPPROP15
Ring Width (Taiko)  	: IDD_SETUPPROP16
DiceMap					: IDD_SETUPPROP92

Online Status			: IDD_ONLINEDLG
Load Port #%d Status	: IDD_LOADPORT
Equipment Status		: IDD_SYSTEMSTATUSDLG
413 Module Status		: IDD_SYSTEM413DLG
Robot Status			: IDD_SYSTEMROBOTDLG
Light Tower Status		: IDD_SYSTEMLTOWERDLG

Load Port #%d			: IDD_PROCESSJOB

===================================================

*/