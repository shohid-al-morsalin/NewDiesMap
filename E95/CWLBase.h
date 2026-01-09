#pragma once

#define stringify( name ) #name
#include <vector>
class CSerialICC; // Mahedi Kamal 10172024

#include "SRC/DOSUtil.h"
class CCWLBase
{
public:

	enum CMD
	{
		MTO, SSP, ONO, OBJ, SCA, MOS, MTN, MTX, CMT, LCK, HLD, ODR, SDD, NCFG, SRZ, STH,
		PMS_LOCK, MSO, MRO, SHZ, EXP, PDT, ATR, ATS, ATI, ATP_L, ATP_E, LED, TRG, DTM,
		SFM, SAB, FTR, FTR_AVG, FTR_MID, HMM, HLV, ASC, ZRO, OFS, JDH, JDL, UNI, OFM,
		RFM, CRD, CRF, MAT, RFI, DSA, DSB, DSS, DSR, DSO, DSC, DSI, PRG, SAV, CPG, INT,
		BPG, SUS, SIF, CIF, REBOOT, ETH, NMAC, NLIP, NSNT, NGWY, NLPT, NRIP, NRPT, NSAV, DRK,
		PWD, STR, TIM, RST, DEO, ENC_AXS, ENC_MOD, ENC_PHS, ENC_ZRO, ENC_ITV, ENC_FTR,
		ENC_PST, ENC_PSP, ENC_PRE, ENC_CNT, ENC_SAV, MFO, AO_MOD, AO_NER, AO_FAR, AO_ASC,
		AO_CPE, AO_SAV, DI_PIN, DI_CTL, DI_FTR, DO_PIN, DO_CTL, DO_STT, DIO_SAV, MCO, CAL_OFM,
		CAL_K, CAL_V, CAL_O, DCO, CSD
	};
	enum CHANNEL {
		CH1 = 1, CH2, CH3, CH4
	};
	enum AXIS_ASSIGNED
	{
		All, X, Y, Z
	};

	CString _ErrorDescription = _T("");

	CString tostring(CMD cmd);
	CString CmdCompose(CMD cmd, int _p1);
	CString CmdCompose(CMD cmd, double _p1);
	CString CmdCompose(CMD cmd);
	CString CmdCompose(CMD cmd, CString _p1);
	CString CmdCompose(CMD cmd, CString _p1, int _p2);
	CString CmdCompose(CMD cmd, int _p1, CString _p2);
	CString CmdCompose(CMD cmd, int _p1, CString _p2, int _p3);
	CString CmdCompose(CMD cmd, int _p1, double _p2, double _p3);
	CString CmdCompose(CMD cmd, double _p1, double _p2, double _p3, double _p4, double _p5);
	CString CmdCompose(CMD cmd, double _p1, int _p2);
	CString CmdCompose(CMD cmd, double _p1, double _p2, double _p3, double _p4);
	CString CmdCompose(CMD cmd, int _p1, int _p2);
	CString CmdCompose(CMD cmd, int _p1, int _p2, int _p3);
	CString CmdCompose(CMD cmd, int _p1, int _p2, int _p3, int _p4);
	CString CmdCompose(CMD cmd, int _p1, double _p2, double _p3, double _p4);

	std::vector<CString> _ResultArray;

	CCWLBase() {}
	CSerialICC* m_pConfocalCom = NULL; // Mahedi Kamal 10172024
	CSerialICC* m_pConfocalCom2 = NULL; // Mahedi Kamal 10182024  // for 008
	virtual void DisplayError();
	virtual bool Connect(CString address, CListBox* pMessage);
	virtual bool ConnectRS232(int nPort, CListBox* pMessage); // Mahedi Kamal 10172024
	virtual bool Disconnect();
	virtual bool Send(CString str);
	virtual CString Receive();
	virtual bool SendAndResponse(CMD str);
	virtual bool SendAndResponse(CString str);
	virtual bool ProcessResponse(CString str);
	virtual bool GetMeasuredValueMRO(int nChan, double& dVal);
	virtual bool GetMeasuredValueMSO(int nChan, double& dVal);
	virtual bool GetMeasuredValueMFO(int nCmd, int nChan, double& dVal);
	virtual bool GetMeasuredValueMCO();
	virtual bool SetMSOFormat(int n);
	virtual bool GetMSOFormat(int& n);
	virtual bool SetZero(int nChan);
	virtual bool SetMovingAverage(int nChan, int power);
	virtual int GetMovingAverage(int nChan); // 09232025
	virtual bool SetCALKValue(float n);						// 10132022 / yukchiu
	virtual bool GetCALKValue(float& n);					// 10132022 / yukchiu

