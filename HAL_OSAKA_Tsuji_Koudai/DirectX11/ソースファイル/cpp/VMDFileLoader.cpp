//*****************************************************************************
//!	@file	VMDFileLoader.h
//!	@brief	VMDFile(���[�V�����t�@�C��)��ǂݍ���
//!	@note	
//!	@author	
//*****************************************************************************
#define _CRT_SECURE_NO_WARNINGS
#include "VMDFileLoader.h"
#include <Windows.h>
#include <stdio.h>

const int MAXCOUNT = 65535;

VMDFileLoader::VMD::VMD(const char *fileName) {
	FILE* hFp = nullptr;
	hFp = fopen(fileName, "rb");

	//�w�b�_�[�̓ǂݍ���
	fread(&vmdHeader, sizeof(struct VMD::VMDHeader), 1, hFp);

	//���[�V�����f�[�^�̓ǂݍ���
	fread(&motionCount, sizeof(motionCount), 1, hFp);	//���[�V�����f�[�^���ǂݍ���
	vmdMotion = new VMDMotion[motionCount];
	if (vmdMotion == nullptr) {
		MessageBoxW(NULL, L"vmdMotionError", L"Error", MB_ICONSTOP);
	}
	fread(vmdMotion,111,motionCount,hFp);

	//�\��f�[�^�̓ǂݍ���
	fread(&skinCount, sizeof(skinCount), 1, hFp);
	vmdSkin = new VMDSkin[skinCount];
	if (vmdSkin == nullptr) {
		MessageBoxW(NULL, L"vmdSkinError", L"Error", MB_ICONSTOP);
	}
	fread(vmdSkin, 23, skinCount, hFp);

	//�J�����f�[�^�̓ǂݍ���
	fread(&cameraCount, sizeof(cameraCount), 1, hFp);
	if (cameraCount <= MAXCOUNT) {			//�f�[�^���Ȃ��ꍇ�͐������傫���Ȃ邽�߁A�����Œe��
		vmdCamera = new VMDCamera[cameraCount];
		if (vmdCamera == nullptr) {
			MessageBoxW(NULL, L"vmdCameraError", L"Error", MB_ICONSTOP);
		}
		fread(vmdCamera, 61, cameraCount, hFp);
	}

	//�Ɩ��f�[�^�̓ǂݍ���
	if (cameraCount <= MAXCOUNT) {
		fread(&lightCount, sizeof(lightCount), 1, hFp);
		vmdLight = new VMDLight[lightCount];
		if (vmdLight == nullptr) {
			MessageBoxW(NULL, L"vmdLightError", L"Error", MB_ICONSTOP);
		}
		fread(vmdLight, 28, lightCount, hFp);
	}
	
	//�Z���t�V���h�E�f�[�^�̓ǂݍ���
	if (cameraCount <= MAXCOUNT) {
		fread(&selfShadowCount, sizeof(selfShadowCount), 1, hFp);
		vmdSS = new VMDSelfShadow[selfShadowCount];
		if (vmdSS == nullptr) {
			MessageBoxW(NULL, L"vmdSSError", L"Error", MB_ICONSTOP);
		}
		fread(vmdSS, 9, selfShadowCount, hFp);

		fclose(hFp);
		hFp = nullptr;
	}
	else {
		MessageBoxW(NULL, L"fopen", L"Error", MB_ICONSTOP);
	}

	//�t�@�C���ɏ����N����
	hFp = fopen("VMDData.txt", "w");
	if (hFp != 0) {
		fprintf(hFp, "���@�w�b�_�[���\n");
		fprintf(hFp, "[Header]\n%s\n %s\n ", &vmdHeader.header,&vmdHeader.modelName);

		fprintf(hFp, "���@���[�V�������\n");
		fprintf(hFp, "[motionDataCount]\n%lu\n\n ", motionCount);
		for (unsigned int i = 0; i < motionCount; i++) {
			fprintf(hFp, "�� motionData:Name\n%s\n", &vmdMotion[i].BoneName);
			fprintf(hFp, "motionData:flameNo\n%lu\n", vmdMotion[i].flameNo);
			fprintf(hFp, "motionData:location\nX:%f\n Y:%f\n Z:%f\n", vmdMotion[i].location.x, vmdMotion[i].location.y, vmdMotion[i].location.z);
			fprintf(hFp, "motionData:rotation\nX:%f\n Y:%f\n Z:%f\nW:%f\n", vmdMotion[i].rotation.x, vmdMotion[i].rotation.y, vmdMotion[i].rotation.z, vmdMotion[i].rotation.w);
			fprintf(hFp, "motionData:interpolation\n");
			for (int j = 0; j < 64; j++) {
				fprintf(hFp, "%s\n",&vmdMotion[i].interpolation[j]);
			}
		}

		fprintf(hFp, "���@�\����\n");
		fprintf(hFp, "[skinDataCount]\n%lu\n\n ", skinCount);
		for (unsigned int i = 0; i < skinCount; i++) {
			fprintf(hFp, "�� skinData:Name\n%s\n",vmdSkin[i].skinName);
			fprintf(hFp, "skinData:flameNo\n%lu\n", vmdSkin[i].flameNo);
			fprintf(hFp, "skinData:weight\n%f\n", vmdSkin[i].weight);
		}

	}
}
