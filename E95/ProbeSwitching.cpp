
//02082024/MORSALIN/CProbeSwitching c++
#include "stdafx.h"
#include "ProbeSwitching.h"
#include "E95.h"
#include "413App.h"
#include "Comm/Serial.h"
#include<string>
#include <sstream>
#include <iomanip>
#include "..\413\MotorS2.h"

CProbeSwitching::CProbeSwitching() {

}
CProbeSwitching::~CProbeSwitching()
{
	//Disconnect();
}
string HexVal;

BOOL CProbeSwitching::Connect(int comPort, int transRate, char parity, int stopBit)
{
	com = new CSerialReal;
	C413Global* g = &p413App->Global;
	com->Close();

	if (!com->Open(g->comPort, g->transRate, g->parity, 8, g->stopBit)) {
		return FALSE;
	}
	IsConnect = TRUE;
	return TRUE;
}
void CProbeSwitching::Disconnect()
{
	if (IsConnect) {
		com = new CSerialReal;
		com->Close();
		IsConnect = false;
	}
	return;
}

void CProbeSwitching::Cal_OneCRC(vector<long long> dataArray)
{
	long long initVal = 65535;
	long long val = 40961;

	CRC = initVal ^ dataArray[0];
	for (int i = 0; i < 8; i++)
	{
		int lstBit = (CRC & 1);
		CRC = CRC >> 1;
		if (lstBit == 1)
		{
			CRC = CRC ^ val;
		}
	}
	int vec_len = dataArray.size();
	for (int j = 1; j < vec_len; j++)
	{
		CRC = CRC ^ dataArray[j];
		for (int i = 0; i < 8; i++)
		{
			int lstBit = (CRC & 1);
			CRC = CRC >> 1;
			if (lstBit == 1)
			{
				CRC = CRC ^ val;
			}
		}
	}
}

void DecToHexa(int crc)
{
	HexVal = "";
	while (crc != 0) {
		int rem = 0;
		char ch;
		rem = crc % 16;
		if (rem < 10) ch = rem + 48;
		else ch = rem + 55;
		HexVal += ch;
		crc = crc / 16;
	}
	int i = 0, j = HexVal.size() - 1;
	while (i <= j)
	{
		swap(HexVal[i], HexVal[j]);
		i++;
		j--;
	}
}

int hexToDecimal(const std::string& hexValue) {
	int decimalValue;
	std::sscanf(hexValue.c_str(), "%x", &decimalValue);
	return decimalValue;
}
CString CProbeSwitching::Cal_CRC(std::string stream)
{
	CString tmp1 = "";
	vector<long long> dataArray;
	vector<CString> dataArrayST;
	dataArray.clear();
	dataArrayST.clear();
	int len = stream.size();
	for (int i = 0; i <= len; i++)
	{
		if (stream[i] == ' ' || i == len)
		{
			dataArrayST.push_back(tmp1);
			dataArray.push_back(hexToDecimal((string)tmp1));
			tmp1 = "";
		}
		else tmp1 += stream[i];
	}

	Cal_OneCRC(dataArray);
	DecToHexa(CRC);
	CRCString = HexVal;
	if (CRCString.size() < 4)
	{
		string ttt = "";
		for (int i = 0; i < (4 - CRCString.size()); i++) ttt += '0';
		for (int i = 0; i < CRCString.size(); i++) ttt += CRCString[i];
		CRCString = "";
		CRCString += ttt;
	}
	finalCRC = CRCString.c_str();

	CString finalCRCUp = "";
	CString finalCRCLow = "";

	finalCRCUp += finalCRC[0];
	finalCRCUp += finalCRC[1];

	finalCRCLow += finalCRC[2];
	finalCRCLow += finalCRC[3];

	CString SendStream = "";
	for (int i = 0; i < dataArray.size(); i++)
	{
		if (strlen(dataArrayST[i]) == 1) SendStream += "0";
		SendStream += dataArrayST[i];
		SendStream += " ";
	}

	if (strlen(finalCRCLow) == 1) SendStream += "0";
	SendStream += finalCRCLow;
	SendStream += " ";

	if (strlen(finalCRCUp) == 1) SendStream += "0";
	SendStream += finalCRCUp;
	return SendStream;
}

BOOL CProbeSwitching::isMotorMoving()
{
	if (CurrentPosition()) {
		if(abs(prevPos-p413App->Global.ProbeSwitchCurPos) >= 10)
		{
			prevPos = p413App->Global.ProbeSwitchCurPos;
			return TRUE;
		}
	}
	return FALSE;
}

BOOL CProbeSwitching::ExecuteCommand(CString cmd)
{
	if (!SendCommand(Cal_CRC((string)cmd))) {
		return FALSE;
	}

	// Check the motor is still moving or not
	/*CurrentPosition();
	prevPos = p413App->Global.ProbeSwitchCurPos;
	while (1) {
		if (!isMotorMoving())
			break;
	}*/
	// Motor is stopped now. 
	return TRUE;
}

