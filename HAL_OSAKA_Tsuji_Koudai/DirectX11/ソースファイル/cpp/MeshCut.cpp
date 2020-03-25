#include "Player.h"
#include "MeshCut.h"
#include <iostream>

using namespace DirectX;

void MMDBreaker::Cutter::Init(PMDModel* cutter, PMDModel* target,XMFLOAT4X4 cuttingMtx, XMFLOAT4X4 targetMtx) {
	
	pMtx = targetMtx;						//�ؒf����鑤�̃��[���h�ϊ����W
	cMtx = cuttingMtx;						//�ؒf���鑤�̃��[���h�ϊ����W

	model[0] = cutter;						//�ؒf���邽�߂�object(�ł�����A���b�V���ł�����)
	model[1] = target;						//�ؒf�����object
	
	//�ؒf����鑤�̃I�u�W�F�N�g�̏������o��	
	for (i = 0; i < model[1]->GetVertexCount(); i++) {
		provi[0].vertsPos = Triangle(model[1]->GetVertexData(i).position, pMtx); // vertex�F���_
		provi[0].normal   = model[1]->GetVertexData(i).nomal;	// �@��
		provi[0].color	  = XMFLOAT4{0.0f,0.0f,0.0f,0.0f};		// �J���[
		provi[0].uv	      = model[1]->GetVertexData(i).uv;		// UV���W

		workMesh.push_back(provi[0]);
	}

	auto indexCount = model[1]->GetIndexCount();
	for (auto j = 0; j < indexCount; j++) {
		workMeshIndex.push_back(model[1]->GetModelData()->GetfaceVertIndex(j));			//�C���f�b�N�X�f�[�^
	}
	colorCount = model[1]->GetColorCount();

	//���ʂ̒�`
	XMFLOAT3 vertexAbc[planemax];							//ABC�����߂�
	for (i = 0; i < planemax; i++) {
		pos[i] = model[0]->GetModelData()->GetPmdVertexPos(
				 model[0]->GetModelData()->GetfaceVertIndex(i));
		vertexAbc[i].x = pos[i].x;
		vertexAbc[i].y = pos[i].y;
		vertexAbc[i].z = pos[i].z;
		DX11Vec3MulMatrix(vertexAbc[i], vertexAbc[i], cMtx);
	}

	//���o�������_�����g��,3D��Ԃ𕪒f����p�l���i���ԂȂ��j���쐬����
	cuttingObjPlane.CreatePlaneFromPorigon(vertexAbc[0], vertexAbc[1], vertexAbc[2]);
	XMFLOAT3 no = cuttingObjPlane.GetNormal();

	//�㑤mesh�Ɖ���mesh�ɕ�����
	for (i = 0; i < clonemax; i++) {
		for (int j = 0; j < static_cast<int>(workMesh.size()); j++) {
			provi[i].vertsPos = workMesh[j].vertsPos;
			provi[i].uv = workMesh[j].uv;
			provi[i].color = workMesh[j].color;
			provi[i].normal = workMesh[j].normal;

			clone[i].push_back(provi[i]);
		}
	}
}

