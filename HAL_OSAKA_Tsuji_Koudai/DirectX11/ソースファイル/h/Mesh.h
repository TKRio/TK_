#pragma once
//メッシュを作成するclass
#include "DX11Settransform.h"
#include "dx11mathutil.h"
#include "modelFileLoader.h"

//メッシュデータ構造体
struct MESHDATA {
	DirectX::XMFLOAT3   vertsPos;		//頂点情報
	DirectX::XMFLOAT2   uv;				//UV値
	DirectX::XMFLOAT4   color;			//色データ
	DirectX::XMFLOAT3   normal;			//法線情報
};

namespace MMDBreaker {
	class Mesh {
	private:
		ID3D11Device*		    device;
		ID3D11DeviceContext*	devicecontext;				// デバイスコンテキスト
		DirectX::XMFLOAT4X4		worldMtx;
		DirectX::XMFLOAT3		angle;
		DirectX::XMFLOAT3		trans;
		PMDData*				victimModel;

		ID3D11Buffer*			vertexBuffer = nullptr;  	// 頂点バッファ
		ID3D11Buffer*			indexBuffer = nullptr;      //インデックスバッファ
		ID3D11Buffer*			constantBuffer = nullptr;   //コンスタントバッファ
		CBDATA					cb;

		ID3D11VertexShader*		vertexShader = nullptr;		// 頂点シェーダー
		ID3D11GeometryShader*	geometryShader = nullptr;   //ジオメトリシェーダー
		ID3D11PixelShader*		pixelShader = nullptr;		// ピクセルシェーダー
		ID3D11InputLayout*		vertexLayout = nullptr;		// 頂点レイアウト
		ID3D11ShaderResourceView** srv = nullptr;			// シェーダーリソースビュー

		int vertexListSize;
		int indexListSize;
		int colorCount;
		WORD* indexList = nullptr;

	public:
		bool CreateMesh(const char *vx, 
						const char *px, 
						VERTEX * vert, 
						std::vector<WORD> indexList, 
						PMDData *pmdModel);		//メッシュ作成

		void DrawMesh();		//描画
		void UnInitMesh();

		//Getter
		DirectX::XMFLOAT4X4 GetWorldMtx() { return worldMtx;}
		DirectX::XMFLOAT3   GetAngle() { return angle;}
		DirectX::XMFLOAT3   GetTrans() { return trans;}

		//Setter
		void SetVertexListSize(int count) { vertexListSize = count;}
		void SetIndexListSize(int count)  { indexListSize  = count;}
		void SetColorCount(int count)     { colorCount     = count;}
		void SetWorldMtx(DirectX::XMFLOAT4X4 workMtx) { worldMtx = workMtx;}
		void SetAngle(DirectX::XMFLOAT3 setAngle)     { angle = setAngle;}
		void SetTrans(DirectX::XMFLOAT3 setTrans)     { trans = setTrans;}
	};
}