string decimalToTwosComplementHex(int decimal) {
	int absoluteValue = std::abs(decimal);
	unsigned int binary = static_cast<unsigned int>(absoluteValue);

	if (decimal < 0) {
		binary = (~binary) + 1;
	}
	ostringstream oss;
	oss << hex << setw(2) << setfill('0') << binary;
	return oss.str();
}
BOOL CProbeSwitching::SendCommand(CString SendStream)
{
	string buf = "";
	//Sleep(50);
	if (!IsConnect) AfxMessageBox("Please connect the port first !!");
	else
	{
		int strm_size = strlen(SendStream);
		com->SendData(SendStream, (strm_size / 3) + 1);

		int n = 0;
		char reply[100];
		//Sleep(300);
		buf = "";
		while (n < 100) {
			Sleep(20);
			if (com->ReadData(&reply[n], 1) < 1) {
				break;
			}
			n++;
		}
		int x = 0;
		for (int i = 0; i < n; i++)
		{
			string tmp = decimalToTwosComplementHex(reply[i]);
			if (tmp.size() > 6)
				tmp = tmp.substr(6, 2);
			buf += tmp.c_str();
			buf += " ";
		}
		ReceiveStream = buf.c_str();
		//TRACE("----> %s ----> %s\n", SendStream, ReceiveStream);
		if (ReceiveStream == "") {
			return FALSE;
		}
	}
	return TRUE;
}
BOOL CProbeSwitching::InitMovingParameters(int steps)
{
	CString tmp_Initialize_cmd = Initialize_cmd;
	C413Global* g = &p413App->Global;
	// Reset
	ExecuteCommand("01 06 00 7D 00 80");

	HexVal = decimalToTwosComplementHex(steps);

	string xx = HexVal;
	if (HexVal.size() < 8)
	{
		string ttt = "";
		for (int i = 0; i < (8 - HexVal.size()); i++) ttt += '0';
		for (int i = 0; i < HexVal.size(); i++) ttt += HexVal[i];
		HexVal = "";
		HexVal += ttt;
	}
	xx = HexVal;
	HexVal.insert(2, 1, ' ');
	HexVal.insert(5, 1, ' ');
	HexVal.insert(8, 1, ' ');
	xx = HexVal;
	tmp_Initialize_cmd += HexVal.c_str();
	tmp_Initialize_cmd += " 00 00 ";

	// Speed value conversion & String making
	//int Speed = 2000;
	DecToHexa(g->Speed);
	xx = HexVal;
	if (HexVal.size() < 4)
	{
		string ttt = "";
		for (int i = 0; i < (4 - HexVal.size()); i++) ttt += '0';
		for (int i = 0; i < HexVal.size(); i++) ttt += HexVal[i];
		HexVal = "";
		HexVal += ttt;
	}
	xx = HexVal;
	HexVal.insert(2, 1, ' ');
	xx = HexVal;
	tmp_Initialize_cmd += HexVal.c_str();
	tmp_Initialize_cmd += " ";

	// Acceleration value conversion & String making
	//int Acc_Rate = 1500;
	DecToHexa(g->Acceleration);
	xx = HexVal;
	if (HexVal.size() < 8)
	{
		string ttt = "";
		for (int i = 0; i < (8 - HexVal.size()); i++) ttt += '0';
		for (int i = 0; i < HexVal.size(); i++) ttt += HexVal[i];
		HexVal = "";
		HexVal += ttt;
	}
	xx = HexVal;
	HexVal.insert(2, 1, ' ');
	HexVal.insert(5, 1, ' ');
	HexVal.insert(8, 1, ' ');
	xx = HexVal;
	tmp_Initialize_cmd += HexVal.c_str();

	// Deceleration value conversion & String making
	//int Dcc_Rate = 1500;
	DecToHexa(g->Deceleration);
	xx = HexVal;
	if (HexVal.size() < 8)
	{
		string ttt = "";
		for (int i = 0; i < (8 - HexVal.size()); i++) ttt += '0';
		for (int i = 0; i < HexVal.size(); i++) ttt += HexVal[i];
		HexVal = "";
		HexVal += ttt;
	}
	xx = HexVal;
	HexVal.insert(2, 1, ' ');
	HexVal.insert(5, 1, ' ');
	HexVal.insert(8, 1, ' ');
	xx = HexVal;
	tmp_Initialize_cmd += " ";
	tmp_Initialize_cmd += HexVal.c_str();

	if (!ExecuteCommand(tmp_Initialize_cmd))
		return FALSE;
	return TRUE;
}
BOOL CProbeSwitching::MoveR(int Pos)
{
	if (!InitMovingParameters(Pos))
		return FALSE;

	// This 2 commands will move the motor
	/*if (!ExecuteCommand(Reset_cmd))
		return FALSE;*/
	if (!ExecuteCommand(Moving_cmd))
		return FALSE;

	// Check the motor is still moving or not
	CurrentPosition();
	prevPos = p413App->Global.ProbeSwitchCurPos;
	while (1) {
		if (!isMotorMoving())
			break;
	}
	// Motor is stopped now.

	return TRUE;
}