	/*
	* to do continuous data acquisition
	* CSD,1 means using serial
	* CSD,2 means using ethernet
	*/
	virtual bool StartContinuousMode();
	virtual bool StopContinuousMode();
	/*
	* consist of multiple data
	*/
	virtual bool GetContinuousData(std::vector<std::vector<CString>>& _ContinuousArray);
	/*
	* consist of only one data by MCO
	*/
	virtual bool GetContinuousMCOData(std::vector<std::vector<CString>>& _ContinuousArray);
	// [ FSMB
	virtual bool GetDataSavingQuantity(int& n);
	virtual bool GetDataSavingInterval(int& n);
	virtual bool GetDataSavingFuncStatus(int& n);
	virtual bool GetDataSavingCirculationStatus(int& n);
	virtual std::vector<CString> OutputSavedData();
	virtual std::vector<CString> OutputSavedData(CHANNEL ch);
	virtual std::vector<CString> OutputSavedData(int _startIndex, int _endIndex);
	virtual std::vector<CString> OutputEncoderData();
	virtual std::vector<CString> OutputEncoderData(AXIS_ASSIGNED _axis);
	virtual bool ClearSavedData();
	virtual bool GetDataSavedIndex(int& n);
	// ]

	//[ Tushar  //20240122
	virtual bool SetDisplacementMode(int nChannel);
	virtual bool SetAutoIntensity(int nChannnel, int mode);
	virtual bool SetAutoTunningSetpoint(int nChannnel, int value);
	//]
	virtual bool SetOFMFormat(int nChan); //24102024/MORSALIN
	virtual bool GetOFMFormat(int& n); //24102024/MORSALIN
	virtual bool SetDSAFormat(int nChan); //24102024/MORSALIN
	virtual bool GetDSAFormat(int& n); //24102024/MORSALIN
	virtual bool SetDSBFormat(int nChan); //24102024/MORSALIN
	virtual bool GetDSBFormat(int& n); //24102024/MORSALIN

private:

	const char CMDFIRSTCHAR = '$';

