#include "Player.h"
#include "MeshCut.h"
#include <iostream>

using namespace DirectX;

void MMDBreaker::Cutter::Init(PMDModel* cutter, PMDModel* target,XMFLOAT4X4 cuttingMtx, XMFLOAT4X4 targetMtx) {
	
	pMtx = targetMtx;						//切断される側のワールド変換座標
	cMtx = cuttingMtx;						//切断する側のワールド変換座標

	model[0] = cutter;						//切断するためのobject(でもあり、メッシュでもある)
	model[1] = target;						//切断されるobject
	
	//切断される側のオブジェクトの情報を取り出す	
	for (i = 0; i < model[1]->GetVertexCount(); i++) {
		provi[0].vertsPos = Triangle(model[1]->GetVertexData(i).position, pMtx); // vertex：頂点
		provi[0].normal   = model[1]->GetVertexData(i).nomal;	// 法線
		provi[0].color	  = XMFLOAT4{0.0f,0.0f,0.0f,0.0f};		// カラー
		provi[0].uv	      = model[1]->GetVertexData(i).uv;		// UV座標

		workMesh.push_back(provi[0]);
	}

	auto indexCount = model[1]->GetIndexCount();
	for (auto j = 0; j < indexCount; j++) {
		workMeshIndex.push_back(model[1]->GetModelData()->GetfaceVertIndex(j));			//インデックスデータ
	}
	colorCount = model[1]->GetColorCount();

	//平面の定義
	XMFLOAT3 vertexAbc[planemax];							//ABCを求める
	for (i = 0; i < planemax; i++) {
		pos[i] = model[0]->GetModelData()->GetPmdVertexPos(
				 model[0]->GetModelData()->GetfaceVertIndex(i));
		vertexAbc[i].x = pos[i].x;
		vertexAbc[i].y = pos[i].y;
		vertexAbc[i].z = pos[i].z;
		DX11Vec3MulMatrix(vertexAbc[i], vertexAbc[i], cMtx);
	}

	//取り出した頂点情報を使い,3D空間を分断するパネル（実態なし）を作成する
	cuttingObjPlane.CreatePlaneFromPorigon(vertexAbc[0], vertexAbc[1], vertexAbc[2]);
	XMFLOAT3 no = cuttingObjPlane.GetNormal();

	//上側meshと下側meshに分ける
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

		//セットされた頂点Vが、切断用パネルの法線側にあるかどうか
		if (cuttingObjPlane.GetSide(Triangle(i)) &&
			cuttingObjPlane.GetSide(Triangle(i + 1)) &&
			cuttingObjPlane.GetSide(Triangle(i + 2)))
		{
			for (int j = 0; j < 3; j++) {
				//全てが正であれば頂点の追加
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
				//全てが負であれば頂点の追加
				provi[1].vertsPos = Triangle(i + j);
				clone[1].push_back(provi[1]);
				outSiderIndex[1].push_back(static_cast<WORD>(clone[1].size() - 1));
			}
		}//3つの頂点のなかでいずれかが負(もしくは正)であれば
		else {
			std::vector<WORD> overVertList;				//上側の頂点インデックス
			std::vector<WORD> underVertList;			//下側の頂点インデックス
			//三頂点分のループ
			for (unsigned int j = i; j < i + 3; j++)
			{
				//正？負？どっちにあるの？
				if (cuttingObjPlane.GetSide(Triangle(j)))
				{
					//正の方向にあればoverに追加
					overVertList.push_back(workMeshIndex[j]);
				}
				else
				{
					//負の方向にあればunderに追加
					underVertList.push_back(workMeshIndex[j]);
				}
			}
			//上側のListに含まれる頂点の数が一つならば
			if (overVertList.size() == 1) {
				XMFLOAT3 vert1 = workMesh[overVertList[0]].vertsPos;
				XMFLOAT3 vert2_old = workMesh[underVertList[0]].vertsPos;
				XMFLOAT3 vert3_old = workMesh[underVertList[1]].vertsPos;

				XMFLOAT3 l_0 = vert1;													 //vert1のローカル座標をワールド座標に変換
				XMFLOAT3 l = ChangeVector(XMFLOAT3Arithmetic(vert2_old, vert1, 1));		 //vert2_old - vert1 で求められたベクトル(vert1とvert2_oldの距離)を座標変換
				XMFLOAT3 n = cuttingObjPlane.GetNormal();                                //切断用パネルの法線ベクトルを代入
				XMFLOAT3 p_0 = XMFLOAT3(cMtx._41, cMtx._42, cMtx._43);					 //切断用パネルのポジションを代入

				float denominator;
				float t;
				XMFLOAT3 vert2;
				XMFLOAT3 vert3;

				DX11Vec3Dot(denominator, l, n);											//「vert1とvert2_oldの距離」と「切断用パネルの法線ベクトル」の内積を求める
				DX11Vec3Dot(t, XMFLOAT3Arithmetic(p_0, l_0, 1), n);
				t = t / denominator;													// (「パネルの中心点から正にある頂点までの距離」と「切断用パネルの法線ベクトル」の内積) ÷　「vert1とvert2_oldの距離」と「切断用パネルの法線ベクトル」の内積

				vert2 = XMFLOAT3Arithmetic(l_0,
					XMFLOAT3Arithmetic(l, XMFLOAT3(t, t, t), 2),
					0);																	//「vert1のワールド座標」＋「vert1とvert2_oldの距離」×　t

				l = ChangeVector(XMFLOAT3Arithmetic(vert3_old, vert1, 1));				// vert3_old - vert1 で求められたベクトル(vert1とvert3_oldの距離)を座標変換 
				DX11Vec3Dot(denominator, l, n);	                                        //「vert1とvert3_oldの距離」と「切断用パネルの法線ベクトル」の内積を求める
				DX11Vec3Dot(t, XMFLOAT3Arithmetic(p_0, l_0, 1), n);
				t = t / denominator;													// (「パネルの中心点から正にある頂点までの距離」と「切断用パネルの法線ベクトル」の内積) ÷　「vert1とvert3_oldの距離」と「切断用パネルの法線ベクトル」の内積
				vert3 = XMFLOAT3Arithmetic(l_0,
					XMFLOAT3Arithmetic(l, XMFLOAT3(t, t, t), 2),
					0);						                                            //「vert2のワールド座標」＋「vert1とvert3_oldの距離」×　t
				
				//新たな頂点情報を追加
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
						cloneIndex[0].push_back(static_cast<WORD>(clone[0].size() - 2 + cont));  //overMeshVertsの総数 - ２ + countの数をindexバッファに追加
						cont++;
					}
				}

				//下側の計算
				vert1     = workMesh[underVertList[0]].vertsPos;
				vert2     = workMesh[underVertList[1]].vertsPos;
				vert3_old = workMesh[overVertList[0]].vertsPos;

				l_0 = vert1;															 //vert1のローカル座標をワールド座標に変換
				l = ChangeVector(XMFLOAT3Arithmetic(vert3_old, vert1, 1));				 //vert2_old - vert1 で求められたベクトル(vert1とvert2_oldの距離)を座標変換
				n = cuttingObjPlane.GetNormal();										 //切断用パネルの法線ベクトルを代入
				p_0 = XMFLOAT3(cMtx._41, cMtx._42, cMtx._43);							 //切断用パネルのポジションを代入

				XMFLOAT3 vert4;

				DX11Vec3Dot(denominator, l, n);											//「vert1とvert2_oldの距離」と「切断用パネルの法線ベクトル」の内積を求める
				DX11Vec3Dot(t, XMFLOAT3Arithmetic(p_0, l_0, 1), n);
				t = t / denominator;													// (「パネルの中心点から正にある頂点までの距離」と「切断用パネルの法線ベクトル」の内積) ÷　「vert1とvert2_oldの距離」と「切断用パネルの法線ベクトル」の内積

				vert3 = XMFLOAT3Arithmetic(l_0,
					XMFLOAT3Arithmetic(l, XMFLOAT3(t, t, t), 2),
					0);																	//「vert1のワールド座標」＋「vert1とvert2_oldの距離」×　t

				l_0 = vert2;
				l = ChangeVector(XMFLOAT3Arithmetic(vert3_old, vert2, 1));				// vert3_old - vert1 で求められたベクトル(vert1とvert3_oldの距離)を座標変換 
				DX11Vec3Dot(denominator, l, n);	                                        //「vert1とvert3_oldの距離」と「切断用パネルの法線ベクトル」の内積を求める
				DX11Vec3Dot(t, XMFLOAT3Arithmetic(p_0, l_0, 1), n);
				t = t / denominator;													// (「パネルの中心点から正にある頂点までの距離」と「切断用パネルの法線ベクトル」の内積) ÷　「vert1とvert3_oldの距離」と「切断用パネルの法線ベクトル」の内積
				vert4 = XMFLOAT3Arithmetic(l_0,
					XMFLOAT3Arithmetic(l, XMFLOAT3(t, t, t), 2),
					0);						                                            //「vert2のワールド座標」＋「vert1とvert3_oldの距離」×　t

				//新たな頂点情報を追加
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
				//並び替え まずはソート
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
				//動的配列をクリア
				std::vector<WORD>().swap(overVertList);
				std::vector<WORD>().swap(underVertList);
			}

			//二つであれば
			else {
				XMFLOAT3 vert1 = workMesh[overVertList[0]].vertsPos;
				XMFLOAT3 vert2 = workMesh[overVertList[1]].vertsPos;
				XMFLOAT3 vert3_old = workMesh[underVertList[0]].vertsPos;

				XMFLOAT3 l_0 = vert1;													 //vert1のローカル座標をワールド座標に変換
				XMFLOAT3 l = ChangeVector(XMFLOAT3Arithmetic(vert3_old, vert1, 1));		 //vert2_old - vert1 で求められたベクトル(vert1とvert2_oldの距離)を座標変換
				XMFLOAT3 n = cuttingObjPlane.GetNormal();                                //切断用パネルの法線ベクトルを代入
				XMFLOAT3 p_0 = XMFLOAT3(cMtx._41, cMtx._42, cMtx._43);					 //切断用パネルのポジションを代入

				float denominator;
				float t;
				XMFLOAT3 vert3;
				XMFLOAT3 vert4;

				DX11Vec3Dot(denominator, l, n);											//「vert1とvert2_oldの距離」と「切断用パネルの法線ベクトル」の内積を求める
				DX11Vec3Dot(t, XMFLOAT3Arithmetic(p_0, l_0, 1), n);
				t = t / denominator;													// (「パネルの中心点から正にある頂点までの距離」と「切断用パネルの法線ベクトル」の内積) ÷　「vert1とvert2_oldの距離」と「切断用パネルの法線ベクトル」の内積

				vert3 = XMFLOAT3Arithmetic(l_0,
					XMFLOAT3Arithmetic(l, XMFLOAT3(t, t, t), 2),
					0);																		//「vert1のワールド座標」＋「vert1とvert2_oldの距離」×　t
			
				l_0 = vert2;
				l = ChangeVector(XMFLOAT3Arithmetic(vert3_old, vert2, 1));				// vert3_old - vert1 で求められたベクトル(vert1とvert3_oldの距離)を座標変換 
				DX11Vec3Dot(denominator, l, n);	                                        //「vert1とvert3_oldの距離」と「切断用パネルの法線ベクトル」の内積を求める
				DX11Vec3Dot(t, XMFLOAT3Arithmetic(p_0, l_0, 1), n);
				t = t / denominator;													// (「パネルの中心点から正にある頂点までの距離」と「切断用パネルの法線ベクトル」の内積) ÷　「vert1とvert3_oldの距離」と「切断用パネルの法線ベクトル」の内積
				vert4 = XMFLOAT3Arithmetic(l_0, XMFLOAT3Arithmetic(l, XMFLOAT3(t, t, t), 2), 0);
				
				//新たな頂点情報を追加
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

				//入れ替えして追加
				// 追加する頂点の法則：既存の頂点が１、追加した頂点の中で引くい方が２、一番大きい数が３
				std::sort(workIndex.begin(), workIndex.end());				//まずはworkIndex内の値をソートする
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
						cloneIndex[0].push_back(static_cast<WORD>(clone[0].size() - (1 + cont)));  //overMeshVertsの総数 - (２) をindexバッファに追加
					}
				}

				//下側の計算

				vert1 = workMesh[underVertList[0]].vertsPos;
				XMFLOAT3 vert2_old = workMesh[overVertList[0]].vertsPos;
				vert3_old = workMesh[overVertList[1]].vertsPos;

				l_0 = vert1;															//vert1のローカル座標をワールド座標に変換
				l = ChangeVector(XMFLOAT3Arithmetic(vert2_old, vert1, 1));				//vert2_old - vert1 で求められたベクトル(vert1とvert2_oldの距離＊scale)
				n = cuttingObjPlane.GetNormal();										//切断用パネルの法線ベクトルを代入
				p_0 = XMFLOAT3(cMtx._41, cMtx._42, cMtx._43);							//切断用パネルのポジションを代入

				
				DX11Vec3Dot(denominator, l, n);											//「vert1とvert2_oldの距離」と「切断用パネルの法線ベクトル」の内積を求める
				DX11Vec3Dot(t, XMFLOAT3Arithmetic(p_0, l_0, 1), n);
				t = t / denominator;													// (「パネルの中心点から正にある頂点までの距離」と「切断用パネルの法線ベクトル」の内積) ÷　「vert1とvert2_oldの距離」と「切断用パネルの法線ベクトル」の内積

				vert2 = XMFLOAT3Arithmetic(l_0,
					XMFLOAT3Arithmetic(l, XMFLOAT3(t, t, t), 2),
					0);																	//「vert1のワールド座標」＋「vert1とvert2_oldの距離」×　t

				l = ChangeVector(XMFLOAT3Arithmetic(vert3_old, vert1, 1));				// vert3_old - vert1 で求められたベクトル(vert1とvert3_oldの距離)を座標変換 
				DX11Vec3Dot(denominator, l, n);	                                        //「vert1とvert3_oldの距離」と「切断用パネルの法線ベクトル」の内積を求める
				DX11Vec3Dot(t, XMFLOAT3Arithmetic(p_0, l_0, 1), n);
				t = t / denominator;													// (「パネルの中心点から正にある頂点までの距離」と「切断用パネルの法線ベクトル」の内積) ÷　「vert1とvert3_oldの距離」と「切断用パネルの法線ベクトル」の内積
				vert3 = XMFLOAT3Arithmetic(l_0,
					XMFLOAT3Arithmetic(l, XMFLOAT3(t, t, t), 2),
					0);						                                            //「vert2のワールド座標」＋「vert1とvert3_oldの距離」×　t

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
						cloneIndex[1].push_back(static_cast<WORD>(clone[1].size() - 2 + cont));  //overMeshVertsの総数 - ２ + countの数をindexバッファに追加
						cont++;
					}
				}
				//動的配列をクリア
				std::vector<WORD>().swap(overVertList);
				std::vector<WORD>().swap(underVertList);
			}
		}
	}

	//***************************************************************************
	//側面の生成
	//note@ 計算とは関係のない頂点のIndexを保存しておき、それを代入
	//***************************************************************************

	for (unsigned int j = 0; j < outSiderIndex[0].size(); j++) {
		cloneIndex[0].push_back(outSiderIndex[0][j]);
	}
	for (unsigned int j = 0; j < outSiderIndex[1].size(); j++) {
		cloneIndex[1].push_back(outSiderIndex[1][j]);
	}

	//***************************************************************************
	// 切断面を作成する
	// note@ 新頂点から中心点を求めて代入する
	// tink@ 断面図生成アルゴリズムを見直す
	//***************************************************************************

	//上側	
	XMFLOAT3 overEdgeMidPoint = XMFLOAT3(0.0f,0.0f,0.0f);
	//新たなメッシュを作成する為の頂点を作成する
	for (unsigned int i = 0; i < addToIndex[0].size(); i++)
	{
		XMFLOAT3 cloneVert;
		cloneVert = clone[0][static_cast<int>(addToIndex[0][i])].vertsPos;
		overEdgeMidPoint = XMFLOAT3Arithmetic(overEdgeMidPoint, cloneVert, 0);//追加された頂点座標を全部足す
	}
	overEdgeMidPoint = XMFLOAT3Arithmetic(overEdgeMidPoint, 
					   XMFLOAT3(static_cast<float>(addToIndex[0].size()), 
								static_cast<float>(addToIndex[0].size()),
								static_cast<float>(addToIndex[0].size())),3);				//それを追加した頂点座標分割る(中心点になります)

	provi[0].vertsPos = Triangle(overEdgeMidPoint, cMtx);
	provi[0].normal = clone[0][0].normal;								//メッシュの総頂点情報リストにぶち込む 

	for (unsigned int i = 0; i < addToIndex[0].size(); i += 2)
	{
		//indexバッファに新しく生成する頂点情報を追加
		cloneIndex[0].push_back(static_cast<WORD>(clone[0].size() - 1));
		cloneIndex[0].push_back(addToIndex[0][i]);
		cloneIndex[0].push_back(addToIndex[0][i + 1]);
	}

	//下側
	XMFLOAT3 underEdgeMidPoint = XMFLOAT3(0.0f, 0.0f, 0.0f);
	for (unsigned int i = 0; i < addToIndex[1].size(); i++)
	{
		XMFLOAT3 cloneVert;
		cloneVert = clone[1][static_cast<int>(addToIndex[1][i])].vertsPos;
		underEdgeMidPoint = XMFLOAT3Arithmetic(underEdgeMidPoint, cloneVert, 0);		//追加された頂点座標を全部足す
	}
	underEdgeMidPoint = XMFLOAT3Arithmetic(underEdgeMidPoint,
						XMFLOAT3(static_cast<float>(addToIndex[1].size()),
								 static_cast<float>(addToIndex[1].size()),
								 static_cast<float>(addToIndex[1].size())), 3);

	provi[1].vertsPos = Triangle(underEdgeMidPoint,cMtx);								//メッシュの総頂点情報リストにぶち込む 
	provi[1].normal =  clone[1][0].normal;
	
	//indexバッファに新しく生成する頂点情報を追加
	for (unsigned int i = 0; i < addToIndex[1].size(); i += 2)

	{
		cloneIndex[1].push_back(addToIndex[1][i]);
		cloneIndex[1].push_back(static_cast<WORD>(clone[1].size() - 1));
		cloneIndex[1].push_back(addToIndex[1][i + 1]);
	}

	//*******************************************************
	//メッシュ作成
	//*******************************************************
	//送った頂点情報を基に新しいメッシュを作る 
	overVertex  = new VERTEX[static_cast<int>(clone[0].size())];
	underVertex = new VERTEX[static_cast<int>(clone[1].size())];
	
	//上の情報をセット
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

	//下の情報をセット
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
	//メッシュ作成
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
// MeshCut更新
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
// MeshCut描画
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

