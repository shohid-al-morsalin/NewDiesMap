#include "stdafx.h"
#include "CWLBase.h"
#include "413App.h"

CString CCWLBase::tostring(CMD cmd) {
	switch (cmd)
	{
	case MTO:return stringify(MTO);
	case SSP:return stringify(SSP);
	case ONO:return stringify(ONO);
	case OBJ:return stringify(OBJ);
	case SCA:return stringify(SCA);
	case MOS:return stringify(MOS);
	case MTN:return stringify(MTN);
	case MTX:return stringify(MTX);
	case CMT:return stringify(CMT);
	case LCK:return stringify(LCK);
	case HLD:return stringify(HLD);
	case ODR:return stringify(ODR);
	case SDD:return stringify(SDD);
	case SRZ:return stringify(SRZ);
	case STH:return stringify(STH);
	case MSO:return stringify(MSO);
	case MRO:return stringify(MRO);
	case SHZ:return stringify(SHZ);
	case EXP:return stringify(EXP);
	case PDT:return stringify(PDT);
	case ATR:return stringify(ATR);
	case ATS:return stringify(ATS);
	case ATI:return stringify(ATI);
	case LED:return stringify(LED);
	case TRG:return stringify(TRG);
	case DTM:return stringify(DTM);
	case SFM:return stringify(SFM);
	case SAB:return stringify(SAB);
	case FTR:return stringify(FTR);
	case HMM:return stringify(HMM);
	case HLV:return stringify(HLV);
	case ASC:return stringify(ASC);
	case ZRO:return stringify(ZRO);
	case OFS:return stringify(OFS);
	case JDH:return stringify(JDH);
	case JDL:return stringify(JDL);
	case UNI:return stringify(UNI);
	case OFM:return stringify(OFM);
	case RFM:return stringify(RFM);
	case CRD:return stringify(CRD);
	case CRF:return stringify(CRF);
	case MAT:return stringify(MAT);
	case RFI:return stringify(RFI);
	case DSA:return stringify(DSA);
	case DSB:return stringify(DSB);
	case DSS:return stringify(DSS);
	case DSR:return stringify(DSR);
	case DSO:return stringify(DSO);
	case DSC:return stringify(DSC);
	case DSI:return stringify(DSI);
	case PRG:return stringify(PRG);
	case SAV:return stringify(SAV);
	case CPG:return stringify(CPG);
	case INT:return stringify(INT);
	case BPG:return stringify(BPG);
	case SUS:return stringify(SUS);
	case SIF:return stringify(SIF);
	case CIF:return stringify(CIF);
	case ETH:return stringify(ETH);
	case DRK:return stringify(DRK);
	case PWD:return stringify(PWD);
	case STR:return stringify(STR);
	case TIM:return stringify(TIM);
	case RST:return stringify(RST);
	case DEO:return stringify(DEO);
	case MFO:return stringify(MFO);
	case MCO:return stringify(MCO);
	case DCO:return stringify(DCO);
	case CSD:return stringify(CSD);
	case NCFG:return stringify(NCFG);
	case NMAC:return stringify(NMAC);
	case NLIP:return stringify(NLIP);
	case NSNT:return stringify(NSNT);
	case NGWY:return stringify(NGWY);
	case NLPT:return stringify(NLPT);
	case NRIP:return stringify(NRIP);
	case NRPT:return stringify(NRPT);
	case NSAV:return stringify(NSAV);
	case ATP_L:return stringify(ATP_L);
	case ATP_E:return stringify(ATP_E);
	case CAL_K:return stringify(CAL_K);
	case CAL_V:return stringify(CAL_V);
	case CAL_O:return stringify(CAL_O);
	case AO_MOD:return stringify(AO_MOD);
	case AO_NER:return stringify(AO_NER);
	case AO_FAR:return stringify(AO_FAR);
	case AO_ASC:return stringify(AO_ASC);
	case AO_CPE:return stringify(AO_CPE);
	case AO_SAV:return stringify(AO_SAV);
	case DI_PIN:return stringify(DI_PIN);
	case DI_CTL:return stringify(DI_CTL);
	case DI_FTR:return stringify(DI_FTR);
	case DO_PIN:return stringify(DO_PIN);
	case DO_CTL:return stringify(DO_CTL);
	case DO_STT:return stringify(DO_STT);
	case REBOOT:return stringify(REBOOT);
	case DIO_SAV:return stringify(DIO_SAV);
	case ENC_AXS:return stringify(ENC_AXS);
	case ENC_MOD:return stringify(ENC_MOD);
	case ENC_PHS:return stringify(ENC_PHS);
	case ENC_ZRO:return stringify(ENC_ZRO);
	case ENC_ITV:return stringify(ENC_ITV);
	case ENC_FTR:return stringify(ENC_FTR);
	case ENC_PST:return stringify(ENC_PST);
	case ENC_PSP:return stringify(ENC_PSP);
	case ENC_PRE:return stringify(ENC_PRE);
	case ENC_CNT:return stringify(ENC_CNT);
	case ENC_SAV:return stringify(ENC_SAV);
	case FTR_AVG:return stringify(FTR_AVG);
	case FTR_MID:return stringify(FTR_MID);
	case CAL_OFM:return stringify(CAL_OFM);
	case PMS_LOCK:return stringify(PMS_LOCK);
	}
	return "";
}

