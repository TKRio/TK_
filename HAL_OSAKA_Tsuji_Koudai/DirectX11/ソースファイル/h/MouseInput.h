#pragma once

typedef struct MOUSEPOINTERPOS {
	long x;
	long y;
}PointerPos;

class Mouse {
private:
	bool lButtonClick;
	bool rButtonClick;
	bool wButtonClick;

	int  wheelUpDown; //ホイールの上下を検知
	PointerPos pos;
public :
	Mouse() {
	}
	Mouse(const Mouse&) = delete;
	Mouse& operator=(const Mouse&) = delete;
	Mouse(Mouse&&) = delete;
	Mouse& operator=(Mouse&&) = delete;
	//インスタンス化
	static Mouse* GetInstance() {
		static Mouse instance;
		return &instance;
	}
	//Getter
	bool GetLButtonClick() {
		return lButtonClick;
	}
	bool GetRButtonClick() {
		return rButtonClick;
	}
	bool GetWButtonClick() {
		return wButtonClick;
	}
	int  GetWheelUpDown() {
		return wheelUpDown;
	}
	long GetPointerX() {
		return pos.x;
	}
	long GetPointerY() {
		return pos.y;
	}
	//Setter
	void SetLButtonClick(bool clickFlag) {
		lButtonClick = clickFlag;
	}
	void SetRButtonClick(bool clickFlag) {
		rButtonClick = clickFlag;
	}
	void SetWButtonClick(bool clickFlag) {
		wButtonClick = clickFlag;
	}
	void SetWheelUpDown(int wheel) {
		wheelUpDown = wheel;
	}
	void SetPointerPos(long posx, long posy) {
		pos.x = posx;
		pos.y = posy;
	}
};
