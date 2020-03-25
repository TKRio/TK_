//*****************************************************************************
//!	@file	explosion.h
//!	@brief	”š”­ˆ—ŠÖ˜A
//!	@note	
//!	@author
//*****************************************************************************
#pragma once

//-----------------------------------------------------------------------------
//	Include header files.
//-----------------------------------------------------------------------------
#include	"dx11mathutil.h"
#include	<DirectXMath.h>
#include	"PMXModel.h"
//-----------------------------------------------------------------------------
// ƒ}ƒNƒéŒ¾
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// ƒvƒƒgƒ^ƒCƒvéŒ¾
//-----------------------------------------------------------------------------

// ’¸“_î•ñ
struct	MyVertex{
	DirectX::XMFLOAT3   Pos;
	DirectX::XMFLOAT4	col;
	float				tu, tv;
};

// Œ‹üî•ñ
struct MyConnect{
	WORD		idx[3];
};

// ‚RŠpŒ`î•ñ
struct	MyTriangle{
	MyVertex		Vertex[3];					// ‚RŠpŒ`‚Ì’¸“_
	float			xangle;						// ‰ñ“]Šp“x‚w
	float			yangle;						// ‰ñ“]Šp“x‚x
	float			zangle;						// ‰ñ“]Šp“x‚y
	float			dxa;						// ‰ñ“]Šp“x‘•ª‚w
	float			dya;						// ‰ñ“]Šp“x‘•ª‚x
	float			dza;						// ‰ñ“]Šp“x‘•ª‚y
	float			cx, cy, cz;					// dSÀ•W
	float			nx, ny, nz;					// –@üƒxƒNƒgƒ‹

	DirectX::XMFLOAT4X4		mat;				// •ÏŠ·s—ñ
	MyTriangle(){
		xangle = 0.0f;							// ‰ñ“]Šp“x‚w‰Šú‰»
		yangle = 0.0f;							// ‰ñ“]Šp“x‚x‰Šú‰»
		zangle = 0.0f;							// ‰ñ“]Šp“x‚y‰Šú‰»
		dxa = (float)((float)(rand() / (float)RAND_MAX)*10);	// ‘•ª‰Šú‰»‚w
		dya = (float)((float)(rand() / (float)RAND_MAX)*10);	// ‘•ª‰Šú‰»‚x
		dza = (float)((float)(rand() / (float)RAND_MAX)*10);	// ‘•ª‰Šú‰»‚y
		DX11MatrixIdentity(mat);
	}
};

void TriangleTransforms(const DirectX::XMFLOAT4X4& mat);
void PMDToTriangle(Model::cPmd* pmdData);
void ExplosionInit(Model::cPmd* pmdData);
void ExplosionUpdate();
void ExplosionDraw();
void ExplosionExit();

//******************************************************************************
//	End of file.
//******************************************************************************
