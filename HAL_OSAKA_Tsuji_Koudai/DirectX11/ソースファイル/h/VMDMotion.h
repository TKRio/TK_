//*****************************************************************************
//!	@file	VMDMotion.h
//!	@brief	VMD���Đ����邽�߂̃N���X
//!	@note
//!	@author
//*****************************************************************************
#pragma once
#include "VMDFileLoader.h"

namespace Motion {
	class cVmd {
	private:
		VMDFileLoader::VMD*		motionData;		//���[�V�����f�[�^
	public:
		bool Init(const char* fileName);
		bool Uninit();
		void Update();
	};
}