#ifndef _ONLINE_EVENT_H_INCLUDED_
#define _ONLINE_EVENT_H_INCLUDED_

// CEvt::eventid
// @@VIjay The EVENT ID MUST BE same as CEID published in
// ....Documentation
// Otherwise it should be above 10000
#define	EVT_LINEPHASE_REMOTEEVT				1
#define	EVT_LINEPHASE_LOCALEVT				2
#define	EVT_LINEPHASE_OFFLINE				3
#define	EVT_LINEPHASE_HOSTOFF				4
#define	EVT_LINEPHASE_BCRON					5
#define	EVT_LINEPHASE_BCROFF				6
#define	EVT_LINEPHASE_AGVON					7
#define	EVT_LINEPHASE_AGVOFF				8
#define EVT_LINEPHASE_COMMESTABLISH         10016
#define	EVT_LINEPHASE_CHGLOCAL				10
#define EVT_LINEPHASE_CHGREMOTE			    16
#define	EVT_LINEMODE						12
#define	EVT_LINEPHASE_ATTEMPT_ONLINE        10012
#define	EVT_LINEPHASE_ATTEMPT_LOCAL		    10014
#define EVT_LINEPHASE_INIT_DATA		        10015
#define	EVT_ID10							10
#define	EVT_TRANSACTION						5000

// CEvt::make
#define     EVT_REPLY_TIMEOUT                  10013
#define     EVT_REPLY_TIMEOUT                  10013
#define	EVT_ABORT_TRANSACTION				5001
#define      EVT_MAKE10                         10019
/*******************************************************
 * CMS  EVENT
 *  - LOAD REQUEST
 *  ~
 *  - UNLOAD COMPLETE
 ******************************************************/
#define     EVT_CMS							2
#define     EVT_CMSPHASE_LOADREQUEST			1
#define     EVT_CMSPHASE_TRANSFERBLOACKED		2
#define     EVT_CMSPHASE_LOADCOMPLETE			3
#define     EVT_CMSWAITFORHOST		            4
#define     EVT_CMSPHASE_VERCARIDCOMPLETED		5
#define     EVT_CMSPHASE_DOCKINGCOMPLETED		6
#define     EVT_CMSPHASE_FOUPOPEN				7
#define     EVT_CMSPHASE_FOUPOPENFAIL		   -7

#define     EVT_CMSPHASE_SLOTMAPREADCOMPLETED	8
#define	 EVT_CMSPHASE_SLOTMAPVERIFY			9

#define     EVT_CMSPHASE_CARRIERPRCOMPLETED	10
#define     EVT_CMSPHASE_FOUPCLOSE				11
#define     EVT_CMSPHASE_UNDOCKINGCOMPLETED	12
#define     EVT_CMSPHASE_UNLOADREQUEST			13
#define     EVT_CMSPHASE_CARRIERUNCLAMPTED		14
#define     EVT_CMSPHASE_UNLOADCOMPLETE		15
#define     EVY_CMSPHASE_WAFERPICKUP			50
#define     EVT_CMSPHASE_WAFERFINISHEDMESURE	/*51*/2036
#define     EVT_CMSPHASE_WAFERRETURNBACK		52
#define     EVT_CMSCARRIERCANEL				101
#define     EVT_CMSCARRIERIDVERIFYFAIL         102
#define     EVT_CMSSLOTMAPVERIFYFAIL           103

#define     EVT_CMSREMOTESTOP                  220
#define     EVT_CMSREMOTEPAUSE                 221
#define     EVT_CMSREMOTERESUME                222
#define     EVT_CMSREMOTEABORT                 223
#define     EVT_CMSREMOTECANCEL                224

 // #define     EVT_CMSPHASE_WATINGFORHOSTSLOTMAP  300
 // #define     EVT_CMSPHASE_SLOTMAPVERIFY         301

  //
  // CMS EVENT ADD TO TTI
  //
#define     EVT_CMSPHASE_IDLE                  400
#define	 EVT_CMSPHASE_WORKING               401
#define     EVT_CMSPHASE_MAINTENANCE           402
#define     EVT_CMSPHASE_OUTOFSERVICE          403
#define     EVT_CMSPHASE_INSERVICE             404
#define     EVT_CMSPHASE_CASSETTEFILEPRESENT   405
#define     EVT_CMSPHASE_CASSETTEFILENOPRESENT 406
#define     EVT_CMSPHASE_CASSETTEPRESENT       407
#define     EVT_CMSPHASE_CASSETTENOPRESENT     408

