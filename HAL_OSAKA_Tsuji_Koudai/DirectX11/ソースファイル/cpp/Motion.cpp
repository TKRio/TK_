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

//補間
void Motion::Interpolated(KeyFrame* startkeyframe, KeyFrame* endkeyframe, unsigned int frameno, KeyFrame* nowkeyframe) {
	float rate;		//割合

	float framesu = static_cast<float>((endkeyframe->GetFrameNo() - startkeyframe->GetFrameNo() + 1));
	float nowframe = static_cast<float>(frameno - startkeyframe->GetFrameNo());

	//割合の計算
	rate = nowframe / framesu;

	//キーフレームの作成
	for (unsigned int i = 0; i < nowkeyframe->GetPartsNum(); i++) {
		DirectX::XMFLOAT4 q;	//クオータニオン
		DirectX::XMFLOAT3 pos;	//位置	

		//スラープ補間	
		DX11QtSlerp(startkeyframe->GetQt(i), endkeyframe->GetQt(i), rate, q);

		//クオータニオンをセット
		nowkeyframe->SetQt(i, q);

		//位置の線形補間
		DirectX::XMFLOAT3 startpos;
		DirectX::XMFLOAT3 endpos;
		startpos = startkeyframe->GetPos(i);
		endpos = endkeyframe->GetPos(i);

		pos.x = startpos.x + (endpos.x - startpos.x) * rate;	//ここオペレーターで省略出来そう
		pos.y = startpos.y + (endpos.y - startpos.y) * rate;
		pos.z = startpos.z + (endpos.z - startpos.z) * rate;

		//位置をセット
		nowkeyframe->SetPos(i, pos);
	}//end for
	//フレーム番号セット
	nowkeyframe->SetFrameNo(frameno);
}

//キーフレームをすべて削除
void Motion::Clear() {
	std::vector<KeyFrame*>::iterator it;
	for (it = m_KeyFrameVector.begin(); it != m_KeyFrameVector.end();) {
		delete *it;
		it = m_KeyFrameVector.erase(it);
		continue;
	}//end for
}

//キーフレームのセット
void Motion::SetKeyFrame(unsigned int frameno, DirectX::XMFLOAT4X4* matrix) {
	KeyFrame* kf;
	DirectX::XMFLOAT4 q;
	DirectX::XMFLOAT3 pos;

	kf = new KeyFrame();
	kf->Init(m_partsnum);

	kf->SetFrameNo(frameno);		//フレーム番号セット
	for (unsigned int i = 0; i < m_partsnum; i++) {
		DX11GetQtfromMatrix(matrix[i], q);		//行列からクオータニオンを作る
		kf->SetQt(i, q);

		pos.x = matrix[i]._41;		//オペレーターで(ry
		pos.y = matrix[i]._42;
		pos.z = matrix[i]._43;
		kf->SetPos(i, pos);
	}//end for
	m_KeyFrameVector.push_back(kf);		//キーフレームの保存
}

//モーション終了してる？
bool Motion::IsEnd(unsigned int frame) {
	if (frame > m_KeyFrameVector[m_KeyFrameVector.size() - 1]->GetFrameNo()) {
		return true;
	}//end if
	return false;
}
// フレーム番号に該当するキーフレームを取得する
KeyFrame* Motion::GetKeyFrame(unsigned int frameno) {
	KeyFrame* kf = nullptr;
	std::vector<KeyFrame*>::iterator it;		//frame数分のvectorを用意
	KeyFrame* oldkeyflame = nullptr;

	//該当するキーフレームに対応したキーフレーム情報を見つける
	for (it = m_KeyFrameVector.begin(); it != m_KeyFrameVector.end(); it++) {
		if ((*it)->GetFrameNo() >= frameno) {
			if ((*it)->GetFrameNo() == frameno) {
				kf = *it;
			}
			else {
				kf = new KeyFrame();
				kf->Init(m_partsnum);
				//補間してキーフレームを計算
				Interpolated(oldkeyflame, (*it), frameno, kf);
			}//end if in
			break;
		}//end if out
		//ひとつ前のキーフレーム情報をセーブ
		oldkeyflame = *it;
	}//end for
	return kf;
}

//モーションのロード
void Motion::LoadMotion() {
	Clear();

	FILE* fp;

	fopen_s(&fp, "motion.txt", "r");
	fscanf_s(fp, "%d", &m_partsnum);		//パーツ数

	unsigned int KeyFrameNum;
	fscanf_s(fp, "%d", &KeyFrameNum);	//キーフレーム数

	unsigned int frameno;

	for (unsigned int i = 0; i < KeyFrameNum; i++) {
		//キーフレーム作成
		KeyFrame* kf = new KeyFrame();
		kf->Init(m_partsnum);

		//フレーム番号をセット
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

//シリアライズ関数
void Motion::Serilaize() {
	FILE* fp;
	fopen_s(&fp, "motion.txt", "w");
	fprintf_s(fp, "%d\n", m_partsnum);						//パーツ数

	fprintf_s(fp, "%d\n", m_KeyFrameVector.size());			//キーフレーム数

	for (unsigned int i = 0; i < m_KeyFrameVector.size(); i++) {
		fprintf_s(fp, "%d\n", m_KeyFrameVector[i]->GetFrameNo());	//フレーム番号
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
