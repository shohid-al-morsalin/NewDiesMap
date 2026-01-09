#include "stdafx.h"
#include "CWL2.h"

#include "SRC/DOSUtil.h"
#include "..\serial\SerialICC.h" // Mahedi Kamal  10172024
#include "413App.h" // Mahedi Kamal 10172024
#include<stdlib.h> // 09232025

CCWL2::CCWL2() {
	bConnected = false;
}

void CCWL2::DisplayError() {
	AfxMessageBox(_ErrorDescription, MB_ICONERROR);
}

bool CCWL2::Connect(CString address, CListBox* pMessage)
{
	if (bConnected) return true;
	int iWsaStartup = WSAStartup(MAKEWORD(2, 2), &WinSockData);
	if (iWsaStartup != 0)
	{
		pMessage->InsertString(pMessage->GetCount(), "Confocal2 (Ethernet) WSAStartup 2 failed");
		//AfxMessageBox("WSAStartup 2 failed !");
		return false;
	}
	// WSAStartUp Success

	TCPClientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (TCPClientSocket == INVALID_SOCKET)
	{
		pMessage->InsertString(pMessage->GetCount(), "Confocal2 (Ethernet) TCP Client Socket 2 creation failed");
		//AfxMessageBox("TCP Client Socket 2 creation failed !");
		return false;
	}
	// TCP Client Socket Creation Success

	// create server structure (CWL controller)
	TCPServerAdd.sin_family = AF_INET;
	TCPServerAdd.sin_addr.s_addr = inet_addr(address);
	TCPServerAdd.sin_port = htons(5000);

	int iConnect = connect(TCPClientSocket, (SOCKADDR*)&TCPServerAdd, sizeof(TCPServerAdd));

	if (iConnect == SOCKET_ERROR)
	{
		CString err_msg; err_msg.Format("Error code: %d", WSAGetLastError());
		pMessage->InsertString(pMessage->GetCount(), "Confocal2 (Ethernet) system started FAILED!");
		//AfxMessageBox("Connection 2 Failed!" + err_msg);
		return false;
	}
	// Connection Success

	// send CIF command to get controller infomation
	if (!SendAndResponse(CIF)) bConnected = false;
	else
	{
		pMessage->InsertString(pMessage->GetCount(), "Confocal2 (Ethernet) system started OK");
		bConnected = true;
	}
	return bConnected;
}
//[ // Mahedi Kamal 10172024
bool CCWL2::ConnectRS232(int nPort, CListBox* pMessage) {
	/*if (m_pConfocalCom) { delete m_pConfocalCom; m_pConfocalCom = NULL; }
	m_pConfocalCom = new CSerialICC;*/

	if (m_pConfocalCom2) {
		if (m_pConfocalCom2->Open(nPort, 115200, 'N', 8, 1)) {
			pMessage->InsertString(pMessage->GetCount(), "Confocal (RS232) system started OK");
			bConnected = true;
			return TRUE;
		}
	}
	pMessage->InsertString(pMessage->GetCount(), "Confocal (RS232) system started failed");
	return FALSE;
}

//]

bool CCWL2::Send(CString str) {
	if (str.Find("\r\n") == -1) str.Append("\r\n");
	iSend = send(TCPClientSocket, str.GetBuffer(0), str.GetLength(), 0);
	if (iSend == SOCKET_ERROR)
	{
		//AfxMessageBox("Command send failed!");
		return false;
	}
	return true;
}

//[ // Mahedi Kamal 10172024
bool CCWL2::SendRS232(CString str) {
	if (str.Find("\r\n") == -1) str.Append("\r\n");
	char cmd[100];
	strcpy_s(cmd, CT2A(str));
	iSend = m_pConfocalCom2->Send(cmd);
	if (iSend == SOCKET_ERROR)
	{
		//AfxMessageBox("Command send failed!");
		return false;
	}
	return true;
}
//]

CString CCWL2::Receive() {
	char recvBuf[256];
	recvBuf[0] = '\0';
	int len = 256;
	iRecv = recv(TCPClientSocket, recvBuf, len, 0);
	return CString(recvBuf);
}

bool CCWL2::SendAndResponse(CMD str)
{
	CString cmd = CmdCompose(str);
	return SendAndResponse(cmd);
}

bool CCWL2::SendAndResponse(CString str) {
	CSingleLock lock(&CS);
	lock.Lock();
	if (!Send(str)) { lock.Unlock(); return false; }
	if (!ProcessResponse(Receive())) { lock.Unlock(); return false; }
	lock.Unlock();

	return true;
}

