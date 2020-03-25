#pragma once
#include <d3d11.h>
#include <vector>
#include <DirectXMath.h>

class KeyFrame {
private:
	unsigned int			m_frameNo = 0;			//フレーム番号
	DirectX::XMFLOAT4*		m_qt = nullptr;
	DirectX::XMFLOAT3*		m_pos = nullptr;
	unsigned int			m_partsnum = 0;
public:
	void Init(unsigned int partsnum);					//初期処理
	void Exit();										//終了処理

	//Getter
	unsigned int GetFrameNo() const {						//フレーム番号の取得
		return m_frameNo;
	}
	DirectX::XMFLOAT4 GetQt(unsigned int partno) const {	//姿勢(クオータニオン)を取得
		return m_qt[partno];
	}
	DirectX::XMFLOAT3 GetPos(unsigned int partno) const {	//位置を取得
		return m_pos[partno];
	}
	unsigned int GetPartsNum() const {						//パーツ数を取得
		return m_partsnum;
	}

	//Settre
	void SetFrameNo(unsigned int no) {									//フレーム番号をセット
		m_frameNo = no;
	}
	void SetQt(unsigned int partno, const DirectX::XMFLOAT4& qt) {		//姿勢(クオータニオン)をセット
		m_qt[partno] = qt;
	}
	void SetPos(unsigned int partno, const DirectX::XMFLOAT3& pos) {	//位置をセット
		m_pos[partno] = pos;
	}
};

class Motion {
private:
	std::vector<KeyFrame*> m_KeyFrameVector;
	unsigned int m_partsnum = 0;
public:
	void Init(unsigned int partsnum);
	void Exit();
	void Interpolated(KeyFrame* startkeyframe, KeyFrame* endkeyframe, unsigned int frameno, KeyFrame* nowkeyframe);	//補間

	//Getter
	KeyFrame* GetKeyFrame(unsigned int frameno);
	unsigned int GetKeyFrameNum() {
		return static_cast<unsigned int>(m_KeyFrameVector.size());
	}

	//Setter
	void SetKeyFrame(unsigned int frameno, DirectX::XMFLOAT4X4* matrix);
	void SetPartsNum(unsigned int partsnum) {				//パーツの総数をセット
		m_partsnum = partsnum;
	}

	void Serilaize();
	void Clear();											//キーフレームをすべて削除する
	bool IsEnd(unsigned int frame);
	void LoadMotion();
};