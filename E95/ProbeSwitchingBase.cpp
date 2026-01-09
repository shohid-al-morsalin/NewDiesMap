
//02082024/MORSALIN/CProbeSwitchingBase c++
#include <stdafx.h>
#include <ProbeSwitchingBase.h>
#include "413App.h"

CProbeSwitchingBase::CProbeSwitchingBase() {

}

BOOL CProbeSwitchingBase::Connect(int comPort, int transRate, char parity, int stopBit) {
	return TRUE;
}
void CProbeSwitchingBase::Disconnect() {
	return;
}
CString CProbeSwitchingBase::Cal_CRC(std::string stream) {
	return NULL;
}
void CProbeSwitchingBase::Cal_OneCRC(vector<long long> dataArray) {
	return;
}
BOOL CProbeSwitchingBase::ExecuteCommand(CString cmd) {
	return TRUE;
}
BOOL CProbeSwitchingBase::SendCommand(CString SendStream) {
	return TRUE;
}
BOOL CProbeSwitchingBase::InitMovingParameters(int steps) {
	return TRUE;
}
BOOL CProbeSwitchingBase::MoveR(int steps)
{
	return TRUE;
}
BOOL CProbeSwitchingBase::MoveA(int steps)
{
	return TRUE;
}
BOOL CProbeSwitchingBase::MoveToECHOprb() {
	return TRUE;
}
BOOL CProbeSwitchingBase::MoveToCWL008prb() { // 10172024 Morsalin
	return TRUE;
}
BOOL CProbeSwitchingBase::MoveToCWL030prb() { // 10172024 Morsalin
	return TRUE;
}
BOOL CProbeSwitchingBase::MoveToROUGHNESSprb() {
	return TRUE;
}
BOOL CProbeSwitchingBase::CurrentPosition() {
	return TRUE;
}
BOOL CProbeSwitchingBase::ResetAlarm() {
	return TRUE;
}
BOOL CProbeSwitchingBase::toEchoOffset() {
	return TRUE;
}
BOOL CProbeSwitchingBase::toCWLOffset() {
	return TRUE;
}