//[ // Mahedi Kamal 10172024
bool CCWL2::SendAndResponseRS232(CString str) {
	CSingleLock lock(&CS);
	lock.Lock();
	if (!SendRS232(str)) { lock.Unlock(); return false; }
	Sleep(10);
	char Buff[256];
	if (!m_pConfocalCom2->ReadData(Buff, 255)) { lock.Unlock(); return false; }
	CString response(Buff);
	if (!ProcessResponse(response)) { lock.Unlock(); return false; }
	lock.Unlock();

	return true;
}
//]

bool CCWL2::ProcessResponse(CString str) {
	int symbala = str.Find("$");
	str = str.Mid(symbala);
	int foundDelimiter = str.Find("\r\n");

	CString line = str.Mid(symbala, foundDelimiter);
	CString text = line.Trim();
	if (text.Find("ERR") != -1)
	{
		_ErrorDescription = "Parameter or command error.";
		return false;
	}

	// to verify the response header
	_ResultArray.clear();
	int foundSeperater = line.Find(",");
	CString text2 = line.Mid(0, foundSeperater == -1 ? line.GetLength() : foundSeperater);
	if (text.Find(text2) != -1)
	{
		text.Delete(0, text2.GetLength());
		text.Replace(" ", "");

		// get each value our of response seperated by ","
		int i = 0;
		while (i != -1)
		{
			CString res = text.Tokenize(",", i);
			_ResultArray.push_back(res);
		}
	}
	return true;
}

bool CCWL2::GetMeasuredValueMRO(int nChan, double& dVal)
{
	if (!bConnected) return false;
	//[ // Mahedi Kamal 10172024
	if (p413App->bConfocalRS232NotEthernet == TRUE) {
		if (!SendAndResponseRS232(CmdCompose(MRO, nChan))) {
			DisplayError();
			return false;
		}
	} //]
	else {
		if (!SendAndResponse(CmdCompose(MRO, nChan))) {
			DisplayError();
			return false;
		}
	}
	dVal = atof(_ResultArray[0].GetBuffer(0));
	return true;
}

bool CCWL2::GetMeasuredValueMFO(int nCmd, int nChan, double& dVal) {
	if (!bConnected) return false;

	//[ // Mahedi Kamal 10172024
	if (p413App->bConfocalRS232NotEthernet == TRUE) {
		if (!SendAndResponseRS232(CmdCompose(MFO, nCmd, nChan))) {
			DisplayError();
			return false;
		}
	} //]
	else {
		if (!SendAndResponse(CmdCompose(MFO, nCmd, nChan))) {
			DisplayError();
			return false;
		}
	}
	dVal = atof(_ResultArray[0].GetBuffer(0));
	return true;

}

bool CCWL2::GetMeasuredValueMSO(int nChan, double& dVal)
{
	if (!bConnected) return false;

	if (!SendAndResponse(CmdCompose(MSO, nChan))) {
		DisplayError();
		return false;
	}
	else {
		dVal = atof(_ResultArray[0].GetBuffer(0));
		return true;
	}
}

// [ 24102024/MORSALIN
bool CCWL2::SetOFMFormat(int nChan)
{
	if (!bConnected) return false;

	if (!SendAndResponse(CmdCompose(OFM, 1, nChan))) return false;
	return true;
}

bool CCWL2::GetOFMFormat(int& n) {
	if (!bConnected) return false;

	if (!SendAndResponse(CmdCompose(OFM, "?", 1))) {
		DisplayError();
		return true;
	}
	else {
		n = atoi(_ResultArray[0].GetBuffer(0));
		return true;
	}
}

bool CCWL2::SetDSAFormat(int nChan)
{
	if (!bConnected) return false;

	if (!SendAndResponse(CmdCompose(DSA, 10))) return false;
	return true;
}

bool CCWL2::GetDSAFormat(int& n) {
	if (!bConnected) return false;

	if (!SendAndResponse(CmdCompose(DSA, "?"))) {
		DisplayError();
		return true;
	}
	else {
		n = atoi(_ResultArray[0].GetBuffer(0));
		return true;
	}
}

bool CCWL2::SetDSBFormat(int nChan)
{
	if (!bConnected) return false;

	if (!SendAndResponse(CmdCompose(DSB, 130))) return false;
	return true;
}

bool CCWL2::GetDSBFormat(int& n) {
	if (!bConnected) return false;

	if (!SendAndResponse(CmdCompose(DSB, "?"))) {
		DisplayError();
		return true;
	}
	else {
		n = atoi(_ResultArray[0].GetBuffer(0));
		return true;
	}
}
// ]

bool CCWL2::SetMSOFormat(int n) {
	if (!bConnected) return false;

	if (!SendAndResponse(CmdCompose(UNI, n))) return false;
	return true;
}

