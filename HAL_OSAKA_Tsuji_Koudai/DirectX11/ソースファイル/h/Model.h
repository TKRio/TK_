//*****************************************************************************
//!	@file	Model.h
//!	@brief	PMX&PMDモデルを表示するためのクラス
//!	@note
//!	@author
//*****************************************************************************
#pragma once
#include "modelFileLoader.h"

//******************************************************************************
//class
//******************************************************************************
namespace MMDBreaker {
	class PMDModel {
	private:
		int				vertexCount;					//頂点数のカウント
		int				indexCount;						//インデックスデータのカウント
		int				colorCount;						//色情報のカウント
		unsigned short *indexData;						//インデックスデータの格納場所
		CBDATA          cb;								//定数バッファ

		PMDData*				   modelDate = nullptr;		// モデルデータ
		VERTEX*					   vertexData = nullptr;	// 頂点データ
		ID3D11Buffer*			   vertexBuffer = nullptr;	// 頂点バッファ
		ID3D11Buffer*              indexBuffer = nullptr;	// インデックスバッファ
		ID3D11Buffer*			   constantBuffer = nullptr;	// コンスタントバッファ
		ID3D11VertexShader*		   vertexShader = nullptr;	// 頂点シェーダー
		ID3D11PixelShader*		   pixelShader = nullptr;	// ピクセルシェーダー
		ID3D11InputLayout*		   vertexLayout = nullptr;	// 頂点レイアウト
		ID3D11SamplerState*        samplerState = nullptr;	// サンプルステート	
		ID3D11ShaderResourceView** srv = nullptr;	// シェーダーリソースビュー

		ID3D11Device*				device;						// デバイス
		ID3D11DeviceContext*		devcontext;					//デバイスコンテキスト
	public:
		bool Init(const char* fileName, const char* vsfile, const char* psfile);
		void UnInit();
		void Draw();
		//void DrawParts();

		//Gettter
		PMDData* GetModelData()   { return modelDate;}
		VERTEX GetVertexData(int count) { return vertexData[count]; }
		int GetVertexCount()      {return vertexCount;}
		int GetColorCount()       { return colorCount; }
		unsigned short* GetIndexData() { return indexData;}
		int GetIndexCount() { return indexCount;}
	};
};