	//bool ReDataCheck_String(CMD cmd, CString[] _reData, CString ResultStr)
	//{
	//	ResultStr = "";
	//	if (_reData.Length == 0)
	//	{
	//		return false;
	//	}
	//	CString text = _reData[0].Trim();
	//	CString text2 = "$" + _cmdName + ",";
	//	if (text.Contains("ERR") && text.IndexOf("ERR") == 0)
	//	{
	//		_ErrorDescription = "Parameter or command error.";
	//		return false;
	//	}
	//	if (text.Contains(text2) && text.IndexOf(text2) == 0)
	//	{
	//		text = text.Remove(0, text2.Length);
	//		text = (ResultStr = text.Replace(" ", ""));
	//		return true;
	//	}
	//	_ErrorDescription = "Response error.";
	//	return false;
	//}
	//bool ReDataCheck_StringArray(CMD cmd, CString[] _reData, out CString[] Result1DArray)
	//{
	//	Result1DArray = new CString[0];
	//	if (_reData.Length == 0)
	//	{
	//		return false;
	//	}
	//	CString text = _reData[0].Trim();
	//	CString text2 = "$" + _cmdName + ",";
	//	if (text.Contains("ERR") && text.IndexOf("ERR") == 0)
	//	{
	//		_ErrorDescription = "Parameter or command error.";
	//		return false;
	//	}
	//	if (text.Contains(text2) && text.IndexOf(text2) == 0)
	//	{
	//		text = text.Remove(0, text2.Length);
	//		text = text.Replace(" ", "");
	//		Result1DArray = text.Split(',');
	//		return true;
	//	}
	//	_ErrorDescription = "Response error.";
	//	return false;
	//}
	//bool ReDataCheck_StringArray(CMD cmd, CString[] _reData, out CString[, ] Result2DArray)
	//{
	//	Result2DArray = new CString[0, 0];
	//	if (_reData.Length == 0)
	//	{
	//		return false;
	//	}
	//	CString text = _reData[0].Trim();
	//	CString text2 = "$" + _cmdName + ",";
	//	if (text.Contains("ERR") && text.IndexOf("ERR") == 0)
	//	{
	//		_ErrorDescription = "Parameter or command error.";
	//		return false;
	//	}
	//	if (text.Contains(text2) && text.IndexOf(text2) == 0)
	//	{
	//		text = text.Remove(0, text2.Length);
	//		text = text.Replace(" ", "");
	//		try
	//		{
	//			CString[] array = text.Split(new char[1] { ';' }, StringSplitOptions.RemoveEmptyEntries);
	//			CString[] array2 = array[0].Split(new char[1] { ',' }, StringSplitOptions.RemoveEmptyEntries);
	//			Result2DArray = new CString[array.Length, array2.Length];
	//			for (int i = 0; i < array.Length; i++)
	//			{
	//				array2 = array[i].Split(new char[1] { ',' }, StringSplitOptions.RemoveEmptyEntries);
	//				for (int j = 0; j < array2.Length; j++)
	//				{
	//					Result2DArray[i, j] = array2[j];
	//				}
	//			}
	//		}
	//		catch (Exception)
	//		{
	//			_ErrorDescription = "Response split error.";
	//			return false;
	//		}
	//		return true;
	//	}
	//	_ErrorDescription = "Response error.";
	//	return false;
	//}
	//bool ReDataCheck_Bool(CMD cmd, CString[] _reData)
	//{
	//	if (_reData.Length == 0)
	//	{
	//		return false;
	//	}
	//	CString text = _reData[0].Trim();
	//	CString value = "$" + _cmdName;
	//	if (text.Contains("ERR") && text.IndexOf("ERR") == 0)
	//	{
	//		_ErrorDescription = "Command error.";
	//		return false;
	//	}
	//	if (text.Equals(value))
	//	{
	//		return true;
	//	}
	//	_ErrorDescription = "Response error.";
	//	return false;
	//}
	//bool ReDataCheck_Int(CMD cmd, CString[] _reData, out int ResultInt)
	//{
	//	ResultInt = -1;
	//	if (_reData.Length == 0)
	//	{
	//		return false;
	//	}
	//	CString text = _reData[0].Trim();
	//	CString text2 = "$" + _cmdName + ",";
	//	if (text.Contains("ERR") && text.IndexOf("ERR") == 0)
	//	{
	//		_ErrorDescription = "Parameter or command error.";
	//		return false;
	//	}
	//	if (text.Contains(text2) && text.IndexOf(text2) == 0)
	//	{
	//		text = text.Remove(0, text2.Length);
	//		if (!int.TryParse(text, out ResultInt))
	//		{
	//			ResultInt = -1;
	//			_ErrorDescription = "Response tryparse error.";
	//			return false;
	//		}
	//		return true;
	//	}
	//	_ErrorDescription = "Response error.";
	//	return false;
	//}
	//bool ReDataCheck_Double(CMD cmd, CString[] _reData, out double ResultDouble)
	//{
	//	ResultDouble = -1.0;
	//	if (_reData.Length == 0)
	//	{
	//		return false;
	//	}
	//	CString text = _reData[0].Trim();
	//	CString text2 = "$" + _cmdName + ",";
	//	if (text.Contains("ERR") && text.IndexOf("ERR") == 0)
	//	{
	//		_ErrorDescription = "Parameter or command error.";
	//		return false;
	//	}
	//	if (text.Contains(text2) && text.IndexOf(text2) == 0)
	//	{
	//		text = text.Remove(0, text2.Length);
	//		if (!double.TryParse(text, out ResultDouble))
	//		{
	//			ResultDouble = -1.0;
	//			_ErrorDescription = "Response tryparse error.";
	//			return false;
	//		}
	//		return true;
	//	}
	//	_ErrorDescription = "Response error.";
	//	return false;
	//}
	//bool ReDataCheck_IntArray(CMD cmd, CString[] _reData, out int[] Result1DArray)
	//{
	//	Result1DArray = new int[0];
	//	if (_reData.Length == 0)
	//	{
	//		return false;
	//	}
	//	CString text = _reData[0].Trim();
	//	CString text2 = "$" + _cmdName + ",";
	//	if (text.Contains("ERR") && text.IndexOf("ERR") == 0)
	//	{
	//		_ErrorDescription = "Parameter or command error.";
	//		return false;
	//	}
	//	if (text.Contains(text2) && text.IndexOf(text2) == 0)
	//	{
	//		text = text.Remove(0, text2.Length);
	//		CString[] array = null;
	//		array = ((!text.Contains(";")) ? text.Split(new char[1] { ',' }, StringSplitOptions.RemoveEmptyEntries) : text.Split(new char[1] { ';' }, StringSplitOptions.RemoveEmptyEntries));
	//		Result1DArray = new int[array.Length];
	//		for (int i = 0; i < Result1DArray.Length; i++)
	//		{
	//			if (!int.TryParse(array[i], out Result1DArray[i]))
	//			{
	//				Result1DArray = new int[0];
	//				_ErrorDescription = "Response tryparse error.";
	//				return false;
	//			}
	//		}
	//		return true;
	//	}
	//	_ErrorDescription = "Response error.";
	//	return false;
	//}
	//bool ReDataCheck_IntArray(CMD cmd, CString[] _reData, out int[, ] Result2DArray)
	//{
	//	Result2DArray = new int[0, 0];
	//	if (_reData.Length == 0)
	//	{
	//		return false;
	//	}
	//	CString text = _reData[0].Trim();
	//	CString text2 = "$" + _cmdName + ",";
	//	if (text.Contains("ERR") && text.IndexOf("ERR") == 0)
	//	{
	//		_ErrorDescription = "Parameter or command error.";
	//		return false;
	//	}
	//	if (text.Contains(text2) && text.IndexOf(text2) == 0)
	//	{
	//		text = text.Remove(0, text2.Length);
	//		CString[] array = text.Split(new char[1] { ';' }, StringSplitOptions.RemoveEmptyEntries);
	//		CString[] array2 = array[0].Split(new char[1] { ',' }, StringSplitOptions.RemoveEmptyEntries);
	//		Result2DArray = new int[array.Length, array2.Length];
	//		for (int i = 0; i < array.Length; i++)
	//		{
	//			array2 = array[i].Split(new char[1] { ',' }, StringSplitOptions.RemoveEmptyEntries);
	//			for (int j = 0; j < array2.Length; j++)
	//			{
	//				if (!int.TryParse(array2[j], out Result2DArray[i, j]))
	//				{
	//					Result2DArray = new int[0, 0];
	//					_ErrorDescription = "Response tryparse error.";
	//					return false;
	//				}
	//			}
	//		}
	//		return true;
	//	}
	//	_ErrorDescription = "Response error.";
	//	return false;
	//}
	//bool ReDataCheck_ShortArray(CMD cmd, CString[] _reData, out short[] Result1DArray)
	//{
	//	Result1DArray = new short[0];
	//	if (_reData.Length == 0)
	//	{
	//		return false;
	//	}
	//	CString text = _reData[0].Trim();
	//	CString text2 = "$" + _cmdName + ",";
	//	if (text.Contains("ERR") && text.IndexOf("ERR") == 0)
	//	{
	//		_ErrorDescription = "Parameter or command error.";
	//		return false;
	//	}
	//	if (text.Contains(text2) && text.IndexOf(text2) == 0)
	//	{
	//		text = text.Remove(0, text2.Length);
	//		CString[] array = text.Split(new char[1] { ',' }, StringSplitOptions.RemoveEmptyEntries);
	//		Result1DArray = new short[array.Length];
	//		try
	//		{
	//			for (int i = 0; i < Result1DArray.Length; i++)
	//			{
	//				if (!short.TryParse(array[i], out Result1DArray[i]))
	//				{
	//					Result1DArray = new short[0];
	//					_ErrorDescription = "Response tryparse error.";
	//					return false;
	//				}
	//			}
	//		}
	//		catch (Exception)
	//		{
	//			_ErrorDescription = "Response split error.";
	//			return false;
	//		}
	//		return true;
	//	}
	//	_ErrorDescription = "Response error.";
	//	return false;
	//}
	//bool ReDataCheck_DoubleArray(CMD cmd, CString[] _reData, out double[] Result1DArray)
	//{
	//	Result1DArray = new double[0];
	//	if (_reData.Length == 0)
	//	{
	//		return false;
	//	}
	//	CString text = _reData[0].Trim();
	//	CString text2 = "$" + _cmdName + ",";
	//	if (text.Contains("ERR") && text.IndexOf("ERR") == 0)
	//	{
	//		_ErrorDescription = "Parameter or command error.";
	//		return false;
	//	}
	//	if (text.Contains(text2) && text.IndexOf(text2) == 0)
	//	{
	//		text = text.Remove(0, text2.Length);
	//		CString[] array = text.Split(new char[1] { ',' }, StringSplitOptions.RemoveEmptyEntries);
	//		Result1DArray = new double[array.Length];
	//		try
	//		{
	//			for (int i = 0; i < Result1DArray.Length; i++)
	//			{
	//				if (!double.TryParse(array[i], out Result1DArray[i]))
	//				{
	//					Result1DArray = new double[0];
	//					_ErrorDescription = "Response tryparse error.";
	//					return false;
	//				}
	//			}
	//		}
	//		catch (Exception)
	//		{
	//			_ErrorDescription = "Response split error.";
	//			return false;
	//		}
	//		return true;
	//	}
	//	_ErrorDescription = "Response error.";
	//	return false;
	//}
	//bool ReDataCheck_DoubleArray(CMD cmd, CString[] _reData, out double[, ] Result2DArray)
	//{
	//	Result2DArray = new double[0, 0];
	//	if (_reData.Length == 0)
	//	{
	//		return false;
	//	}
	//	CString text = _reData[0].Trim();
	//	CString text2 = "$" + _cmdName + ",";
	//	if (text.Contains("ERR") && text.IndexOf("ERR") == 0)
	//	{
	//		_ErrorDescription = "Parameter or command error.";
	//		return false;
	//	}
	//	if (text.Contains(text2) && text.IndexOf(text2) == 0)
	//	{
	//		text = text.Remove(0, text2.Length);
	//		try
	//		{
	//			CString[] array = text.Split(new char[1] { ';' }, StringSplitOptions.RemoveEmptyEntries);
	//			CString[] array2 = array[0].Split(new char[1] { ',' }, StringSplitOptions.RemoveEmptyEntries);
	//			Result2DArray = new double[array.Length, array2.Length];
	//			for (int i = 0; i < array.Length; i++)
	//			{
	//				array2 = array[i].Split(new char[1] { ',' }, StringSplitOptions.RemoveEmptyEntries);
	//				for (int j = 0; j < array2.Length; j++)
	//				{
	//					if (!double.TryParse(array2[j], out Result2DArray[i, j]))
	//					{
	//						Result2DArray = new double[0, 0];
	//						_ErrorDescription = "Response tryparse error.";
	//						return false;
	//					}
	//				}
	//			}
	//		}
	//		catch (Exception)
	//		{
	//			_ErrorDescription = "Response split error.";
	//			return false;
	//		}
	//		return true;
	//	}
	//	_ErrorDescription = "Response error.";
	//	return false;
	//}
	//bool ReDataCheck_NCFG(CString[] _reData, out CString[] Result1DArray)
	//{
	//	Result1DArray = new CString[0];
	//	if (_reData.Length == 0)
	//	{
	//		return false;
	//	}
	//	CString text = _reData[0].Trim();
	//	if (text.Contains("ERR") && text.IndexOf("ERR") == 0)
	//	{
	//		_ErrorDescription = "Parameter or command error.";
	//		return false;
	//	}
	//	if (_reData.Length == 6)
	//	{
	//		try
	//		{
	//			Result1DArray = new CString[6];
	//			for (int i = 0; i < 6; i++)
	//			{
	//				CString[] array = _reData[i].Trim().Replace(" ", "").Split(new char[1] { ':' }, StringSplitOptions.RemoveEmptyEntries);
	//				if (array.Length != 2)
	//				{
	//					_ErrorDescription = "Response error.";
	//					Result1DArray = new CString[0];
	//					return false;
	//				}
	//				Result1DArray[i] = array[1];
	//			}
	//		}
	//		catch (Exception)
	//		{
	//			_ErrorDescription = "Response analyze error.";
	//			Result1DArray = new CString[0];
	//			return false;
	//		}
	//		return true;
	//	}
	//	if (_reData.Length == 1)
	//	{
	//		try
	//		{
	//			CString[] array2 = text.Replace(" ", "").Split(new char[1] { ';' }, StringSplitOptions.RemoveEmptyEntries);
	//			if (array2.Length != 6)
	//			{
	//				_ErrorDescription = "Response error.";
	//				return false;
	//			}
	//			Result1DArray = new CString[6];
	//			for (int j = 0; j < 6; j++)
	//			{
	//				CString[] array3 = array2[j].Split(new char[1] { ':' }, StringSplitOptions.RemoveEmptyEntries);
	//				if (array3.Length != 2)
	//				{
	//					_ErrorDescription = "Response error.";
	//					Result1DArray = new CString[0];
	//					return false;
	//				}
	//				Result1DArray[j] = array3[1];
	//			}
	//		}
	//		catch (Exception)
	//		{
	//			_ErrorDescription = "Response analyze error.";
	//			Result1DArray = new CString[0];
	//			return false;
	//		}
	//		return true;
	//	}
	//	_ErrorDescription = "Response error.";
	//	return false;
	//}
	//bool ReDataCheck_REBOOT(CMD cmd, CString[] _reData)
	//{
	//	if (_reData.Length == 0)
	//	{
	//		return false;
	//	}
	//	CString text = _reData[0].Trim();
	//	if (text.Contains("ERR") && text.IndexOf("ERR") == 0)
	//	{
	//		_ErrorDescription = "Parameter or command error.";
	//		return false;
	//	}
	//	if (_reData.Length == ResponseLineNum[_cmdName])
	//	{
	//		return true;
	//	}
	//	_ErrorDescription = "Response split error.";
	//	return false;
	//}
};