bool CCWL2::GetMSOFormat(int& n) {
	if (!bConnected) return false;

	if (!SendAndResponse(CmdCompose(UNI, "?"))) {
		DisplayError();
		return true;
	}
	else {
		n = atoi(_ResultArray[0].GetBuffer(0));
		return true;
	}
}

// 10132022 / yukchiu
bool CCWL2::SetCALKValue(float n) {
	if (!bConnected) return false;
	if (n == 0) {
		if (!SendAndResponse(CmdCompose(CAL_K, 0))) return false;
	}
	else if (!SendAndResponse(CmdCompose(CAL_K, n))) return false;

	return true;
}

bool CCWL2::GetCALKValue(float& n) {
	if (!bConnected) return false;

	if (!SendAndResponse(CmdCompose(CAL_K, "?"))) {
		DisplayError();
		return true;
	}
	else {
		n = atof(_ResultArray[0].GetBuffer(0));
		return true;
	}
}
// ==================

bool CCWL2::SetZero(int nChan)
{
	if (!bConnected) return false;
	// [ MORSALIN
	if (p413App->bConfocalRS232NotEthernet)
	{
		if (!SendRS232(CmdCompose(ZRO, 0, nChan))) return false;
		if (!SendRS232(CmdCompose(ZRO, 1, nChan))) return false;
	}
	// ]
	else
	{
		if (!SendAndResponse(CmdCompose(ZRO, 0, nChan))) return false;
		if (!SendAndResponse(CmdCompose(ZRO, 1, nChan))) return false;
	}
	
	return true;
}


// [ // Mahedi Kamal // 01202025
bool CCWL2::SetMovingAverage(int nChan, int power)
{
	if (!bConnected) return false;
	// [ MORSALIN
	if (p413App->bConfocalRS232NotEthernet)
	{
		if (!SendAndResponseRS232(CmdCompose(FTR_AVG, power, nChan))) return false;

	}
	// ]
	else
	{
		if (!SendAndResponse(CmdCompose(FTR_AVG, power, nChan))) return false;

	}

	return true;
}
//]



// [ // Mahedi Kamal // 23092025
int CCWL2::GetMovingAverage(int nChan)
{
	if (!bConnected) return -1;
	// [ MORSALIN
	if (p413App->bConfocalRS232NotEthernet)
	{
		if (!SendAndResponseRS232(CmdCompose(FTR_AVG, "?", nChan))) return -1;

	}
	// ]
	else
	{
		if (!SendAndResponse(CmdCompose(FTR_AVG, "?", nChan))) return -1;

	}
	return atoi(_ResultArray[0]);
	//return 1;
	//return true;
}
//]

/*
* to do continuous data acquisition
* CSD,1 means using serial
* CSD,2 means using ethernet
*/

bool CCWL2::StartContinuousMode() {
	if (!bConnected) return false;

	if (!SendAndResponse(CmdCompose(CSD, 2))) return false;
	return true;
}

bool CCWL2::StopContinuousMode() {
	if (!bConnected) return false;

	if (!SendAndResponse(CmdCompose(CSD, 0))) return false;
	return true;
}

/*
* consist of multiple data
*/

bool CCWL2::GetContinuousData(std::vector<std::vector<CString>>& _ContinuousArray) {
	if (!Send("")) return false;
	DosUtil.Yld(40);
	CString str(Receive());
	int symbala = str.Find("$");
	int symbalb = str.Find(",", symbala);
	str = str.Mid(symbala);

	int foundDelimiter = str.Find("\r\n");
	_ContinuousArray.clear();
	while (foundDelimiter != -1)
	{
		CString line = str.Mid(0, foundDelimiter);
		int i = 0;
		std::vector<CString> nums;
		while (i != -1)
		{
			CString res = line.Tokenize(",", i);
			if (res.Find("$") != -1) continue;
			nums.push_back(res);
		}
		_ContinuousArray.push_back(nums);
		int foundNext = str.Find("$", 1);
		str = foundNext == -1 ? "" : str.Mid(foundNext);
		foundDelimiter = str.Find("\r\n");
	}
	return true;
}

/*
* consist of only one data by MCO
*/

bool CCWL2::GetContinuousMCOData(std::vector<std::vector<CString>>& _ContinuousArray) {
	if (!bConnected) return false;
	if (!SendAndResponse(CmdCompose(MCO))) {
		DisplayError();
		return false;
	}
	_ContinuousArray.clear();
	_ContinuousArray.push_back(_ResultArray);
	return true;
}

