#pragma once
#include <d3d11.h>
#include <vector>
#include <DirectXMath.h>

class KeyFrame {
private:
	unsigned int			m_frameNo = 0;			//�t���[���ԍ�
	DirectX::XMFLOAT4*		m_qt = nullptr;
	DirectX::XMFLOAT3*		m_pos = nullptr;
	unsigned int			m_partsnum = 0;
public:
	void Init(unsigned int partsnum);					//��������
	void Exit();										//�I������

	//Getter
	unsigned int GetFrameNo() const {						//�t���[���ԍ��̎擾
		return m_frameNo;
	}
	DirectX::XMFLOAT4 GetQt(unsigned int partno) const {	//�p��(�N�I�[�^�j�I��)���擾
		return m_qt[partno];
	}
	DirectX::XMFLOAT3 GetPos(unsigned int partno) const {	//�ʒu���擾
		return m_pos[partno];
	}
	unsigned int GetPartsNum() const {						//�p�[�c�����擾
		return m_partsnum;
	}

	//Settre
	void SetFrameNo(unsigned int no) {									//�t���[���ԍ����Z�b�g
		m_frameNo = no;
	}
	void SetQt(unsigned int partno, const DirectX::XMFLOAT4& qt) {		//�p��(�N�I�[�^�j�I��)���Z�b�g
		m_qt[partno] = qt;
	}
	void SetPos(unsigned int partno, const DirectX::XMFLOAT3& pos) {	//�ʒu���Z�b�g
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
	void Interpolated(KeyFrame* startkeyframe, KeyFrame* endkeyframe, unsigned int frameno, KeyFrame* nowkeyframe);	//���

	//Getter
	KeyFrame* GetKeyFrame(unsigned int frameno);
	unsigned int GetKeyFrameNum() {
		return static_cast<unsigned int>(m_KeyFrameVector.size());
	}

	//Setter
	void SetKeyFrame(unsigned int frameno, DirectX::XMFLOAT4X4* matrix);
	void SetPartsNum(unsigned int partsnum) {				//�p�[�c�̑������Z�b�g
		m_partsnum = partsnum;
	}

	void Serilaize();
	void Clear();											//�L�[�t���[�������ׂč폜����
	bool IsEnd(unsigned int frame);
	void LoadMotion();
};