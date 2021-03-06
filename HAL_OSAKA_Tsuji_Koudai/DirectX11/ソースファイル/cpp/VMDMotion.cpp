//*****************************************************************************
//!	@file	VMDMotion.cpp
//!	@brief	VMDFileをモデルに適応させて動かす
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