bool CCWL2::GetDataSavingQuantity(int& n)
{
	if (SendAndResponse(CmdCompose(CMD::DSA, "?")))
	{
		n = atoi(_ResultArray[0]);	//quantity
		return true;
	}

	_ErrorDescription = "Error from DSA";
	return false;
}
bool CCWL2::GetDataSavingInterval(int& n)
{
	if (SendAndResponse(CmdCompose(CMD::DSB, "?")))
	{
		n = atoi(_ResultArray[0]);	//interval
		return true;
	}

	_ErrorDescription = "Error from DSB";
	return false;
}
bool CCWL2::GetDataSavingFuncStatus(int& n)
{
	if (SendAndResponse(CmdCompose(CMD::DSS, "?")))
	{
		n = atoi(_ResultArray[0]);	//1=ON, 0=OFF, false=ERROR
		return true;
	}

	_ErrorDescription = "Error from DSS";
	return false;
}
bool CCWL2::GetDataSavingCirculationStatus(int& n)
{
	if (SendAndResponse(CmdCompose(CMD::DSR, "?")))
	{
		n = atoi(_ResultArray[0]);	//1=ON, 0=OFF, false=ERROR
		return true;
	}

	_ErrorDescription = "Error from DSR";
	return false;
}
std::vector<CString> CCWL2::OutputSavedData()
{
	if (SendAndResponse(CmdCompose(CMD::DSO))) return _ResultArray;

	_ErrorDescription = "Error from DSO";
	return std::vector<CString>();
}
std::vector<CString> CCWL2::OutputSavedData(CHANNEL ch)
{
	if (SendAndResponse(CmdCompose(CMD::DSO, ch))) return _ResultArray;

	_ErrorDescription = "Error from DSO";
	return std::vector<CString>();
}
std::vector<CString> CCWL2::OutputSavedData(int _startIndex, int _endIndex)
{
	if (SendAndResponse(CmdCompose(CMD::DSO, _startIndex, _endIndex))) return _ResultArray;

	_ErrorDescription = "Error from DSO";
	return std::vector<CString>();
}
std::vector<CString> CCWL2::OutputEncoderData()
{
	if (SendAndResponse(CmdCompose(CMD::DEO))) return _ResultArray;

	_ErrorDescription = "Error from DEO";
	return std::vector<CString>();
}
std::vector<CString> CCWL2::OutputEncoderData(AXIS_ASSIGNED _axis)
{
	if (SendAndResponse(CmdCompose(CMD::DEO, _axis))) return _ResultArray;

	_ErrorDescription = "Error from DEO";
	return std::vector<CString>();
}
bool CCWL2::ClearSavedData()
{
	return SendAndResponse(CmdCompose(CMD::DSC));
}
bool CCWL2::GetDataSavedIndex(int& n)
{
	if (SendAndResponse(CmdCompose(CMD::DSI))) {
		n = atoi(_ResultArray[0]);
		return true;
	}
	_ErrorDescription = "Error from DEO";
	return false;
}

//[ Tushar //20231227
bool CCWL2::SetTRGINTFormat() {
	if (!bConnected) return false;

	if (!SendAndResponse(CmdCompose(TRG, 0))) return false;
	return true;
}

//0 --> Displacement Mode
//1 --> Thickness Mode

bool CCWL2::SetDisplacementMode(int nChannel) {
	if (!bConnected) return false;

	// [ MORSALIN
	if (p413App->bConfocalRS232NotEthernet) {
		if (!SendRS232(CmdCompose(DTM, 0, nChannel))) {
			DisplayError();
			return false;
		}
		else return true;
	}
	// ]
	if (!SendAndResponse(CmdCompose(DTM, 0, nChannel))) {
		DisplayError();
		return false;
	}
	else {
		//AfxMessageBox("Displacement mode on");
		return true;
	}
}

bool CCWL2::SetAutoIntensity(int nChannnel, int mode) {
	//20240122
	if (!bConnected) return false;

	if (!SendAndResponse(CmdCompose(ATR, mode, nChannnel))) {
		DisplayError();
		return false;
	}
	else {
		return true;
	}
}

bool CCWL2::SetAutoTunningSetpoint(int nChannnel, int value) {
	//20240122
	if (!bConnected) return false;

	if (!SendAndResponse(CmdCompose(ATS, value, nChannnel))) {
		DisplayError();
		return false;
	}
	else {
		return true;
	}
}

//]

// [Morsalin 12142023
bool CCWL2::GetContinuousMROData(double& dVal)
{
	//if (!bConnected) return false;

	if (!SendAndResponse(CmdCompose(MRO, 1))) {
		DisplayError();
		return false;
	}
	else {
		dVal = atof(_ResultArray[0].GetBuffer(0));
		return true;
	}
}

//]