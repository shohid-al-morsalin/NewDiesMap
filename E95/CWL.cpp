#include "stdafx.h"
#include "SRC/DOSUtil.h"
#include "CWL.h"
#include "..\serial\SerialICC.h" // Mahedi Kamal  10172024
#include "413App.h" // Mahedi Kamal 10172024

CCWL::CCWL() {
	bConnected = false;
}

void CCWL::DisplayError() {
	AfxMessageBox(_ErrorDescription, MB_ICONERROR);
}

bool CCWL::Connect(CString address, CListBox* pMessage)
{
	if (bConnected) return true;
	int iWsaStartup = WSAStartup(MAKEWORD(2, 2), &WinSockData);
	if (iWsaStartup != 0)
	{
		pMessage->InsertString(pMessage->GetCount(), "Confocal (Ethernet) WSAStartup 1 failed");
		//AfxMessageBox("WSAStartup 1 failed!");
		return false;
	}
	// WSAStartUp Success

	TCPClientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (TCPClientSocket == INVALID_SOCKET)
	{
		pMessage->InsertString(pMessage->GetCount(), "Confocal (Ethernet) TCP Client Socket 1 creation failed");
		//AfxMessageBox("TCP Client Socket 1 creation failed!");
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
		pMessage->InsertString(pMessage->GetCount(), "Confocal (Ethernet) system started FAILED!");
		//CString err_msg; err_msg.Format("Error code: %d", WSAGetLastError());
		//AfxMessageBox("Connection 1 Failed!" + err_msg);
		return false;
	}
	// Connection Success

	// send CIF command to get controller infomation
	if (!SendAndResponse(CIF)) bConnected = false;
	else
	{
		pMessage->InsertString(pMessage->GetCount(), "Confocal (Ethernet) system started OK");
		bConnected = true;
	}
	return bConnected;
}
//[ // Mahedi Kamal 10172024
bool CCWL::ConnectRS232(int nPort, CListBox* pMessage) {
	/*if (m_pConfocalCom) { delete m_pConfocalCom; m_pConfocalCom = NULL; }
	m_pConfocalCom = new CSerialICC;*/

	if (m_pConfocalCom) {
		if (m_pConfocalCom->Open(nPort, 115200, 'N', 8, 1)) {
			pMessage->InsertString(pMessage->GetCount(), "Confocal (RS232) system started OK");
			bConnected = true;
			return TRUE;
		}
	}
	pMessage->InsertString(pMessage->GetCount(), "Confocal (RS232) system started failed");
	return FALSE;
}

//]

