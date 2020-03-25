//*****************************************************************************
//!	@file	MeshCut.h
//!	@brief	メッシュカット処理
//!	@note	一回だけ切断可能。テクスチャのあるモデルは切れない。サブセットを作る？
//!	@author
//*****************************************************************************

#pragma once
#include "Model.h"
#include "string"
#include "Mesh.h"

//*****************************************************************************
//Define
//*****************************************************************************
int const modelmax = 2;
int const planemax = 3;
int const posmax   = 3;
int const clonemax = 2;

//*****************************************************************************
//class
//*****************************************************************************

namespace MMDBreaker {
	class Cutter {
	private:
		Cutter() {};
		//コピーの禁止
		Cutter(const Cutter &);
		//代入演算子禁止
		void operator=(const Cutter &);

		PMDModel*				   model[modelmax];			//0 : 切断mesh 1:被害者(targetobj)mesh 3: 分割させるmesh 
		std::vector<MESHDATA>	   clone[clonemax];			//分割した後のmeshを格納するための構造体配列 0 :　上　１：　下
		std::vector<WORD>		   cloneIndex[clonemax];	//インデックス情報
		std::vector<WORD>		   outSiderIndex[clonemax];	//計算には関係のない頂点のインデックス情報
		std::vector<WORD>		   addToIndex[clonemax];	//新たに追加した頂点のインデックス情報

		std::vector<MESHDATA>	   workMesh;				//work用のMesh構造体
		std::vector<WORD>		   workMeshIndex;			//インデックス情報
		DirectX::XMFLOAT3		   pos[posmax];				//板オブジェクトに含まれている三角形ポリゴンの各頂点情報を抜き出す(1ポリゴン3頂点)	
		MESHDATA				   provi[clonemax];			//仮のデータを入れる箱

		DirectX::XMFLOAT4X4 pMtx;						//playerのmatrix
		DirectX::XMFLOAT4X4 cMtx;						//cutterのmatrix
		DirectX::XMFLOAT3 fixation;						//作業用（注視点）
		int i;
		int colorCount;

		Mesh				afterMesh[clonemax];		//メッシュカット実装後の情報を入れて描画する 0 : 上　１：下
		DirectX::XMFLOAT4X4 afterMtx[clonemax];
		DirectX::XMFLOAT3	afterAngle[clonemax];
		DirectX::XMFLOAT3	afterTrans[clonemax];

		VERTEX* overVertex = nullptr;
		VERTEX* underVertex = nullptr;
		//平面の定義
		Plane	  cuttingObjPlane;
		PLAVERTEX cuttingPlane;

		std::vector<int>		 workIndex;			//ワーク用頂点配列
	public:
		//シングルトン
		static Cutter& getInstance() {
			static Cutter singleton;
			return singleton;
		}

		//処理の中で使用する関数
		DirectX::XMFLOAT3 ChangePoint(DirectX::XMFLOAT3 pos, DirectX::XMFLOAT4X4 mtx);			//頂点のローカル座標をワールド変換
		DirectX::XMFLOAT3 InverseChangePoint(DirectX::XMFLOAT3 pos, DirectX::XMFLOAT4X4 mtx);	//ワールド座標をローカル座標に変換
		DirectX::XMFLOAT3 ChangeVector(DirectX::XMFLOAT3 pos);						//ベクターをワールド座標に変換
		DirectX::XMFLOAT3 Triangle(int count);
		DirectX::XMFLOAT3 Triangle(DirectX::XMFLOAT3 vert, DirectX::XMFLOAT4X4 mtx);

		void Init(PMDModel* cutter, PMDModel* target, DirectX::XMFLOAT4X4 cuttingMtx, DirectX::XMFLOAT4X4 targetMtx);
		void MeshCut();
		void UpDate(DirectX::XMFLOAT3 trans, DirectX::XMFLOAT3 angle);
		void Draw();
		void UnInit();

		//Getter
		DirectX::XMFLOAT4X4 GetAfterMeshWorldMtx(int count) { return afterMesh[count].GetWorldMtx(); };
		//Setter
		void SetAfterMeshWorldMtx(DirectX::XMFLOAT4X4 workMtx, int count) { afterMesh[count].SetWorldMtx(workMtx); };
		
	};
}
