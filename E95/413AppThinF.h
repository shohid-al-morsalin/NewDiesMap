#pragma once

#include "413AppTrim.h"

class CSpec;
class CRecipe413;
class CSICam;
class COceanBase;

struct SDoPattrnMatching;

class C413AppThinF : public C413AppTrim {
public:

	COceanBase* pOceanOp;
	BOOL SaveSpectrum(CString fname);

	static DWORD WINAPI GetThinFilmThread(LPVOID Param);
	BOOL ExecuteTFUMC(CRecipe413* pRcp, CSICam* pVideo, CMeParam* pMeP, short i, CMPoint* pMP, short ExpBck, BOOL bFirst);
	BOOL UpdateResult(BOOL bAllOK, short St, CMPoint* pMP, short MeSetX, CMeParam* pMeP, MParam::PRBNUM Pb);
	BOOL ExecuteTFILM(CRecipe413* pRcp, CSICam* pVideo, CMeParam* pMeP, short St, CMPoint* pMP, short ExpBck, BOOL bFirst);
	BOOL ExecuteODSTI(CRecipe413* pRcp, CSICam* pVideo, CMeParam* pMeP, short St, CMPoint* pMP, short ExpBck);
	BOOL GetThinFilmThicknesses(SDoPattrnMatching& Param);
	BOOL TakeReference(CRecipe413* pRcp, CSICam* pVideo, int Exposure1, int Exposure2);
	BOOL TakeBackground(CRecipe413* pRcp, CSICam* pVideo, int Exposure1, int Exposure2);
	BOOL OpenSpecCSV(CString SpecName, CSpec* pSpec);
	BOOL OpenSpecFile(CString SpecName, CSpec* pSpec);

	C413AppThinF();
	virtual ~C413AppThinF();
};