bool CCWL::Send(CString str) {
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
bool CCWL::SendRS232(CString str) {
	if (str.Find("\r\n") == -1) str.Append("\r\n");
	char cmd[100];
	strcpy_s(cmd, CT2A(str));
	iSend = m_pConfocalCom->Send(cmd);
	if (iSend == SOCKET_ERROR)
	{
		//AfxMessageBox("Command send failed!");
		return false;
	}
	return true;
}
//]

CString CCWL::Receive() {
	char recvBuf[256];
	recvBuf[0] = '\0';
	int len = 256;
	iRecv = recv(TCPClientSocket, recvBuf, len, 0);
	return CString(recvBuf);
}

bool CCWL::SendAndResponse(CMD str)
{
	CString cmd = CmdCompose(str);
	return SendAndResponse(cmd);
}

bool CCWL::SendAndResponse(CString str) {
	CSingleLock lock(&CS);
	lock.Lock();
	if (!Send(str)) { lock.Unlock(); return false; }
	if (!ProcessResponse(Receive())) { lock.Unlock(); return false; }
	lock.Unlock();

	return true;
}

//[ // Mahedi Kamal 10172024
bool CCWL::SendAndResponseRS232(CString str) {
	CSingleLock lock(&CS);
	lock.Lock();
	if (!SendRS232(str)) { lock.Unlock(); return false; }
	Sleep(10);
	char Buff[256];
	if (!m_pConfocalCom->ReadData(Buff, 255)) { lock.Unlock(); return false; }
	CString response(Buff);
	if (!ProcessResponse(response)) { lock.Unlock(); return false; }
	lock.Unlock();

	return true;
}
//]

bool CCWL::ProcessResponse(CString str) {
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

bool CCWL::GetMeasuredValueMRO(int nChan, double& dVal)
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

// [ 24102024/MORSALIN
bool CCWL::SetOFMFormat(int nChan)
{
	if (!bConnected) return false;

	if (!SendAndResponse(CmdCompose(OFM, 1, nChan))) return false;
	return true;
}

bool CCWL::GetOFMFormat(int& n) {
	if (!bConnected) return false;

	if (!SendAndResponse(CmdCompose(OFM, "?", n))) {
		DisplayError();
		return true;
	}
	else {
		n = atoi(_ResultArray[0].GetBuffer(0));
		return true;
	}
}
// ]

bool CCWL::GetMeasuredValueMFO(int nCmd, int nChan, double& dVal) {
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

bool CCWL::GetMeasuredValueMSO(int nChan, double& dVal)
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

bool CCWL::SetMSOFormat(int n) {
	if (!bConnected) return false;

	if (!SendAndResponse(CmdCompose(UNI, n))) return false;
	return true;
}

bool CCWL::GetMSOFormat(int& n) {
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
bool CCWL::SetCALKValue(float n) {
	if (!bConnected) return false;
	if (n == 0) {
		if (!SendAndResponse(CmdCompose(CAL_K, 0))) return false;
	}
	else if (!SendAndResponse(CmdCompose(CAL_K, n))) return false;

	return true;
}

bool CCWL::GetCALKValue(float& n) {
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

bool CCWL::SetZero(int nChan)
{
	if (!bConnected) return false;
	// [ MORSALIN
	if (p413App->bConfocalRS232NotEthernet)
	{
		if (!SendAndResponseRS232(CmdCompose(ZRO, 0, nChan))) return false;
		if (!SendAndResponseRS232(CmdCompose(ZRO, 1, nChan))) return false;
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
bool CCWL::SetMovingAverage(int nChan, int power)
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
int CCWL::GetMovingAverage(int nChan)
{
	if (!bConnected) return -1;
	// [ MORSALIN
	if (p413App->bConfocalRS232NotEthernet)
	{
		if (!SendAndResponseRS232(CmdCompose(FTR_AVG,"?", nChan))) return -1;

	}
	// ]
	else
	{
		if (!SendAndResponse(CmdCompose(FTR_AVG, "?", nChan))) return -1;

	}
	return atoi(_ResultArray[0]);
	//return 0;
	//return true;
}
//]

/*
* to do continuous data acquisition
* CSD,1 means using serial
* CSD,2 means using ethernet
*/

bool CCWL::StartContinuousMode() {
	if (!bConnected) return false;

	if (!SendAndResponse(CmdCompose(CSD, 2))) return false;
	return true;
}

bool CCWL::StopContinuousMode() {
	if (!bConnected) return false;

	if (!SendAndResponse(CmdCompose(CSD, 0))) return false;
	return true;
}

/*
* consist of multiple data
*/

bool CCWL::GetContinuousData(std::vector<std::vector<CString>>& _ContinuousArray) {
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

bool CCWL::GetContinuousMCOData(std::vector<std::vector<CString>>& _ContinuousArray) {
	if (!bConnected) return false;
	if (!SendAndResponse(CmdCompose(MCO))) {
		DisplayError();
		return false;
	}
	_ContinuousArray.clear();
	_ContinuousArray.push_back(_ResultArray);
	return true;
}

bool CCWL::GetDataSavingQuantity(int& n)
{
	if (SendAndResponse(CmdCompose(CMD::DSA, "?")))
	{
		n = atoi(_ResultArray[0]);	//quantity
		return true;
	}

	_ErrorDescription = "Error from DSA";
	return false;
}
bool CCWL::GetDataSavingInterval(int& n)
{
	if (SendAndResponse(CmdCompose(CMD::DSB, "?")))
	{
		n = atoi(_ResultArray[0]);	//interval
		return true;
	}

	_ErrorDescription = "Error from DSB";
	return false;
}
bool CCWL::GetDataSavingFuncStatus(int& n)
{
	if (SendAndResponse(CmdCompose(CMD::DSS, "?")))
	{
		n = atoi(_ResultArray[0]);	//1=ON, 0=OFF, false=ERROR
		return true;
	}

	_ErrorDescription = "Error from DSS";
	return false;
}
bool CCWL::GetDataSavingCirculationStatus(int& n)
{
	if (SendAndResponse(CmdCompose(CMD::DSR, "?")))
	{
		n = atoi(_ResultArray[0]);	//1=ON, 0=OFF, false=ERROR
		return true;
	}

	_ErrorDescription = "Error from DSR";
	return false;
}
std::vector<CString> CCWL::OutputSavedData()
{
	if (SendAndResponse(CmdCompose(CMD::DSO))) return _ResultArray;

	_ErrorDescription = "Error from DSO";
	return std::vector<CString>();
}
std::vector<CString> CCWL::OutputSavedData(CHANNEL ch)
{
	if (SendAndResponse(CmdCompose(CMD::DSO, ch))) return _ResultArray;

	_ErrorDescription = "Error from DSO";
	return std::vector<CString>();
}
std::vector<CString> CCWL::OutputSavedData(int _startIndex, int _endIndex)
{
	if (SendAndResponse(CmdCompose(CMD::DSO, _startIndex, _endIndex))) return _ResultArray;

	_ErrorDescription = "Error from DSO";
	return std::vector<CString>();
}
std::vector<CString> CCWL::OutputEncoderData()
{
	if (SendAndResponse(CmdCompose(CMD::DEO))) return _ResultArray;

	_ErrorDescription = "Error from DEO";
	return std::vector<CString>();
}
std::vector<CString> CCWL::OutputEncoderData(AXIS_ASSIGNED _axis)
{
	if (SendAndResponse(CmdCompose(CMD::DEO, _axis))) return _ResultArray;

	_ErrorDescription = "Error from DEO";
	return std::vector<CString>();
}
bool CCWL::ClearSavedData()
{
	return SendAndResponse(CmdCompose(CMD::DSC));
}
bool CCWL::GetDataSavedIndex(int& n)
{
	if (SendAndResponse(CmdCompose(CMD::DSI))) {
		n = atoi(_ResultArray[0]);
		return true;
	}
	_ErrorDescription = "Error from DEO";
	return false;
}

//[ Tushar //20231227
bool CCWL::SetTRGINTFormat() {
	if (!bConnected) return false;

	if (!SendAndResponse(CmdCompose(TRG, 0))) return false;
	return true;
}

//0 --> Displacement Mode
//1 --> Thickness Mode

bool CCWL::SetDisplacementMode(int nChannel) {
	if (!bConnected) return false;

	// [ MORSALIN
	if (p413App->bConfocalRS232NotEthernet) {
		if (!SendAndResponseRS232(CmdCompose(DTM, 0, nChannel))) {
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

bool CCWL::SetAutoIntensity(int nChannnel, int mode) {
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

bool CCWL::SetAutoTunningSetpoint(int nChannnel, int value) {
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
bool CCWL::GetContinuousMROData(double& dVal)
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