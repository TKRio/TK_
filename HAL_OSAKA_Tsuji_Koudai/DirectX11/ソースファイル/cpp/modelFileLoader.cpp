
//*****************************************************************************
//!	@file	modelFileLoader.cpp
//!	@brief	PMXFile&PMDFileを読み込む
//!	@note	コンストラクタでファイル名を指定してロードしている
//!	@author	
//*****************************************************************************
#define _CRT_SECURE_NO_WARNINGS
#include "modelFileLoader.h"
#include <Windows.h>
#include <stdio.h>
#include <fstream>

MMDBreaker::PMDData::PMDData(const char* fileName) {
	FILE* hFp = nullptr;
	hFp = fopen(fileName, "rb");

	if (hFp != 0) {
		//ヘッダーの読み込み
		fread(&pmdHeader, sizeof(struct PMDData::PMDHEADER), 1, hFp);

		//頂点データの読み込み
		fread(&vertCount, sizeof(vertCount), 1, hFp);	//頂点数の読み込み	
		pmdVertex = new PMDVERTEX[vertCount];
		if (pmdVertex == nullptr) {
			MessageBoxW(NULL, L"pmdVertexError", L"Error", MB_ICONSTOP);
		}
		fread(pmdVertex,  38, vertCount, hFp);								

		//面頂点リストデータの読み込み
		fread(&faceVertCount, sizeof(faceVertCount), 1, hFp);	//面頂点リストの読み込み			
		faceVertIndex = new unsigned short[faceVertCount];	
		if (faceVertIndex == nullptr) {
			MessageBoxW(NULL, L"faceVertIndexError", L"Error", MB_ICONSTOP);
		}
		fread(faceVertIndex, 2, faceVertCount, hFp);	

		//マテリアルデータの読み込み
		fread(&materialCount, sizeof(materialCount), 1, hFp);	//材質の数
		pmdMaterial = new PMDMATERIAL[materialCount];
		if (pmdMaterial == nullptr) {
			MessageBoxW(NULL, L"pmdMaterialError", L"Error", MB_ICONSTOP);
		}
		fread(pmdMaterial, 70, materialCount, hFp);								

		//ボーン情報の読み込み
		fread(&boneCount, sizeof(boneCount), 1, hFp);
		pmdBone = new PMDBONE[boneCount];
		if (pmdBone == nullptr) {
			MessageBoxW(NULL, L"pmdBoneError", L"Error", MB_ICONSTOP);
		}
		fread(pmdBone, 39, boneCount, hFp);

		fclose(hFp);
		hFp = nullptr;
	}
	else {
		MessageBoxW(NULL, L"fopen", L"Error", MB_ICONSTOP);
	}

	//ファイルに書き起こす
	hFp = fopen("PMDData.txt", "w");
	if (hFp != 0) {
		fprintf(hFp,"◆　ヘッダー情報\n");
		fprintf(hFp, "[Header]\n%s\n %f\n %s\n %s\n", &pmdHeader.magic, pmdHeader.version, &pmdHeader.modelName, &pmdHeader.Comment);

		fprintf(hFp, "◆　頂点情報\n");
		fprintf(hFp, "[vertexCount]\n%d\n", vertCount);
		for (unsigned int i = 0; i < vertCount; i++) {
			fprintf(hFp, "▼vertexData:Pos\nX:%f\n Y:%f\n Z:%f\n", pmdVertex[i].position.x, pmdVertex[i].position.y, pmdVertex[i].position.z);
			fprintf(hFp, "vertexData:Normal\nX:%f\n Y:%f\n Z:%f\n", pmdVertex[i].normal.x, pmdVertex[i].normal.y, pmdVertex[i].normal.z);
			fprintf(hFp, "vertexData:UV \nU:%f\n V:%f\n\n"	 , pmdVertex[i].uv.x, pmdVertex[i].uv.y);
		}
		fprintf(hFp, "vertexData:boneNum \n%hu \n %hu\n",		pmdVertex->boneNum[0],  pmdVertex->boneNum[1]);
		fprintf(hFp, "vertexData:boneWeinght \n%s\n",			&pmdVertex->boneWeinght);
		fprintf(hFp, "vertexData:edgeFlag \n%s\n",			&pmdVertex->edgeFlag);
		fprintf(hFp, "faceVertCount\n%lu\n", faceVertCount);

		fprintf(hFp, "◆　材質(マテリアル)情報\n");
		fprintf(hFp, "[materialCount]\n%lu\n", materialCount);
		fprintf(hFp, "[textureFileName]\n");
		for (unsigned int i = 0; i < materialCount; i++) {
			fprintf(hFp, "%d :%s\n", i + 1, pmdMaterial[i].textureFileName);
			fprintf(hFp, "%d :%d\n", i + 1, pmdMaterial[i].faceVertCount);
		}

		fprintf(hFp, "\n◆　ボーン情報\n");
		fprintf(hFp, "[boneCount]\n%lu\n\n", boneCount);
		for (unsigned int i = 0; i < boneCount; i++) {
			fprintf(hFp, "■boneName\n%s\n", &pmdBone[i].boneName);
			fprintf(hFp, "parentBoneIndex \n%hu\n", pmdBone[i].parentBoneIndex);
			fprintf(hFp, "tailPosBoneIndex \n%hu\n", pmdBone[i].tailPosBoneIndex);
			fprintf(hFp, "boneType\n%s\n", &pmdBone[i].boneType);
			fprintf(hFp, "ikParentBoneIndex\n%hu\n", pmdBone[i].ikParentBoneIndex);
			fprintf(hFp, "boneHeadPos\nX:%f\nY:%f\nZ:%f\n\n", pmdBone[i].boneHeadPos.x, pmdBone[i].boneHeadPos.y, pmdBone[i].boneHeadPos.z);
		}


		fclose(hFp);
		hFp = nullptr;
	}
	else {
		MessageBoxW(NULL, L"fopen", L"Error", MB_ICONSTOP);
	}

}

MMDBreaker::PMDData::~PMDData() {
	delete[] pmdVertex;
	delete faceVertIndex;
}