//***************************************************************************
// MeshCut
//***************************************************************************
void MMDBreaker::Cutter::MeshCut() {
	for (unsigned int i = 0; i < workMeshIndex.size(); i += 3) {

		//�Z�b�g���ꂽ���_V���A�ؒf�p�p�l���̖@�����ɂ��邩�ǂ���
		if (cuttingObjPlane.GetSide(Triangle(i)) &&
			cuttingObjPlane.GetSide(Triangle(i + 1)) &&
			cuttingObjPlane.GetSide(Triangle(i + 2)))
		{
			for (int j = 0; j < 3; j++) {
				//�S�Ă����ł���Β��_�̒ǉ�
				provi[0].vertsPos = Triangle(i + j);
				clone[0].push_back(provi[0]);
				outSiderIndex[0].push_back(static_cast<WORD>(clone[0].size() - 1));
			}
		}
		else if (!cuttingObjPlane.GetSide(Triangle(i)) &&
				 !cuttingObjPlane.GetSide(Triangle(i + 1)) &&
				 !cuttingObjPlane.GetSide(Triangle(i + 2))) 
		{
			for (int j = 0; j < 3; j++) {
				//�S�Ă����ł���Β��_�̒ǉ�
				provi[1].vertsPos = Triangle(i + j);
				clone[1].push_back(provi[1]);
				outSiderIndex[1].push_back(static_cast<WORD>(clone[1].size() - 1));
			}
		}//3�̒��_�̂Ȃ��ł����ꂩ����(�������͐�)�ł����
		else {
			std::vector<WORD> overVertList;				//�㑤�̒��_�C���f�b�N�X
			std::vector<WORD> underVertList;			//�����̒��_�C���f�b�N�X
			//�O���_���̃��[�v
			for (unsigned int j = i; j < i + 3; j++)
			{
				//���H���H�ǂ����ɂ���́H
				if (cuttingObjPlane.GetSide(Triangle(j)))
				{
					//���̕����ɂ����over�ɒǉ�
					overVertList.push_back(workMeshIndex[j]);
				}
				else
				{
					//���̕����ɂ����under�ɒǉ�
					underVertList.push_back(workMeshIndex[j]);
				}
			}
			//�㑤��List�Ɋ܂܂�钸�_�̐�����Ȃ��
			if (overVertList.size() == 1) {
				XMFLOAT3 vert1 = workMesh[overVertList[0]].vertsPos;
				XMFLOAT3 vert2_old = workMesh[underVertList[0]].vertsPos;
				XMFLOAT3 vert3_old = workMesh[underVertList[1]].vertsPos;

				XMFLOAT3 l_0 = vert1;													 //vert1�̃��[�J�����W�����[���h���W�ɕϊ�
				XMFLOAT3 l = ChangeVector(XMFLOAT3Arithmetic(vert2_old, vert1, 1));		 //vert2_old - vert1 �ŋ��߂�ꂽ�x�N�g��(vert1��vert2_old�̋���)�����W�ϊ�
				XMFLOAT3 n = cuttingObjPlane.GetNormal();                                //�ؒf�p�p�l���̖@���x�N�g������
				XMFLOAT3 p_0 = XMFLOAT3(cMtx._41, cMtx._42, cMtx._43);					 //�ؒf�p�p�l���̃|�W�V��������

				float denominator;
				float t;
				XMFLOAT3 vert2;
				XMFLOAT3 vert3;

				DX11Vec3Dot(denominator, l, n);											//�uvert1��vert2_old�̋����v�Ɓu�ؒf�p�p�l���̖@���x�N�g���v�̓��ς����߂�
				DX11Vec3Dot(t, XMFLOAT3Arithmetic(p_0, l_0, 1), n);
				t = t / denominator;													// (�u�p�l���̒��S�_���琳�ɂ��钸�_�܂ł̋����v�Ɓu�ؒf�p�p�l���̖@���x�N�g���v�̓���) ���@�uvert1��vert2_old�̋����v�Ɓu�ؒf�p�p�l���̖@���x�N�g���v�̓���

				vert2 = XMFLOAT3Arithmetic(l_0,
					XMFLOAT3Arithmetic(l, XMFLOAT3(t, t, t), 2),
					0);																	//�uvert1�̃��[���h���W�v�{�uvert1��vert2_old�̋����v�~�@t

				l = ChangeVector(XMFLOAT3Arithmetic(vert3_old, vert1, 1));				// vert3_old - vert1 �ŋ��߂�ꂽ�x�N�g��(vert1��vert3_old�̋���)�����W�ϊ� 
				DX11Vec3Dot(denominator, l, n);	                                        //�uvert1��vert3_old�̋����v�Ɓu�ؒf�p�p�l���̖@���x�N�g���v�̓��ς����߂�
				DX11Vec3Dot(t, XMFLOAT3Arithmetic(p_0, l_0, 1), n);
				t = t / denominator;													// (�u�p�l���̒��S�_���琳�ɂ��钸�_�܂ł̋����v�Ɓu�ؒf�p�p�l���̖@���x�N�g���v�̓���) ���@�uvert1��vert3_old�̋����v�Ɓu�ؒf�p�p�l���̖@���x�N�g���v�̓���
				vert3 = XMFLOAT3Arithmetic(l_0,
					XMFLOAT3Arithmetic(l, XMFLOAT3(t, t, t), 2),
					0);						                                            //�uvert2�̃��[���h���W�v�{�uvert1��vert3_old�̋����v�~�@t
				
				//�V���Ȓ��_����ǉ�
				provi[0].vertsPos = vert2;
				provi[0].normal    = workMesh[underVertList[0]].normal;
				clone[0].push_back(provi[0]);
				addToIndex[0].push_back(static_cast<WORD>(clone[0].size() - 1));

				provi[1].vertsPos = vert3;
				provi[1].normal = workMesh[underVertList[1]].normal;
				clone[0].push_back(provi[1]);
				addToIndex[0].push_back(static_cast<WORD>(clone[0].size() - 1));

				int cont = 0;
				
				for (unsigned int j = i; j < i + 3; j++)
				{
					if (cuttingObjPlane.GetSide(Triangle(j)))
					{
						cloneIndex[0].push_back(overVertList[0]);
					}
					else 
					{
						cloneIndex[0].push_back(static_cast<WORD>(clone[0].size() - 2 + cont));  //overMeshVerts�̑��� - �Q + count�̐���index�o�b�t�@�ɒǉ�
						cont++;
					}
				}

				//�����̌v�Z
				vert1     = workMesh[underVertList[0]].vertsPos;
				vert2     = workMesh[underVertList[1]].vertsPos;
				vert3_old = workMesh[overVertList[0]].vertsPos;

				l_0 = vert1;															 //vert1�̃��[�J�����W�����[���h���W�ɕϊ�
				l = ChangeVector(XMFLOAT3Arithmetic(vert3_old, vert1, 1));				 //vert2_old - vert1 �ŋ��߂�ꂽ�x�N�g��(vert1��vert2_old�̋���)�����W�ϊ�
				n = cuttingObjPlane.GetNormal();										 //�ؒf�p�p�l���̖@���x�N�g������
				p_0 = XMFLOAT3(cMtx._41, cMtx._42, cMtx._43);							 //�ؒf�p�p�l���̃|�W�V��������

				XMFLOAT3 vert4;

				DX11Vec3Dot(denominator, l, n);											//�uvert1��vert2_old�̋����v�Ɓu�ؒf�p�p�l���̖@���x�N�g���v�̓��ς����߂�
				DX11Vec3Dot(t, XMFLOAT3Arithmetic(p_0, l_0, 1), n);
				t = t / denominator;													// (�u�p�l���̒��S�_���琳�ɂ��钸�_�܂ł̋����v�Ɓu�ؒf�p�p�l���̖@���x�N�g���v�̓���) ���@�uvert1��vert2_old�̋����v�Ɓu�ؒf�p�p�l���̖@���x�N�g���v�̓���

				vert3 = XMFLOAT3Arithmetic(l_0,
					XMFLOAT3Arithmetic(l, XMFLOAT3(t, t, t), 2),
					0);																	//�uvert1�̃��[���h���W�v�{�uvert1��vert2_old�̋����v�~�@t

				l_0 = vert2;
				l = ChangeVector(XMFLOAT3Arithmetic(vert3_old, vert2, 1));				// vert3_old - vert1 �ŋ��߂�ꂽ�x�N�g��(vert1��vert3_old�̋���)�����W�ϊ� 
				DX11Vec3Dot(denominator, l, n);	                                        //�uvert1��vert3_old�̋����v�Ɓu�ؒf�p�p�l���̖@���x�N�g���v�̓��ς����߂�
				DX11Vec3Dot(t, XMFLOAT3Arithmetic(p_0, l_0, 1), n);
				t = t / denominator;													// (�u�p�l���̒��S�_���琳�ɂ��钸�_�܂ł̋����v�Ɓu�ؒf�p�p�l���̖@���x�N�g���v�̓���) ���@�uvert1��vert3_old�̋����v�Ɓu�ؒf�p�p�l���̖@���x�N�g���v�̓���
				vert4 = XMFLOAT3Arithmetic(l_0,
					XMFLOAT3Arithmetic(l, XMFLOAT3(t, t, t), 2),
					0);						                                            //�uvert2�̃��[���h���W�v�{�uvert1��vert3_old�̋����v�~�@t

				//�V���Ȓ��_����ǉ�
				provi[0].vertsPos = vert3;
				provi[0].normal = workMesh[underVertList[0]].normal;
				clone[1].push_back(provi[0]);
				addToIndex[1].push_back(static_cast<WORD>(clone[1].size() - 1));

				provi[1].vertsPos = vert4;
				provi[1].normal = workMesh[underVertList[1]].normal;
				clone[1].push_back(provi[1]);
				addToIndex[1].push_back(static_cast<WORD>(clone[1].size() - 1));
		
				cont = 0;

				for (unsigned int j = i; j < i + 3; j++)
				{
					if (!cuttingObjPlane.GetSide(Triangle(j)))
					{
						workIndex.push_back(underVertList[cont]);
						cont = 1;
					}
					else
					{
						workIndex.push_back(clone[1].size() - 1 - cont);
					}
				}
				//���ёւ� �܂��̓\�[�g
				std::sort(workIndex.begin(), workIndex.end());
				for (unsigned int j = 0; j < workIndex.size(); j++) {
					cloneIndex[1].push_back(workIndex[j]);
				}
				workIndex.clear();
				cont = 0;

				for (auto j = i; j < i + 3; j++)
				{
					if (!cuttingObjPlane.GetSide(Triangle(j)))
					{
						cloneIndex[1].push_back(static_cast<WORD>(clone[1].size() - 1 - cont));
						cont = 1; 
					}
					else
					{
						cloneIndex[1].push_back(underVertList[cont]);
					}
				}
				//���I�z����N���A
				std::vector<WORD>().swap(overVertList);
				std::vector<WORD>().swap(underVertList);
			}

			//��ł����
			else {
				XMFLOAT3 vert1 = workMesh[overVertList[0]].vertsPos;
				XMFLOAT3 vert2 = workMesh[overVertList[1]].vertsPos;
				XMFLOAT3 vert3_old = workMesh[underVertList[0]].vertsPos;

				XMFLOAT3 l_0 = vert1;													 //vert1�̃��[�J�����W�����[���h���W�ɕϊ�
				XMFLOAT3 l = ChangeVector(XMFLOAT3Arithmetic(vert3_old, vert1, 1));		 //vert2_old - vert1 �ŋ��߂�ꂽ�x�N�g��(vert1��vert2_old�̋���)�����W�ϊ�
				XMFLOAT3 n = cuttingObjPlane.GetNormal();                                //�ؒf�p�p�l���̖@���x�N�g������
				XMFLOAT3 p_0 = XMFLOAT3(cMtx._41, cMtx._42, cMtx._43);					 //�ؒf�p�p�l���̃|�W�V��������

				float denominator;
				float t;
				XMFLOAT3 vert3;
				XMFLOAT3 vert4;

				DX11Vec3Dot(denominator, l, n);											//�uvert1��vert2_old�̋����v�Ɓu�ؒf�p�p�l���̖@���x�N�g���v�̓��ς����߂�
				DX11Vec3Dot(t, XMFLOAT3Arithmetic(p_0, l_0, 1), n);
				t = t / denominator;													// (�u�p�l���̒��S�_���琳�ɂ��钸�_�܂ł̋����v�Ɓu�ؒf�p�p�l���̖@���x�N�g���v�̓���) ���@�uvert1��vert2_old�̋����v�Ɓu�ؒf�p�p�l���̖@���x�N�g���v�̓���

				vert3 = XMFLOAT3Arithmetic(l_0,
					XMFLOAT3Arithmetic(l, XMFLOAT3(t, t, t), 2),
					0);																		//�uvert1�̃��[���h���W�v�{�uvert1��vert2_old�̋����v�~�@t
			
				l_0 = vert2;
				l = ChangeVector(XMFLOAT3Arithmetic(vert3_old, vert2, 1));				// vert3_old - vert1 �ŋ��߂�ꂽ�x�N�g��(vert1��vert3_old�̋���)�����W�ϊ� 
				DX11Vec3Dot(denominator, l, n);	                                        //�uvert1��vert3_old�̋����v�Ɓu�ؒf�p�p�l���̖@���x�N�g���v�̓��ς����߂�
				DX11Vec3Dot(t, XMFLOAT3Arithmetic(p_0, l_0, 1), n);
				t = t / denominator;													// (�u�p�l���̒��S�_���琳�ɂ��钸�_�܂ł̋����v�Ɓu�ؒf�p�p�l���̖@���x�N�g���v�̓���) ���@�uvert1��vert3_old�̋����v�Ɓu�ؒf�p�p�l���̖@���x�N�g���v�̓���
				vert4 = XMFLOAT3Arithmetic(l_0, XMFLOAT3Arithmetic(l, XMFLOAT3(t, t, t), 2), 0);
				
				//�V���Ȓ��_����ǉ�
				provi[0].vertsPos  = vert3;
				provi[0].normal    = workMesh[overVertList[0]].normal;
				clone[0].push_back(provi[0]);
				addToIndex[0].push_back(static_cast<WORD>(clone[0].size() - 1));

				provi[1].vertsPos  = vert4;
				provi[1].normal    = workMesh[overVertList[1]].normal;
				clone[0].push_back(provi[1]);
				addToIndex[0].push_back(static_cast<WORD>(clone[0].size() - 1));

				int cont = 0;
					
				for (unsigned int j = i; j < i + 3; j++)
				{
					if (cuttingObjPlane.GetSide(Triangle(j)))
					{
						workIndex.push_back (clone[0].size() - 1 - cont);
						cont = 1;
					}
					else
					{
						workIndex.push_back(overVertList[cont]);
					}
				}

				//����ւ����Ēǉ�
				// �ǉ����钸�_�̖@���F�����̒��_���P�A�ǉ��������_�̒��ň����������Q�A��ԑ傫�������R
				std::sort(workIndex.begin(), workIndex.end());				//�܂���workIndex���̒l���\�[�g����
				for (unsigned int j = 0; j < workIndex.size(); j++) {
					cloneIndex[0].push_back(workIndex[j]);
				}
				workIndex.clear();

				cont = 0;
	
				for (unsigned int j = i; j < i + 3; j++)
				{
					if (cuttingObjPlane.GetSide(Triangle(j)))
					{
						cloneIndex[0].push_back(overVertList[cont]);
						cont = 1;
					}
					else
					{
						cloneIndex[0].push_back(static_cast<WORD>(clone[0].size() - (1 + cont)));  //overMeshVerts�̑��� - (�Q) ��index�o�b�t�@�ɒǉ�
					}
				}

				//�����̌v�Z

				vert1 = workMesh[underVertList[0]].vertsPos;
				XMFLOAT3 vert2_old = workMesh[overVertList[0]].vertsPos;
				vert3_old = workMesh[overVertList[1]].vertsPos;

				l_0 = vert1;															//vert1�̃��[�J�����W�����[���h���W�ɕϊ�
				l = ChangeVector(XMFLOAT3Arithmetic(vert2_old, vert1, 1));				//vert2_old - vert1 �ŋ��߂�ꂽ�x�N�g��(vert1��vert2_old�̋�����scale)
				n = cuttingObjPlane.GetNormal();										//�ؒf�p�p�l���̖@���x�N�g������
				p_0 = XMFLOAT3(cMtx._41, cMtx._42, cMtx._43);							//�ؒf�p�p�l���̃|�W�V��������

				
				DX11Vec3Dot(denominator, l, n);											//�uvert1��vert2_old�̋����v�Ɓu�ؒf�p�p�l���̖@���x�N�g���v�̓��ς����߂�
				DX11Vec3Dot(t, XMFLOAT3Arithmetic(p_0, l_0, 1), n);
				t = t / denominator;													// (�u�p�l���̒��S�_���琳�ɂ��钸�_�܂ł̋����v�Ɓu�ؒf�p�p�l���̖@���x�N�g���v�̓���) ���@�uvert1��vert2_old�̋����v�Ɓu�ؒf�p�p�l���̖@���x�N�g���v�̓���

				vert2 = XMFLOAT3Arithmetic(l_0,
					XMFLOAT3Arithmetic(l, XMFLOAT3(t, t, t), 2),
					0);																	//�uvert1�̃��[���h���W�v�{�uvert1��vert2_old�̋����v�~�@t

				l = ChangeVector(XMFLOAT3Arithmetic(vert3_old, vert1, 1));				// vert3_old - vert1 �ŋ��߂�ꂽ�x�N�g��(vert1��vert3_old�̋���)�����W�ϊ� 
				DX11Vec3Dot(denominator, l, n);	                                        //�uvert1��vert3_old�̋����v�Ɓu�ؒf�p�p�l���̖@���x�N�g���v�̓��ς����߂�
				DX11Vec3Dot(t, XMFLOAT3Arithmetic(p_0, l_0, 1), n);
				t = t / denominator;													// (�u�p�l���̒��S�_���琳�ɂ��钸�_�܂ł̋����v�Ɓu�ؒf�p�p�l���̖@���x�N�g���v�̓���) ���@�uvert1��vert3_old�̋����v�Ɓu�ؒf�p�p�l���̖@���x�N�g���v�̓���
				vert3 = XMFLOAT3Arithmetic(l_0,
					XMFLOAT3Arithmetic(l, XMFLOAT3(t, t, t), 2),
					0);						                                            //�uvert2�̃��[���h���W�v�{�uvert1��vert3_old�̋����v�~�@t

				provi[0].vertsPos = vert2;
				provi[0].normal = workMesh[overVertList[0]].normal;
				clone[1].push_back(provi[0]);
				addToIndex[1].push_back(static_cast<WORD>(clone[1].size() - 1));

				provi[1].vertsPos = vert3;
				provi[1].normal = workMesh[overVertList[1]].normal;
				clone[1].push_back(provi[1]);
				addToIndex[1].push_back(static_cast<WORD>(clone[1].size() - 1));

				cont = 0;
				for (unsigned int j = i; j < i + 3; j++)
				{
					if (!cuttingObjPlane.GetSide(Triangle(j)))
					{
						cloneIndex[1].push_back(underVertList[0]);
						
					}
					else
					{
						cloneIndex[1].push_back(static_cast<WORD>(clone[1].size() - 2 + cont));  //overMeshVerts�̑��� - �Q + count�̐���index�o�b�t�@�ɒǉ�
						cont++;
					}
				}
				//���I�z����N���A
				std::vector<WORD>().swap(overVertList);
				std::vector<WORD>().swap(underVertList);
			}
		}
	}

	//***************************************************************************
	//���ʂ̐���
	//note@ �v�Z�Ƃ͊֌W�̂Ȃ����_��Index��ۑ����Ă����A�������
	//***************************************************************************

	for (unsigned int j = 0; j < outSiderIndex[0].size(); j++) {
		cloneIndex[0].push_back(outSiderIndex[0][j]);
	}
	for (unsigned int j = 0; j < outSiderIndex[1].size(); j++) {
		cloneIndex[1].push_back(outSiderIndex[1][j]);
	}

	//***************************************************************************
	// �ؒf�ʂ��쐬����
	// note@ �V���_���璆�S�_�����߂đ������
	// tink@ �f�ʐ}�����A���S���Y����������
	//***************************************************************************

	//�㑤	
	XMFLOAT3 overEdgeMidPoint = XMFLOAT3(0.0f,0.0f,0.0f);
	//�V���ȃ��b�V�����쐬����ׂ̒��_���쐬����
	for (unsigned int i = 0; i < addToIndex[0].size(); i++)
	{
		XMFLOAT3 cloneVert;
		cloneVert = clone[0][static_cast<int>(addToIndex[0][i])].vertsPos;
		overEdgeMidPoint = XMFLOAT3Arithmetic(overEdgeMidPoint, cloneVert, 0);//�ǉ����ꂽ���_���W��S������
	}
	overEdgeMidPoint = XMFLOAT3Arithmetic(overEdgeMidPoint, 
					   XMFLOAT3(static_cast<float>(addToIndex[0].size()), 
								static_cast<float>(addToIndex[0].size()),
								static_cast<float>(addToIndex[0].size())),3);				//�����ǉ��������_���W������(���S�_�ɂȂ�܂�)

	provi[0].vertsPos = Triangle(overEdgeMidPoint, cMtx);
	provi[0].normal = clone[0][0].normal;								//���b�V���̑����_��񃊃X�g�ɂԂ����� 

	for (unsigned int i = 0; i < addToIndex[0].size(); i += 2)
	{
		//index�o�b�t�@�ɐV�����������钸�_����ǉ�
		cloneIndex[0].push_back(static_cast<WORD>(clone[0].size() - 1));
		cloneIndex[0].push_back(addToIndex[0][i]);
		cloneIndex[0].push_back(addToIndex[0][i + 1]);
	}

	//����
	XMFLOAT3 underEdgeMidPoint = XMFLOAT3(0.0f, 0.0f, 0.0f);
	for (unsigned int i = 0; i < addToIndex[1].size(); i++)
	{
		XMFLOAT3 cloneVert;
		cloneVert = clone[1][static_cast<int>(addToIndex[1][i])].vertsPos;
		underEdgeMidPoint = XMFLOAT3Arithmetic(underEdgeMidPoint, cloneVert, 0);		//�ǉ����ꂽ���_���W��S������
	}
	underEdgeMidPoint = XMFLOAT3Arithmetic(underEdgeMidPoint,
						XMFLOAT3(static_cast<float>(addToIndex[1].size()),
								 static_cast<float>(addToIndex[1].size()),
								 static_cast<float>(addToIndex[1].size())), 3);

	provi[1].vertsPos = Triangle(underEdgeMidPoint,cMtx);								//���b�V���̑����_��񃊃X�g�ɂԂ����� 
	provi[1].normal =  clone[1][0].normal;
	
	//index�o�b�t�@�ɐV�����������钸�_����ǉ�
	for (unsigned int i = 0; i < addToIndex[1].size(); i += 2)

	{
		cloneIndex[1].push_back(addToIndex[1][i]);
		cloneIndex[1].push_back(static_cast<WORD>(clone[1].size() - 1));
		cloneIndex[1].push_back(addToIndex[1][i + 1]);
	}

	//*******************************************************
	//���b�V���쐬
	//*******************************************************
	//���������_������ɐV�������b�V������� 
	overVertex  = new VERTEX[static_cast<int>(clone[0].size())];
	underVertex = new VERTEX[static_cast<int>(clone[1].size())];
	
	//��̏����Z�b�g
	for (i = 0; i < static_cast<int>(clone[0].size()); i++) {
		overVertex[i].position = clone[0][i].vertsPos;
	}
	for (i = 0; i < static_cast<int>(clone[0].size()); i++) {
		overVertex[i].col = clone[0][i].color;
	}
	for (i = 0; i < static_cast<int>(clone[0].size()); i++) {
		overVertex[i].uv = clone[0][i].uv;
	}
	for (i = 0; i < static_cast<int>(clone[0].size()); i++) {
		overVertex[i].nomal = clone[0][i].normal;
	}

	//���̏����Z�b�g
	for (i = 0; i < static_cast<int>(clone[1].size()); i++) {
		underVertex[i].position = clone[1][i].vertsPos;
	}
	for (i = 0; i < static_cast<int>(clone[1].size()); i++) {
		underVertex[i].col = clone[1][i].color;
	}
	for (i = 0; i < static_cast<int>(clone[1].size()); i++) {
		underVertex[i].uv = clone[1][i].uv;
	}
	for (i = 0; i < static_cast<int>(clone[1].size()); i++) {
		underVertex[i].nomal = clone[1][i].normal;
	}
	
	for (i = 0; i < clonemax; i++) {
		afterMesh[i].SetVertexListSize(static_cast<int>(clone[i].size()));
		afterMesh[i].SetIndexListSize(static_cast<int>(cloneIndex[i].size()));
		afterMesh[i].SetColorCount(colorCount);
	}
	//���b�V���쐬
	if (static_cast<int>(cloneIndex[0].size()) > 0) {
		afterMesh[0].CreateMesh("shader/vs.fx", "shader/ps.fx", overVertex, cloneIndex[0],model[1]->GetModelData());
	}
	if (static_cast<int>(cloneIndex[1].size()) > 0) {
		afterMesh[1].CreateMesh("shader/vs.fx", "shader/ps.fx", underVertex, cloneIndex[1], model[1]->GetModelData());
	}
	afterMesh[0].SetTrans(XMFLOAT3{ 1.5f,0.0f,0.0f });
	afterMesh[1].SetTrans(XMFLOAT3{ -1.5f,0.0f,0.0f });
}

