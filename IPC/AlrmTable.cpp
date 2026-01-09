//////////////////////////////////////////////////////////////////////
// Author: (insert you name here)
// Version: 0.0
// Date: dd/mm/yy
// Short Description: (insert a short description, one line)
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

// Alarms table

const Alrm alarm[] = 
{
	Alrm::aNONE, Alrm::CONNECTTOSCALEFAILURE, "Connect to scale failure", 2501,
	Alrm::aNONE, Alrm::READFROMSCALEFAILURE, "Read from scale failure", 2502,

	Alrm::aNONE, Alrm::LP1DUPLICATEDCARRIERID, "Duplicated carrier error", 2506,
	Alrm::aNONE, Alrm::LP2DUPLICATEDCARRIERID, "Duplicated carrier error", 2507,
	Alrm::aNONE, Alrm::CIDR1COMMFAILURE, "CIDR 1 communication failure", 2522,
    Alrm::aNONE, Alrm::CIDR1READFAILURE, "CIDR 1 read operation failure", 2523,
	Alrm::aNONE, Alrm::CIDR2COMMFAILURE, "CIDR 2 communication failure", 2524,
    Alrm::aNONE, Alrm::CIDR2READFAILURE, "CIDR 2 read operation failure", 2525,
	
	Alrm::aNONE, Alrm::LP1OPENFAILURE, "Load port 1 open failure", 2529,
	Alrm::aNONE, Alrm::LP2OPENFAILURE, "Load port 2 open failure", 2530,
	Alrm::aNONE, Alrm::LP3OPENFAILURE, "Load port 3 open failure", 2531,
	Alrm::aNONE, Alrm::LP4OPENFAILURE, "Load port 4 open failure", 2532,
	Alrm::aNONE, Alrm::LP1CLOSEFAILURE, "Load port 1 close failure", 2533,
	Alrm::aNONE, Alrm::LP2CLOSEFAILURE, "Load port 2 close failure", 1534,
	Alrm::aNONE, Alrm::LP3CLOSEFAILURE, "Load port 3 close failure", 2535,
	Alrm::aNONE, Alrm::LP4CLOSEFAILURE, "Load port 4 close failure", 2536,

	Alrm::aNONE, Alrm::PIOFAILURE, "PIO failure", 2514,

//	Alrm::aNONE, Alrm::ACCESSMODEVIOLATION, "Access mode violation", 2515,
	Alrm::aNONE, Alrm::OUTOFSERVICELPUSED, "Attempt to use out of service load port", 2517,
	Alrm::aNONE, Alrm::CARRIERPRESENCEERROR, "Carrier presence error", 2504,
	Alrm::aNONE, Alrm::CARRIERPLACEMENTERROR, "Carrier placement error", 2505,
	Alrm::aNONE, Alrm::DOCKUNDOCKFAILURE, "Dock/Undock failure", 3300,
	Alrm::aNONE, Alrm::OPENCLOSEFAILURE, "Open/Close failure", 3301,
	Alrm::aNONE, Alrm::CARRIERREMOVALERROR, "Carrier removal error", 2507,

	Alrm::aNONE, Alrm::FAILTOSCAN, "Fail to scan error", 2508,
	Alrm::aNONE, Alrm::DATACOLLECTIONERROR, "Data collection failure", 2509,
	Alrm::aNONE, Alrm::DETECTORSATURATEDERROR, "Detector saturation failure", 2510,
	Alrm::aNONE, Alrm::HIGHBACKGROUNDINTENSITYERROR, "High background intensity failure", 2513,
	Alrm::aNONE, Alrm::NOLASERSIGNALERROR, "No laser signal failure", 2511,
	Alrm::aNONE, Alrm::AUTOINTENSITYERROR, "Auto intensity failure", 2512, 

	Alrm::aNONE, Alrm::LPNOTASSOCIATEDERROR, "Load port not associated failure", 2524,
	Alrm::aNONE, Alrm::CREATESUBSTRATEOBJERROR, "Create substrate object failure", 2775, 
	Alrm::aNONE, Alrm::CARRIEROBJECTNOTFOUND, "Carrier object not found failure", 2527,

	Alrm::aNONE, Alrm::IMPROPERSCENARIOERROR, "Improper scenario failure", 2537,
	Alrm::aNONE, Alrm::INSTANTIATECARRIEROBJECTFAIL, "Instantiate carrier object failure", 2538,
	Alrm::aNONE, Alrm::CLAMP1FAILURE, "Port 1 clamp failure", 2539,
	Alrm::aNONE, Alrm::CLAMP2FAILURE, "Port 2 clamp failure", 2540,
	Alrm::aNONE, Alrm::CLAMP3FAILURE, "Port 3 clamp failure", 2541,
	Alrm::aNONE, Alrm::CLAMP4FAILURE, "Port 4 clamp failure", 2542,
	Alrm::aNONE, Alrm::UNCLAMP1FAILURE, "Port 1 unclamp failure", 2543,
	Alrm::aNONE, Alrm::UNCLAMP2FAILURE, "Port 2 unclamp failure", 2544,
	Alrm::aNONE, Alrm::UNCLAMP3FAILURE, "Port 3 unclamp failure", 2545,
	Alrm::aNONE, Alrm::UNCLAMP4FAILURE, "Port 4 unclamp failure", 2546,

	Alrm::aNONE, Alrm::WAFERHANDLINGFAILURE, "Wafer handling failure", 2547,
	Alrm::aNONE, Alrm::LP1NOTREADYFAILURE, "Port 1 not ready failure", 2548,
	Alrm::aNONE, Alrm::LP2NOTREADYFAILURE, "Port 2 not ready failure", 2549,
	Alrm::aNONE, Alrm::LP3NOTREADYFAILURE, "Port 3 not ready failure", 2550,
	Alrm::aNONE, Alrm::LP4NOTREADYFAILURE, "Port 4 not ready failure", 2551,
	Alrm::aNONE, Alrm::LP1EXCESSIVEWAFEROUT, "Port 1 excessive wafer out failure", 2552,
	Alrm::aNONE, Alrm::LP2EXCESSIVEWAFEROUT, "Port 2 excessive wafer out failure", 2553,
	Alrm::aNONE, Alrm::LP3EXCESSIVEWAFEROUT, "Port 3 excessive wafer out failure", 2554,
	Alrm::aNONE, Alrm::LP4EXCESSIVEWAFEROUT, "Port 4 excessive wafer out failure", 2555,
	Alrm::aNONE, Alrm::LP1PERISENSORTRIPED, "Port 1 perimeter sensor trip failure", 2556,
	Alrm::aNONE, Alrm::LP2PERISENSORTRIPED, "Port 2 perimeter sensor trip failure", 2557,
	Alrm::aNONE, Alrm::LP3PERISENSORTRIPED, "Port 3 perimeter sensor trip failure", 2558,
	Alrm::aNONE, Alrm::LP4PERISENSORTRIPED, "Port 4 perimeter sensor trip failure",	 2559,

	Alrm::aNONE, Alrm::CARRIER1IDREADFAIL, "Port 1 Carrier ID read failure", 2560,
	Alrm::aNONE, Alrm::CARRIER2IDREADFAIL, "Port 2 Carrier ID read failure", 2561,
	Alrm::aNONE, Alrm::CARRIER3IDREADFAIL, "Port 3 Carrier ID read failure", 2562,
	Alrm::aNONE, Alrm::CARRIER4IDREADFAIL, "Port 4 Carrier ID read failure", 2563,

	Alrm::aNONE, Alrm::SLOTMAP1READFAILURE, "Slotmap 1 read failure", 2564,
	Alrm::aNONE, Alrm::SLOTMAP2READFAILURE, "Slotmap 2 read failure", 2565,
	Alrm::aNONE, Alrm::SLOTMAP3READFAILURE, "Slotmap 3 read failure", 2566,
	Alrm::aNONE, Alrm::SLOTMAP4READFAILURE, "Slotmap 4 read failure", 2567,
	Alrm::aNONE, Alrm::SLOTMAP1VERIFICATIONFAIL, "Slotmap 1 verification failure", 2568,
	Alrm::aNONE, Alrm::SLOTMAP2VERIFICATIONFAIL, "Slotmap 2 verification failure", 2569,
	Alrm::aNONE, Alrm::SLOTMAP3VERIFICATIONFAIL, "Slotmap 3 verification failure", 2570,
	Alrm::aNONE, Alrm::SLOTMAP4VERIFICATIONFAIL, "Slotmap 4 verification failure", 2571,
	
	Alrm::aNONE, Alrm::ID1VERIFICATIONFAIL, "Port 1 ID verification failure", 2572,
	Alrm::aNONE, Alrm::ID2VERIFICATIONFAIL, "Port 2 ID verification failure", 2573,
	Alrm::aNONE, Alrm::ID3VERIFICATIONFAIL, "Port 3 ID verification failure", 2574,
	Alrm::aNONE, Alrm::ID4VERIFICATIONFAIL, "Port 4 ID verification failure", 2575,

	Alrm::aNONE, Alrm::FANFILTERUNITFAIL, "Fan filter unit failure", 2576,
	Alrm::aNONE, Alrm::HOMEROBOTFAIL, "Robot home failure", 2577,

	Alrm::aNONE, Alrm::LOSTE84SIGNAL , "Lost E84 signal", 2578,
	Alrm::aNONE, Alrm::UNEXPECTEDE84SIGNAL , "Unexpected E84 signal", 2579,
	Alrm::aNONE, Alrm::LP1ALARMED , "Load port 1 alarmed", 2580,
	Alrm::aNONE, Alrm::LP2ALARMED , "Load port 2 alarmed", 3200,
	Alrm::aNONE, Alrm::LP3ALARMED , "Load port 3 alarmed", 3201,
	Alrm::aNONE, Alrm::LP4ALARMED , "Load port 4 alarmed", 3202,
	Alrm::aNONE, Alrm::LP1VACUUMLOST , "Load port 1 vacuum lost", 2581,
	Alrm::aNONE, Alrm::LP2VACUUMLOST , "Load port 2 vacuum lost", 3203,
	Alrm::aNONE, Alrm::LP3VACUUMLOST , "Load port 3 vacuum lost", 3204,
	Alrm::aNONE, Alrm::LP4VACUUMLOST , "Load port 4 vacuum lost", 3205,

	Alrm::aNONE, Alrm::LP1E84TP1TIMEOUT , "Load port 1 E84 TP1 timeout", 2582,
	Alrm::aNONE, Alrm::LP1E84TP2TIMEOUT , "Load port 1 E84 TP2 timeout", 2583,
	Alrm::aNONE, Alrm::LP1E84TP3TIMEOUT , "Load port 1 E84 TP3 timeout", 2584,
	Alrm::aNONE, Alrm::LP1E84TP4TIMEOUT , "Load port 1 E84 TP4 timeout", 2585,
	Alrm::aNONE, Alrm::LP1E84TP5TIMEOUT , "Load port 1 E84 TP5 timeout", 2586,
	Alrm::aNONE, Alrm::LP1E84TP6TIMEOUT , "Load port 1 E84 TP6 timeout", 2587,

	Alrm::aNONE, Alrm::LP2E84TP1TIMEOUT , "Load port 2 E84 TP1 timeout", 3000,
	Alrm::aNONE, Alrm::LP2E84TP2TIMEOUT , "Load port 2 E84 TP2 timeout", 3001,
	Alrm::aNONE, Alrm::LP2E84TP3TIMEOUT , "Load port 2 E84 TP3 timeout", 3002,
	Alrm::aNONE, Alrm::LP2E84TP4TIMEOUT , "Load port 2 E84 TP4 timeout", 3003,
	Alrm::aNONE, Alrm::LP2E84TP5TIMEOUT , "Load port 2 E84 TP5 timeout", 3004,
	Alrm::aNONE, Alrm::LP2E84TP6TIMEOUT , "Load port 2 E84 TP6 timeout", 3005,
	
	Alrm::aNONE, Alrm::EQPOUTOFSERVICE , "Equipment out of service", 3100,
	Alrm::aNONE, Alrm::C900OUTOFSERVICE , "900 unit out of service", 3101,
	Alrm::aNONE, Alrm::C128OUTOFSERVICE , "128 unit out of service", 3102,
	Alrm::aNONE, Alrm::ROBOTOUTOFSERVICE , "Robot out of service", 3103,
	Alrm::aNONE, Alrm::SCALEOUTOFSERVICE , "Scale out of service", 3104,
	Alrm::aNONE, Alrm::LP1OUTOFSERVICE , "Load port 1 out of service", 3105,
	Alrm::aNONE, Alrm::LP2OUTOFSERVICE , "Load port 2 out of service", 3106,
	Alrm::aNONE, Alrm::IDR1OUTOFSERVICE , "CIDR 1 out of service", 3107,
	Alrm::aNONE, Alrm::IDR2OUTOFSERVICE , "CIDR 2 out of service", 3108,

	Alrm::aNONE, Alrm::LP1NOTREADYTOLOAD , "Load port 1 not ready to load", 3109,
	Alrm::aNONE, Alrm::LP2NOTREADYTOLOAD , "Load port 2 not ready to load", 3110,

	Alrm::aNONE, Alrm::CARRIERVERIFICATIONFAIL, "Carrier verification failure", 2503, //LYF: Obsolete
    Alrm::aNONE, Alrm::SLOTMAPVERIFICATIONFAIL, "", 2516,//LYF: Obsolete
    Alrm::aNONE, Alrm::SLOTMAPREADFAIL, "", 2525,//LYF: Obsolete
    Alrm::aNONE, Alrm::DOCKFAIL, "", 2518,//LYF: Obsolete
    Alrm::aNONE, Alrm::UNDOCKFAIL, "", 2519,//LYF: Obsolete
    Alrm::aNONE, Alrm::OPENFAIL, "", 1000, //LYF: Obsolete
    Alrm::aNONE, Alrm::CLOSEFAIL, "", 2521, //LYF: Obsolete
    Alrm::aNONE, Alrm::IDVERIFICATIONFAIL, "", 2528, //LYF: Obsolete

    
	Alrm::aNONE, Alrm::LP1ACCESSMODEVIOLATION, "Load port 1 access mode violation", 4000,
	Alrm::aNONE, Alrm::LP2ACCESSMODEVIOLATION, "Load port 2 access mode violation", 4001,
	Alrm::aNONE, Alrm::LP3ACCESSMODEVIOLATION, "Load port 3 access mode violation", 4002,
	Alrm::aNONE, Alrm::LP4ACCESSMODEVIOLATION, "Load port 4 access mode violation", 4003,
	
	Alrm::aNONE, Alrm::C128NOTONLINE, "128 unit not online", 4200,
	Alrm::aNONE, Alrm::C900NOTONLINE, "900 unit not online", 4201,

	Alrm::aNONE, Alrm::LP1LOSTE84SIGNAL , "Load port 1 Lost E84 signal", 4202,
	Alrm::aNONE, Alrm::LP2LOSTE84SIGNAL , "Load port 2 Lost E84 signal", 4203,
	Alrm::aNONE, Alrm::LP1UNEXPECTEDE84SIGNAL , "Load port 1 Unexpected E84 signal", 4204,
	Alrm::aNONE, Alrm::LP2UNEXPECTEDE84SIGNAL , "Load port 2 Unexpected E84 signal", 4205,
	
	Alrm::aNONE, Alrm::ENDLIST, "", 0,
};