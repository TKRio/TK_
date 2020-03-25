#include "Motion.h"
#include "dx11mathutil.h"

//--------------------KeyFrameClass----------------------
//Init
void KeyFrame::Init(unsigned int partsnum) {
	m_qt = new DirectX::XMFLOAT4[partsnum];
	m_pos = new DirectX::XMFLOAT3[partsnum];
	m_partsnum = partsnum;
}
//Exit
void KeyFrame::Exit() {
	if (m_qt)delete[] m_qt;
	if (m_pos)delete[] m_pos;
}
//--------------------------------------------------------

//--------------------MotionClass-------------------------
//Init
void Motion::Init(unsigned int partsnum) {
	m_partsnum = partsnum;
}
//Exit
void Motion::Exit() {
	Clear();
}

//���
void Motion::Interpolated(KeyFrame* startkeyframe, KeyFrame* endkeyframe, unsigned int frameno, KeyFrame* nowkeyframe) {
	float rate;		//����

	float framesu = static_cast<float>((endkeyframe->GetFrameNo() - startkeyframe->GetFrameNo() + 1));
	float nowframe = static_cast<float>(frameno - startkeyframe->GetFrameNo());

	//�����̌v�Z
	rate = nowframe / framesu;

	//�L�[�t���[���̍쐬
	for (unsigned int i = 0; i < nowkeyframe->GetPartsNum(); i++) {
		DirectX::XMFLOAT4 q;	//�N�I�[�^�j�I��
		DirectX::XMFLOAT3 pos;	//�ʒu	

		//�X���[�v���	
		DX11QtSlerp(startkeyframe->GetQt(i), endkeyframe->GetQt(i), rate, q);

		//�N�I�[�^�j�I�����Z�b�g
		nowkeyframe->SetQt(i, q);

		//�ʒu�̐��`���
		DirectX::XMFLOAT3 startpos;
		DirectX::XMFLOAT3 endpos;
		startpos = startkeyframe->GetPos(i);
		endpos = endkeyframe->GetPos(i);

		pos.x = startpos.x + (endpos.x - startpos.x) * rate;	//�����I�y���[�^�[�ŏȗ��o������
		pos.y = startpos.y + (endpos.y - startpos.y) * rate;
		pos.z = startpos.z + (endpos.z - startpos.z) * rate;

		//�ʒu���Z�b�g
		nowkeyframe->SetPos(i, pos);
	}//end for
	//�t���[���ԍ��Z�b�g
	nowkeyframe->SetFrameNo(frameno);
}

//�L�[�t���[�������ׂč폜
void Motion::Clear() {
	std::vector<KeyFrame*>::iterator it;
	for (it = m_KeyFrameVector.begin(); it != m_KeyFrameVector.end();) {
		delete *it;
		it = m_KeyFrameVector.erase(it);
		continue;
	}//end for
}

//�L�[�t���[���̃Z�b�g
void Motion::SetKeyFrame(unsigned int frameno, DirectX::XMFLOAT4X4* matrix) {
	KeyFrame* kf;
	DirectX::XMFLOAT4 q;
	DirectX::XMFLOAT3 pos;

	kf = new KeyFrame();
	kf->Init(m_partsnum);

	kf->SetFrameNo(frameno);		//�t���[���ԍ��Z�b�g
	for (unsigned int i = 0; i < m_partsnum; i++) {
		DX11GetQtfromMatrix(matrix[i], q);		//�s�񂩂�N�I�[�^�j�I�������
		kf->SetQt(i, q);

		pos.x = matrix[i]._41;		//�I�y���[�^�[��(ry
		pos.y = matrix[i]._42;
		pos.z = matrix[i]._43;
		kf->SetPos(i, pos);
	}//end for
	m_KeyFrameVector.push_back(kf);		//�L�[�t���[���̕ۑ�
}

//���[�V�����I�����Ă�H
bool Motion::IsEnd(unsigned int frame) {
	if (frame > m_KeyFrameVector[m_KeyFrameVector.size() - 1]->GetFrameNo()) {
		return true;
	}//end if
	return false;
}
// �t���[���ԍ��ɊY������L�[�t���[�����擾����
KeyFrame* Motion::GetKeyFrame(unsigned int frameno) {
	KeyFrame* kf = nullptr;
	std::vector<KeyFrame*>::iterator it;		//frame������vector��p��
	KeyFrame* oldkeyflame = nullptr;

	//�Y������L�[�t���[���ɑΉ������L�[�t���[������������
	for (it = m_KeyFrameVector.begin(); it != m_KeyFrameVector.end(); it++) {
		if ((*it)->GetFrameNo() >= frameno) {
			if ((*it)->GetFrameNo() == frameno) {
				kf = *it;
			}
			else {
				kf = new KeyFrame();
				kf->Init(m_partsnum);
				//��Ԃ��ăL�[�t���[�����v�Z
				Interpolated(oldkeyflame, (*it), frameno, kf);
			}//end if in
			break;
		}//end if out
		//�ЂƂO�̃L�[�t���[�������Z�[�u
		oldkeyflame = *it;
	}//end for
	return kf;
}

//���[�V�����̃��[�h
void Motion::LoadMotion() {
	Clear();

	FILE* fp;

	fopen_s(&fp, "motion.txt", "r");
	fscanf_s(fp, "%d", &m_partsnum);		//�p�[�c��

	unsigned int KeyFrameNum;
	fscanf_s(fp, "%d", &KeyFrameNum);	//�L�[�t���[����

	unsigned int frameno;

	for (unsigned int i = 0; i < KeyFrameNum; i++) {
		//�L�[�t���[���쐬
		KeyFrame* kf = new KeyFrame();
		kf->Init(m_partsnum);

		//�t���[���ԍ����Z�b�g
		fscanf_s(fp, "%d", &frameno);
		kf->SetFrameNo(frameno);
		DirectX::XMFLOAT4 qt;
		DirectX::XMFLOAT3 pos;
		for (unsigned int j = 0; j < m_partsnum; j++) {
			fscanf_s(fp, "%f,%f,%f,%f", &qt.x, &qt.y, &qt.z, &qt.w);
			kf->SetQt(j, qt);
			fscanf_s(fp, "%f,%f,%f", &pos.x, &pos.y, &pos.z);
			kf->SetPos(j, pos);
		}//end for in
		m_KeyFrameVector.push_back(kf);
	}//end for out
	fclose(fp);
}

//�V���A���C�Y�֐�
void Motion::Serilaize() {
	FILE* fp;
	fopen_s(&fp, "motion.txt", "w");
	fprintf_s(fp, "%d\n", m_partsnum);						//�p�[�c��

	fprintf_s(fp, "%d\n", m_KeyFrameVector.size());			//�L�[�t���[����

	for (unsigned int i = 0; i < m_KeyFrameVector.size(); i++) {
		fprintf_s(fp, "%d\n", m_KeyFrameVector[i]->GetFrameNo());	//�t���[���ԍ�
		for (unsigned int j = 0; j < m_partsnum; j++) {
			DirectX::XMFLOAT4 qt = m_KeyFrameVector[i]->GetQt(j);
			fprintf_s(fp, "%f,%f,%f,%f\n", qt.x, qt.y, qt.z, qt.w);	//qt
			DirectX::XMFLOAT3 pos = m_KeyFrameVector[i]->GetPos(j);
			fprintf_s(fp, "%f,f,f\n", pos.x, pos.y, pos.z);
		}//end for in
	}//end for out
	fclose(fp);
}
//--------------------------------------------------------