//ローカル座標を基に、位置情報を返す
XMFLOAT3 MMDBreaker::Cutter::ChangePoint(XMFLOAT3 pos,XMFLOAT4X4 mtx) {
	//位置情報とvsのposを足し算
	XMFLOAT4X4 workMtx;

	workMtx = mtx;

	workMtx._41 = mtx._41 + pos.x;
	workMtx._42 = mtx._42 + pos.y;
	workMtx._43 = mtx._43 + pos.z;

	return XMFLOAT3(workMtx._41, workMtx._42, workMtx._43);
}					

XMFLOAT3 MMDBreaker::Cutter::InverseChangePoint(XMFLOAT3 pos, XMFLOAT4X4 mtx) {
	//頂点情報のローカル座標を返す
	XMFLOAT3 workVec;

	//逆転させる為にポジションからワールド座標を引く
	DX11Vec3MulMatrix(workVec,pos,mtx);
	return workVec;
}

//ベクターとscaleをかける
XMFLOAT3 MMDBreaker::Cutter::ChangeVector(XMFLOAT3 pos) {
	
	XMFLOAT3   workVec;
	XMFLOAT3   scale = {1.0f,1.0f,1.0f};

	workVec.x = pos.x;
	workVec.y = pos.y;
	workVec.z = pos.z;

	//scaleとポジションを掛けるだけ
	XMFLOAT3Arithmetic(workVec, scale, 2);
	return workVec;
}

//頂点のワールド座標を取得
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

