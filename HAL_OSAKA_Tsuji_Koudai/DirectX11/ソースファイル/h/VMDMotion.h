//*****************************************************************************
//!	@file	VMDMotion.h
//!	@brief	VMDを再生するためのクラス
//!	@note
//!	@author
//*****************************************************************************
#pragma once
#include "VMDFileLoader.h"

namespace Motion {
	class cVmd {
	private:
		VMDFileLoader::VMD*		motionData;		//モーションデータ
	public:
		bool Init(const char* fileName);
		bool Uninit();
		void Update();
	};
}