/*******************************************************
 * PROCESS JOB MANAGENET  EVENT
 *  -
 *  ~
 *  - .
 ******************************************************/
 //  #define     EVT_PJM							3
#define     EVT_PJMCREATECOMPLETE				1
#define     EVT_PJMSTARTEDSETUP				2
#define     EVT_PJMSTARTEDPROCESSING			3
#define     EVT_PJMCPROCESSCOMPLETE			207

#define     EVT_PJMSTART						200
#define     EVT_PJMSTOP						201
#define     EVT_PJMPAUSE						202
#define     EVT_PJMRESUME						203
#define     EVT_PJMABORT						204
#define     EVT_PJMCANCEL						205
#define	 EVT_PJMWAITFORSTART				206
#define		 EVT_EQUIP_MSG_PRCOMPLETE           207
/*******************************************************
 * ALARM  EVENT
 *  -
 *  ~
 *  -
 *******************************************************/
#define     EVT_ALARM							5
#define	 EVT_ALARM_NOTIFY                   1

 /*******************************************************
  * CONTROL JOB  EVENT
  *  -
  *  ~
  *  -
  *******************************************************/
#define     EVT_CJM							6
#define     EVT_CJMPHASE_CJMCREATIONCOMPLETE	1		// QUED
#define     EVT_CJMPHASE_CJMSELECTED			2		// SELECTED
#define     EVT_CJMPHASE_CJMEXCUTING			3		// Control Job Running
#define     EVT_CJMPHASE_CJMPRCOMPLETED		4		// Control Job finished , Deque in Control Job List
#define     EVT_CJMPHASE_CJMPAUSE				300
#define     EVT_CJMPHASE_CJMRESUME				301
#define     EVT_CJMPHASE_CJMCANCEL				302
#define     EVT_CJMPHASE_CJMSTOP				303
#define     EVT_CJMPHASE_CJMABORT				304
#define	 EVT_CJMPHASE_CJWAITFORSTART        310

  /******************************************************
   * Time Tracer Event
   * S6F1
   ******************************************************/
#define     EVT_TIMETRACE						161

   /******************************************************
	* Equipment Terminal Services
	* S10F1
	******************************************************/
#define EVT_ETS									7
#define EVT_MSG_TO_HOST							17001
#define EVT_MSG_RECOGNIZED						3001

	/******************************************************
	 * SPOOLING SERVICES EVENTS
	 * S2F24
	 ******************************************************/
#define EVT_SPOOL								8		// Spooling related events
#define EVT_UNLOAD_SPOOL                        18001		// Unload spool as per MaxSpoolTransmit
#define EVT_SPOOL_ACTIVATED						1001       // Spooling activated
#define EVT_SPOOL_DEACTIVATED					1002		// Spooing deactivated

	 /******************************************************
	  * ALL OTHER EVENTS
	  * S2F17
	  ******************************************************/
#define EVT_OTHER								9		// All other events
#define EVT_REQUEST_TIME_FROM_HOST              19001		// Get time from Host
#define EVT_REQ_MB_RECIPE						19002
#define EVT_UPLOAD_RECIPE						19003
#define EVT_RCP_CHANGE                          4200
#define EVT_RCP_SELECTED                        4201
#define EVT_DOWNLOAD_RECIPE						19005
	  /******************************************************
	   * ALL EXtra
	   * S2F17
	   ******************************************************/
#define EVT_EXTRA							2000

	   /******************************************************
		* PrJobAlert
		* s16f7
		******************************************************/
#define PRJOB_ALERT							3000

#define PRJOB_ALERT2						3500

		/******************************************************
		 * PrJobEvent NOtify
		 * s16f9
		 ******************************************************/
#define EVT_PJM_PRJEN						10

		 // #define EVT_CJM_NEW                         4000

#define EVT_TRANSACTION						5000
#define EVT_ABORT_TRANSACTION				5001
#define EVT_EXCESS_MESSAGE_LENGTH           5002
#define EVT_PAUSE_EVENTS					5003
#define EVT_CONTINUE_EVENTS					5004
#define EVT_EXCESS_LESS_LENGTH				5005
#define EVT_CLEAR_WATT_LIST                 5006
#define EVT_PJMSTARTNEXTPJ					32000

/******************************************************
 * Ui Data Request
 * No Stream & function !
 ******************************************************/
#define EVT_UI_DATA_REQUEST						11

enum {
	REQ_VAR_DATA = 11000,
	REQ_CEID_DATA,
	REQ_PJDATA,
	REQ_CJDATA,
	DUMP_VAR_DATA,
	DUMP_CEID_DATA
};
#endif
