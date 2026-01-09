#pragma once

#include "CWLBase.h"
// copy CWL class / Mahedi Kamal 10182024
class CCWL2 : public CCWLBase
{
	CCriticalSection	CS;
	WSADATA				WinSockData;
	SOCKET				TCPClientSocket;
	SOCKADDR_IN			TCPServerAdd;
	int					iSend;
	int					iRecv;
	bool				bConnected;

public:

	CCWL2();

	void DisplayError();

	bool Connect(CString address, CListBox* pMessage);
	bool ConnectRS232(int nPort, CListBox* pMessage); // Mahedi Kamal 10172024

	bool Disconnect() {
		closesocket(TCPClientSocket);
		bConnected = false;

		return true;
	}

	bool Send(CString str);
	bool SendRS232(CString str); // Mahedi Kamal 10172024
	CString Receive();

	bool SendAndResponse(CMD str);

	bool SendAndResponse(CString str);
	bool SendAndResponseRS232(CString str); // Mahedi Kamal 10172024

	bool ProcessResponse(CString str);

	bool GetMeasuredValueMRO(int nChan, double& dVal);

	bool GetMeasuredValueMSO(int nChan, double& dVal);
	bool SetOFMFormat(int nChan); //24102024/MORSALIN
	bool GetOFMFormat(int& n); //24102024/MORSALIN
	bool SetDSAFormat(int nChan); //24102024/MORSALIN
	bool GetDSAFormat(int& n); //24102024/MORSALIN
	bool SetDSBFormat(int nChan); //24102024/MORSALIN
	bool GetDSBFormat(int& n); //24102024/MORSALIN

	bool GetMeasuredValueMFO(int nCmd, int nChan, double& dVal); // 10172024

	bool GetMeasuredValueMCO() {
		if (!bConnected) return false;

		if (!SendAndResponse(CmdCompose(MCO))) {
			DisplayError();
			return false;
		}
		return true;
	}

	bool SetMSOFormat(int n);

	bool GetMSOFormat(int& n);

	bool SetZero(int nChan);

	bool SetMovingAverage(int nChan, int power);    // Mahedi Kamal // 01202025

	int GetMovingAverage(int nChan); // // Mahedi Kamal // 09232025

	bool SetCALKValue(float n);						// 10132022 / yukchiu

	bool GetCALKValue(float& n);					// 10132022 / yukchiu
	/*
	* to do continuous data acquisition
	* CSD,1 means using serial
	* CSD,2 means using ethernet
	*/
	bool StartContinuousMode();
	bool StopContinuousMode();
	/*
	* consist of multiple data
	*/
	bool GetContinuousData(std::vector<std::vector<CString>>& _ContinuousArray);
	/*
	* consist of only one data by MCO
	*/
	bool GetContinuousMCOData(std::vector<std::vector<CString>>& _ContinuousArray);

	// [ FSMB
	bool CCWL2::GetDataSavingQuantity(int& n);
	bool CCWL2::GetDataSavingInterval(int& n);
	bool CCWL2::GetDataSavingFuncStatus(int& n);
	bool CCWL2::GetDataSavingCirculationStatus(int& n);
	std::vector<CString> CCWL2::OutputSavedData();
	std::vector<CString> CCWL2::OutputSavedData(CHANNEL ch);
	std::vector<CString> CCWL2::OutputSavedData(int _startIndex, int _endIndex);
	std::vector<CString> CCWL2::OutputEncoderData();
	std::vector<CString> CCWL2::OutputEncoderData(AXIS_ASSIGNED _axis);
	bool CCWL2::ClearSavedData();
	bool CCWL2::GetDataSavedIndex(int& n);

	void howToUseAbove() {
		std::vector<CString> out;
		int data;
		GetDataSavingQuantity(data);
		GetDataSavingInterval(data);
		GetDataSavingFuncStatus(data);
		GetDataSavingCirculationStatus(data);
		GetDataSavedIndex(data);
		out = OutputSavedData();
		out = OutputSavedData(CHANNEL::CH3);
		out = OutputSavedData(1, 3);
		out = OutputEncoderData();
		out = OutputEncoderData(AXIS_ASSIGNED::X);
		ClearSavedData();
	}
	// ]

	//Tushar //20231227
	bool SetTRGINTFormat();
	bool SetKValue(double k);
	bool GetContinuousMROData(double& data);
	bool SetDisplacementMode(int nChannel);
	bool SetAutoIntensity(int nChannnel, int mode);
	bool SetAutoTunningSetpoint(int nChannnel, int value);
};