CString CCWLBase::CmdCompose(CMD cmd, int _p1)
{
	CString str; str.Format("%d", _p1);

	if(p413App->bConfocalRS232NotEthernet)
		return CMDFIRSTCHAR + tostring(cmd) + "," + str;
	return CMDFIRSTCHAR + tostring(cmd) + "," + str + "\r\n";
}

CString CCWLBase::CmdCompose(CMD cmd, double _p1)
{
	CString str; str.Format("%lf", _p1);
	return CMDFIRSTCHAR + tostring(cmd) + "," + str + "\r\n";
}

CString CCWLBase::CmdCompose(CMD cmd)
{
	return CMDFIRSTCHAR + tostring(cmd) + "\r\n";
}

CString CCWLBase::CmdCompose(CMD cmd, CString _p1)
{
	return CMDFIRSTCHAR + tostring(cmd) + "," + _p1 + "\r\n";
}

CString CCWLBase::CmdCompose(CMD cmd, CString _p1, int _p2)
{
	CString str; str.Format("%d", _p2);
	//[ 23092025
	if (p413App->bConfocalRS232NotEthernet) // MORSALIN
		return CMDFIRSTCHAR + tostring(cmd) + "," + _p1 + "," + str;
	else
		return CMDFIRSTCHAR + tostring(cmd) + "," + _p1 + "," + str + "\r\n";
	//]
}

CString CCWLBase::CmdCompose(CMD cmd, int _p1, CString _p2)
{
	CString str; str.Format("%d", _p1);
	return CMDFIRSTCHAR + tostring(cmd) + "," + str + "," + _p2 + "\r\n";
}

CString CCWLBase::CmdCompose(CMD cmd, int _p1, CString _p2, int _p3)
{
	CString str; str.Format("%d", _p1);
	CString str2; str2.Format("%d", _p3);
	return CMDFIRSTCHAR + tostring(cmd) + "," + str + "," + _p2 + "," + str2 + "\r\n";
}

CString CCWLBase::CmdCompose(CMD cmd, int _p1, double _p2, double _p3)
{
	CString str; str.Format("%d", _p1);
	CString str2; str2.Format("%lf", _p2);
	CString str3; str3.Format("%lf", _p3);
	return CMDFIRSTCHAR + tostring(cmd) + "," + str + "," + str2 + "," + str3 + "\r\n";
}

CString CCWLBase::CmdCompose(CMD cmd, double _p1, double _p2, double _p3, double _p4, double _p5)
{
	CString str; str.Format("%lf", _p1);
	CString str2; str2.Format("%lf", _p2);
	CString str3; str3.Format("%lf", _p3);
	CString str4; str4.Format("%lf", _p4);
	CString str5; str5.Format("%lf", _p5);
	return CMDFIRSTCHAR + tostring(cmd) + "," + str + "," + str2 + "," + str3 + "," + str4 + "," + str5 + "\r\n";
}

CString CCWLBase::CmdCompose(CMD cmd, double _p1, int _p2)
{
	CString str; str.Format("%lf", _p1);
	CString str2; str2.Format("%d", _p2);
	return CMDFIRSTCHAR + tostring(cmd) + "," + str + "," + str2 + "\r\n";
}

CString CCWLBase::CmdCompose(CMD cmd, double _p1, double _p2, double _p3, double _p4)
{
	CString str; str.Format("%lf", _p1);
	CString str2; str2.Format("%lf", _p2);
	CString str3; str3.Format("%lf", _p3);
	CString str4; str4.Format("%lf", _p4);

	return CMDFIRSTCHAR + tostring(cmd) + "," + str + "," + str2 + "," + str3 + "," + str4 + "\r\n";
}

CString CCWLBase::CmdCompose(CMD cmd, int _p1, int _p2)
{
	CString str; str.Format("%d", _p1);
	CString str2; str2.Format("%d", _p2);
	if (p413App->bConfocalRS232NotEthernet) // MORSALIN
		return CMDFIRSTCHAR + tostring(cmd) + "," + str + "," + str2;
	else 
		return CMDFIRSTCHAR + tostring(cmd) + "," + str + "," + str2 + "\r\n";
}

CString CCWLBase::CmdCompose(CMD cmd, int _p1, int _p2, int _p3)
{
	CString str; str.Format("%d", _p1);
	CString str2; str2.Format("%d", _p2);
	CString str3; str3.Format("%d", _p3);
	return CMDFIRSTCHAR + tostring(cmd) + "," + str + "," + str2 + "," + str3 + "\r\n";
}

