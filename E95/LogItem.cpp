// LogItem.cpp: implementation of the CLogItem class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "LogItem.h"
#include "..\ONLINE\GEM\Annotator.h"
#include "..\ONLINE\GEM\SecsMessage.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CLogItem::CLogItem() {
	mLen = 0;
	message = NULL;
}

CLogItem::~CLogItem() {
	mLen = 0;
	if (message) {
		delete[] message;
	}
}

void CLogItem::Set(BYTE* buf, short len) {
	mLen = 0;
	if (message) {
		delete[] message;
	}
	if (len > 0) {
		message = new BYTE[len + 8];
		if (message) {
			mLen = len;
			memcpy(message, buf, len);
			message[len] = 0;
		}
	}
}

// char * CLogItem::GetMsg()
// {
// 	return message;
// }

// return TRUE for S6F11 only [2/13/2012 Administrator]
BOOL CLogItem::SML(std::string& XML) {
	SecsMessage Msg;
	Msg.DecodeMessage(message + 10, mLen - 10);
	Msg.PrintSML(XML);
	if (XML.size() == 0) {
		XML = "<Header only>";
	}
	unsigned char Function, Stream;
	Function = message[3];
	Stream = *(message + 2) & 0x7f;
	CString hdr;
	hdr.Format("S%dF%d ", Stream, Function);

	UINT SysByte;
	char Buf[26];
	memcpy(&SysByte, message + 6, 4);
	Msg.ByteSwap(&SysByte, 4);
	sprintf(Buf, "[%d] %s\r\n", SysByte, (*(message + 2) & 0x80) ? "Wait" : "");

	hdr += Buf;

	XML = hdr.GetBuffer(0) + XML;
	Annot.Annotate(Msg, Stream, Function, XML);
	return TRUE;
}

//BOOL CLogItem::Append(CString &fname)
//{
// 	string XML;
// 	if (SML(XML))
// 	{
// 		FILE *fp = fopen(fname,"a+b");
// 		if (fp)
// 		{
// 			fprintf(fp,"%s\r\n",XML.c_str());
// 			fflush(fp);
// 			fclose(fp);
// 			return TRUE;
// 		}
// 	}
// 	return FALSE;
//}

BOOL CLogItem::Append(BYTE* buf, short len, CString& fname, BOOL bLcl, short* pTime) {
	BOOL ret = FALSE;
	if (message) {
		delete[] message;
		message = NULL;
	}
	mLen = len;
	message = buf;
	string XML, HDR;
	if (SML(XML)) {
		if (bLcl) {
			HDR = "E ";
		}
		else {
			HDR = "H ";
		}
		Header(HDR);
		FILE* fp = fopen(fname, "a+b");
		if (fp) {
			fprintf(fp, "%s (%d)\r\n", HDR.c_str(), len);
			if (pTime) {
				for (int i = 0; i < 16; i++) {
					if (*pTime == -1) {
						break;
					}
					else {
						fprintf(fp, "%d (%d) ms  ", *(pTime), *(pTime + 1)); pTime += 2;
					}
				}
				fprintf(fp, "\r\n");
			}
			fprintf(fp, "%s\r\n---------------------------------------------\r\n\r\n", XML.c_str());
			fflush(fp);
			fclose(fp);
			ret = TRUE;
		}
	}
	mLen = 0;
	message = NULL;
	return ret;
}

void CLogItem::Header(string& HDR) {
	if (!message) {
		return;
	}
	CString str;
	unsigned char* p = (unsigned char*)message;
	str.Format("%02X ", *(p++));
	HDR += str;
	str.Format("%02X ", *(p++));
	HDR += str;
	str.Format("[%02X ", *(p++) & 0x7F);
	HDR += str;
	str.Format("%02X] ", *(p++));
	HDR += str;
	str.Format("%02X ", *(p++));
	HDR += str;
	str.Format("%02X ", *(p++));
	HDR += str;
	str.Format("[%02X ", *(p++));
	HDR += str;
	str.Format("%02X ", *(p++));
	HDR += str;
	str.Format("%02X ", *(p++));
	HDR += str;
	str.Format("%02X]", *(p++));
	HDR += str;

	BYTE ch = message[5];
	if (ch != 0) {
		switch (ch) {
		case 1:
			HDR += " SELECT.REQ";
			break;
		case 2:
			HDR += " SELECT.RSP";
			break;
		case 3:
			HDR += " DESELECT.REQ";
			break;
		case 4:
			HDR += " DESELECT.RSP";
			break;
		case 5:
			HDR += " LINKTEST.REQ";
			break;
		case 6:
			HDR += " LINKTEST.RSP";
			break;
		case 7:
			HDR += " REJECT.REQ";
			break;
		case 9:
			HDR += " SEPARATE.REQ";
			break;
		}
	}
}

// Dump pData in HEX for len characters [8/12/2012 Administrator]
void CLogItem::Record(BYTE* pData, short len, CString& LogFile, char* pMsg, short* pTime) {
	if (len < 1) {
		return;
	}
	short ll = len;
	if (len > 128) {
		len = 128;
	}
	int i;
	FILE* fp = fopen(LogFile, "a+b");
	if (fp) {
		fprintf(fp, "%s (%d)\r\n", pMsg, ll);
		for (i = 0; i < len; i++) {
			fprintf(fp, "%02X ", *(pData++));
		}
		if (pTime) {
			fprintf(fp, "\r\n");
			for (i = 0; i < 16; i++) {
				if (*pTime == -1) {
					break;
				}
				else {
					fprintf(fp, "%d (%d) ms  ", *(pTime), *(pTime + 1)); pTime += 2;
				}
			}
		}
		fprintf(fp, "\r\n---------------------------------------------\r\n\r\n");
		fflush(fp);
		fclose(fp);
	}
}