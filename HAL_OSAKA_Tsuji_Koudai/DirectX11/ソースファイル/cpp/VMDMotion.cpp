//*****************************************************************************
//!	@file	VMDMotion.cpp
//!	@brief	VMDFile�����f���ɓK�������ē�����
//!	@note	
//!	@author	
//*****************************************************************************
#include "VMDMotion.h"
#include <windows.h>

bool Motion::cVmd::Init(const char* fileName) {
    motionData = new VMDFileLoader::VMD(fileName);
	if (motionData == nullptr) {
		MessageBox(NULL, "MotionData NoneError", "Error", MB_OK);
		return false;
	}
	return true;
}