//***************************************************************************
// MeshCut�X�V
//***************************************************************************
void MMDBreaker::Cutter::UpDate(XMFLOAT3 trans, XMFLOAT3 angle) {
	for (i = 0; i < clonemax; i++) {
		afterMtx[i]	  = MMDBreaker::Cutter::getInstance().GetAfterMeshWorldMtx(0);
		afterTrans[i] = XMFLOAT3Arithmetic(afterMesh[i].GetTrans(), trans, 0);
		afterAngle[i] = angle;

		DX11MakeWorldMatrix(afterMtx[i], 
							afterAngle[i],
							afterTrans[i]);

		MMDBreaker::Cutter::getInstance().SetAfterMeshWorldMtx(afterMtx[i], i);
	}
}


//***************************************************************************
// MeshCut�`��
//***************************************************************************
void MMDBreaker::Cutter::Draw() {
	for (i = 0; i < clonemax; i++) {
		if (cloneIndex[i].size() > 0) {
			afterMesh[i].DrawMesh();
		}
	}
}

void MMDBreaker::Cutter::UnInit() {
	for (i = 0; i < clonemax; i++) {
		std::vector<MESHDATA>().swap(clone[i]);
		std::vector<WORD>().swap(cloneIndex[i]);
		std::vector<WORD>().swap(outSiderIndex[i]);
		std::vector<WORD>().swap(addToIndex[i]);

		afterMesh[i].UnInitMesh();
	}

	std::vector<MESHDATA>().swap(workMesh);
	std::vector<WORD>().swap(workMeshIndex);
}