BOOL CProbeSwitching::MoveA(int Pos)
{
	if (!CurrentPosition())
		return FALSE;
	int PosA = Pos - p413App->Global.ProbeSwitchCurPos;
	if (abs(PosA) < 2) return TRUE;
	if (!MoveR(PosA))
		return FALSE;
	//if (!InitMovingParameters(PosA))
	//	return FALSE;

	//// This 2 commands will move the motor
	//if (!ExecuteCommand(Reset_cmd))
	//	return FALSE;
	//if (!ExecuteCommand(Moving_cmd))
	//	return FALSE;

	//// Check the motor is still moving or not
	//CurrentPosition();
	//prevPos = p413App->Global.ProbeSwitchCurPos;
	//while (1) {
	//	if (!isMotorMoving())
	//		break;
	//}
	//// Motor is stopped now.

	return TRUE;
}
BOOL CProbeSwitching::MoveToECHOprb() // 10172024 Morsalin
{
	if (!MoveA(p413App->Global.MicroscopePrbPos))
		return FALSE;
	return TRUE;
}
BOOL CProbeSwitching::MoveToCWL008prb()
{
	if (!MoveA(p413App->Global.CWL008prbPos))
		return FALSE;
	return TRUE;
}

BOOL CProbeSwitching::MoveToCWL030prb() // 10172024 Morsalin
{
	if (!MoveA(p413App->Global.CWL030prbPos))
		return FALSE;
	return TRUE;
}

BOOL CProbeSwitching::MoveToROUGHNESSprb()
{
	if (!MoveA(p413App->Global.RoughnessPrbPos))
		return FALSE;
	return TRUE;
}

BOOL CProbeSwitching::CurrentPosition()
{
	// Display Feedback Position
	ExecuteCommand(CurPos_cmd);
	if (ReceiveStream == "") {
		return FALSE;
	}
	else {
		string tmp = ReceiveStream;
		tmp = tmp.substr(9, 11);
		tmp.erase(2, 1);
		tmp.erase(4, 1);
		tmp.erase(6, 1);
		p413App->Global.ProbeSwitchCurPos = hexToDecimal(tmp.c_str());
	}
	return TRUE;
}
BOOL CProbeSwitching::ResetAlarm()
{
	// Reset Alarm
	if (!ExecuteCommand(Reset_cmd))
		return FALSE;

	CString IStream = "01 06 00 7D 00 80";
	if (!ExecuteCommand(IStream))
		return FALSE;
	return TRUE;
}
BOOL CProbeSwitching::SetSoftLimit()
{
	// Set Positive Limit
	int pos = p413App->Global.PosSoftLimit;
	HexVal = decimalToTwosComplementHex(pos);
	if (HexVal.size() < 8)
	{
		string ttt = "";
		for (int i = 0; i < (8 - HexVal.size()); i++) ttt += '0';
		for (int i = 0; i < HexVal.size(); i++) ttt += HexVal[i];
		HexVal = "";
		HexVal += ttt;
	}
	HexVal.insert(2, 1, ' ');
	HexVal.insert(5, 1, ' ');
	HexVal.insert(8, 1, ' ');
	PosLimitSet_cmd += HexVal.c_str();
	if (!ExecuteCommand(PosLimitSet_cmd))
		return FALSE;

	// Set Negative Limit
	pos = p413App->Global.NegSoftLimit;
	HexVal = decimalToTwosComplementHex(pos);
	if (HexVal.size() < 8)
	{
		string ttt = "";
		for (int i = 0; i < (8 - HexVal.size()); i++) ttt += '0';
		for (int i = 0; i < HexVal.size(); i++) ttt += HexVal[i];
		HexVal = "";
		HexVal += ttt;
	}
	HexVal.insert(2, 1, ' ');
	HexVal.insert(5, 1, ' ');
	HexVal.insert(8, 1, ' ');
	NegLimitSet_cmd += HexVal.c_str();
	if (!ExecuteCommand(NegLimitSet_cmd))
		return FALSE;
	return TRUE;
}

BOOL CProbeSwitching::toEchoOffset()
{
	if (p413App->pMtr) {
		p413App->pMtr->GotoXY(-p413App->Global.xStageOffset, -p413App->Global.yStageOffset);
		return TRUE;
	}
	return FALSE;
}
BOOL CProbeSwitching::toCWLOffset()
{
	if (p413App->pMtr) {
		p413App->pMtr->GotoXY(p413App->Global.xStageOffset, p413App->Global.yStageOffset);
		return TRUE;
	}
	return FALSE;
}