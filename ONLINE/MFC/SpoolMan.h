// SpoolMan.h: interface for the CSpoolMan class.
//
//////////////////////////////////////////////////////////////////////

#pragma once
#pragma warning (disable: 4786)

#include <map>
#include <vector>
using std::map; using std::vector;
typedef map<int, vector<int> > StreamFunctMap;

class CSpoolMan : public CObject  // Spool is singleton with serialization support
{
	static CSpoolMan* m_pSpool;
	CSpoolMan();
	UINT MaxSpoolItems;				/** Max messages the spool can hold	*/
	UINT SpoolCountActual;			/** Actual messages in the spool	*/
	UINT SpoolCountTotal;			/** Messages spooled so far			*/
	BOOL bEnabled;					/** Is spooling enabled now			*/
	BOOL bSpoolFull;				/** Is the SPOOL full yet ?			*/
	BOOL bSpoolStarted;				/** Is the SPOOL started yet ?		*/
	COleDateTime SpoolFullTime;		/** Time at which spool was full	*/
	COleDateTime SpoolStartTime;	/** Time at which spooling started	*/
	static StreamFunctMap SpooledMap;
public:
	UINT TransmitCount;
	BOOL UndoLastTransmit(void);
	BOOL InitSpool(void);
	BOOL EnableAllStreamFunctions(void);
	BOOL OverWriteSpool;			/** Spool Overwrite/discard flag	*/
	UINT MaxSpoolTransmit;			/** Max messages the spool transmit	*/
	void SerializeVar(CArchive& ar);
	void SerializeMsg(CArchive& ar);
	UINT SpooledSoFar(void);
	UINT ActualSpooled(void);
	UINT TransmitAmount(void);		/**May not be required **/
	BOOL IsEnabled();				/** ?? !*/
	static CSpoolMan* GetSpool();
	void SpoolStreamFunct(			/** Spool the stream and function*/
		short Stream, vector<int>& FunctList);
	void RemoveAllStreamFunct(void); /** Remove spooling for all Stream and function*/
	BOOL RemoveStream(short Stream);/** Remove all functions from this stream */
	BOOL EnableAllFunctions(short Stream);/** Enable all functions from this stream */
	BOOL CanSpool(short Stream, short Function);/** Runtime check for spooling of this stream and function */
	BOOL MakeNewSpoolData();
	BOOL MakeNewSpoolLog();
	BOOL WriteMessage(void* pData, UINT nSize);
	BOOL GetSpoolStartTime(char* pData);
	BOOL GetSpoolFullTime(char* pData);
	BOOL ReadSpoolData();
	int	 GetNextSpooledMessage(void* pData);
	int	 SetSpoolStartTime(int nYear, int Month, int Day, int Hour, int Min, int Sec);
	int	 SetSpoolFullTime(int nYear, int Month, int Day, int Hour, int Min, int Sec);
	void SaveVariables();
	void ReadVariables();
	void SaveMessage();
	void ReadMessage(void* pData);
	void UpdateMsgCount(void);
	DECLARE_SERIAL(CSpoolMan)
	virtual ~CSpoolMan();
};
void DelSpMan();
/**
		To be done
		1) Storing in a file
		2) Message Gem Logical spool variables, Physical file details  & actual messages
			are all stored in the file.
		3) The variables and message parameters are modified in situ by using binary file
			writing routines.

Note    : Spool file structure

		  ______________________
		  |                    |
		  |   Spool            |  This part is fixed length
		  |   Variables        |
		  |                    |
		  ----------------------
		  |                    |
		  |   Message          |  This part is fixed length
		  |   Attributes       |
		  |                    |
		  ----------------------
		  |                    |
		  |   Individual       |  Length of this part varies depending upon the number of messages
		  |   Messages...      |
		  |                    |
		  ----------------------
		  refer documentation for details

*/