//���[�J�����W����ɁA�ʒu����Ԃ�
XMFLOAT3 MMDBreaker::Cutter::ChangePoint(XMFLOAT3 pos,XMFLOAT4X4 mtx) {
	//�ʒu����vs��pos�𑫂��Z
	XMFLOAT4X4 workMtx;

	workMtx = mtx;

	workMtx._41 = mtx._41 + pos.x;
	workMtx._42 = mtx._42 + pos.y;
	workMtx._43 = mtx._43 + pos.z;

	return XMFLOAT3(workMtx._41, workMtx._42, workMtx._43);
}					

XMFLOAT3 MMDBreaker::Cutter::InverseChangePoint(XMFLOAT3 pos, XMFLOAT4X4 mtx) {
	//���_���̃��[�J�����W��Ԃ�
	XMFLOAT3 workVec;

	//�t�]������ׂɃ|�W�V�������烏�[���h���W������
	DX11Vec3MulMatrix(workVec,pos,mtx);
	return workVec;
}

//�x�N�^�[��scale��������
XMFLOAT3 MMDBreaker::Cutter::ChangeVector(XMFLOAT3 pos) {
	
	XMFLOAT3   workVec;
	XMFLOAT3   scale = {1.0f,1.0f,1.0f};

	workVec.x = pos.x;
	workVec.y = pos.y;
	workVec.z = pos.z;

	//scale�ƃ|�W�V�������|���邾��
	XMFLOAT3Arithmetic(workVec, scale, 2);
	return workVec;
}

//���_�̃��[���h���W���擾
XMFLOAT3 MMDBreaker::Cutter::Triangle(int count) {
	XMFLOAT3 fixation = 
		{	 workMesh[workMeshIndex[count]].vertsPos.x,
			 workMesh[workMeshIndex[count]].vertsPos.y,
			 workMesh[workMeshIndex[count]].vertsPos.z		};
	return fixation;
}

XMFLOAT3 MMDBreaker::Cutter::Triangle(XMFLOAT3 vert, XMFLOAT4X4 mtx) {
	XMFLOAT3 fixation;
	DX11Vec3MulMatrix(fixation, vert, mtx);
	return fixation;
}

