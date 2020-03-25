#pragma once
//���b�V�����쐬����class
#include "DX11Settransform.h"
#include "dx11mathutil.h"
#include "modelFileLoader.h"

//���b�V���f�[�^�\����
struct MESHDATA {
	DirectX::XMFLOAT3   vertsPos;		//���_���
	DirectX::XMFLOAT2   uv;				//UV�l
	DirectX::XMFLOAT4   color;			//�F�f�[�^
	DirectX::XMFLOAT3   normal;			//�@�����
};

namespace MMDBreaker {
	class Mesh {
	private:
		ID3D11Device*		    device;
		ID3D11DeviceContext*	devicecontext;				// �f�o�C�X�R���e�L�X�g
		DirectX::XMFLOAT4X4		worldMtx;
		DirectX::XMFLOAT3		angle;
		DirectX::XMFLOAT3		trans;
		PMDData*				victimModel;

		ID3D11Buffer*			vertexBuffer = nullptr;  	// ���_�o�b�t�@
		ID3D11Buffer*			indexBuffer = nullptr;      //�C���f�b�N�X�o�b�t�@
		ID3D11Buffer*			constantBuffer = nullptr;   //�R���X�^���g�o�b�t�@
		CBDATA					cb;

		ID3D11VertexShader*		vertexShader = nullptr;		// ���_�V�F�[�_�[
		ID3D11GeometryShader*	geometryShader = nullptr;   //�W�I���g���V�F�[�_�[
		ID3D11PixelShader*		pixelShader = nullptr;		// �s�N�Z���V�F�[�_�[
		ID3D11InputLayout*		vertexLayout = nullptr;		// ���_���C�A�E�g
		ID3D11ShaderResourceView** srv = nullptr;			// �V�F�[�_�[���\�[�X�r���[

		int vertexListSize;
		int indexListSize;
		int colorCount;
		WORD* indexList = nullptr;

	public:
		bool CreateMesh(const char *vx, 
						const char *px, 
						VERTEX * vert, 
						std::vector<WORD> indexList, 
						PMDData *pmdModel);		//���b�V���쐬

		void DrawMesh();		//�`��
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