CString CCWLBase::CmdCompose(CMD cmd, int _p1, int _p2, int _p3, int _p4)
{
	CString str; str.Format("%d", _p1);
	CString str2; str2.Format("%d", _p2);
	CString str3; str3.Format("%d", _p3);
	CString str4; str4.Format("%d", _p4);

	return CMDFIRSTCHAR + tostring(cmd) + "," + str + "," + str2 + "," + str3 + "," + str4 + "\r\n";
}

CString CCWLBase::CmdCompose(CMD cmd, int _p1, double _p2, double _p3, double _p4)
{
	CString str; str.Format("%d", _p1);
	CString str2; str2.Format("%lf", _p2);
	CString str3; str3.Format("%lf", _p3);
	CString str4; str4.Format("%lf", _p4);
	return CMDFIRSTCHAR + tostring(cmd) + "," + str + "," + str2 + "," + str3 + "," + str4 + "\r\n";
}

void CCWLBase::DisplayError() {  }

bool CCWLBase::Connect(CString address, CListBox* pMessage) { return true; }
bool CCWLBase::ConnectRS232(int nPort, CListBox* pMessage) { return true; } // Mahedi Kamal 10172024

bool CCWLBase::Disconnect() {
	return true;
}

bool CCWLBase::Send(CString str) { return true; }

CString CCWLBase::Receive() { return ""; }

bool CCWLBase::SendAndResponse(CMD str) { return true; }

bool CCWLBase::SendAndResponse(CString str) { return true; }

bool CCWLBase::ProcessResponse(CString str) { return true; }

bool CCWLBase::GetMeasuredValueMRO(int nChan, double& dVal) {
	dVal = 0.0;
	return true;
}

bool CCWLBase::GetMeasuredValueMSO(int nChan, double& dVal) {
	dVal = 0.0;
	return true;
}

bool CCWLBase::GetMeasuredValueMFO(int nCmd, int nChan, double& dVal) {
	dVal = 0.0;
	return true;
}

bool CCWLBase::GetMeasuredValueMCO() {
	return true;
}

bool CCWLBase::SetMSOFormat(int n) {
	return true;
}

bool CCWLBase::GetMSOFormat(int& n) {
	return true;
}

bool CCWLBase::SetZero(int nChan) {
	return true;
}

bool CCWLBase::SetMovingAverage(int nChan, int power) {
	return true;
}

//[ 09232025
int CCWLBase::GetMovingAverage(int nChan) {
	return 0;
}
//]


bool CCWLBase::SetCALKValue(float n) {
	return true;
}

// 10132022 / yukchiu

bool CCWLBase::GetCALKValue(float& n) {
	return true;
}

/*
* to do continuous data acquisition
* CSD,1 means using serial
* CSD,2 means using ethernet
*/

bool CCWLBase::StartContinuousMode() {
	return true;
}

bool CCWLBase::StopContinuousMode() {
	return true;
}

/*
* consist of multiple data
*/

bool CCWLBase::GetContinuousData(std::vector<std::vector<CString>>& _ContinuousArray) {
	return true;
}

/*
* consist of only one data by MCO
*/

bool CCWLBase::GetContinuousMCOData(std::vector<std::vector<CString>>& _ContinuousArray) {
	return true;
}

// [ FSMB

bool CCWLBase::GetDataSavingQuantity(int& n) {
	n = 0;
	return true;
}

bool CCWLBase::GetDataSavingInterval(int& n) {
	n = 0;
	return true;
}

bool CCWLBase::GetDataSavingFuncStatus(int& n) {
	n = 0;
	return true;
}

bool CCWLBase::GetDataSavingCirculationStatus(int& n) {
	n = 0;
	return true;
}

std::vector<CString> CCWLBase::OutputSavedData() { return std::vector<CString>(); }

std::vector<CString> CCWLBase::OutputSavedData(CHANNEL ch) { return std::vector<CString>(); }

std::vector<CString> CCWLBase::OutputSavedData(int _startIndex, int _endIndex) { return std::vector<CString>(); }

std::vector<CString> CCWLBase::OutputEncoderData() { return std::vector<CString>(); }

std::vector<CString> CCWLBase::OutputEncoderData(AXIS_ASSIGNED _axis) { return std::vector<CString>(); }

bool CCWLBase::ClearSavedData() { return true; }

bool CCWLBase::GetDataSavedIndex(int& n) {
	n = 0;
	return true;
}

//[ Tushar 20240122
bool CCWLBase::SetDisplacementMode(int nChannel) {
		return true;
}

bool CCWLBase::SetAutoIntensity(int nChannnel, int mode) {
	return true;
}

bool CCWLBase::SetAutoTunningSetpoint(int nChannnel, int value) {
	return true;
}
//]

// [ 24102024/MORSALIN
bool CCWLBase::SetOFMFormat(int nChan)
{
	return true;
}
bool CCWLBase::GetOFMFormat(int& n)
{
	return true;
}
bool CCWLBase::SetDSAFormat(int nChan)
{
	return true;
}
bool CCWLBase::GetDSAFormat(int& n)
{
	return true;
}
bool CCWLBase::SetDSBFormat(int nChan)
{
	return true;
}
bool CCWLBase::GetDSBFormat(int& n)
{
	return true;
}
// ]