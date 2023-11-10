#include "pch.h"
#include "Helper.h"
#include <comdef.h>
#include <d3dcompiler.h>

LPCWSTR GetComErrorString(HRESULT hr)
{
	_com_error err(hr);
	LPCWSTR errMsg = err.ErrorMessage();
	return errMsg;
}

HRESULT CompileShaderFromFile(const WCHAR* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut)
{
	HRESULT hr = S_OK;

	DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#ifdef _DEBUG
	// Set the D3DCOMPILE_DEBUG flag to embed debug information in the shaders.
	// Setting this flag improves the shader debugging experience, but still allows 
	// the shaders to be optimized and to run exactly the way they will run in 
	// the release configuration of this program.
	dwShaderFlags |= D3DCOMPILE_DEBUG;

	// Disable optimizations to further improve shader debugging
	dwShaderFlags |= D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

	ID3DBlob* pErrorBlob = nullptr;
	hr = D3DCompileFromFile(szFileName, nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, szEntryPoint, szShaderModel,
		dwShaderFlags, 0, ppBlobOut, &pErrorBlob);
	if (FAILED(hr))
	{
		if (pErrorBlob)
		{
			MessageBoxA(NULL, (char*)pErrorBlob->GetBufferPointer(), "CompileShaderFromFile", MB_OK);
			pErrorBlob->Release();
		}
		return hr;
	}
	if (pErrorBlob) pErrorBlob->Release();

	return S_OK;
}

DirectX::XMFLOAT4X4 ConvertXMMATRIXToXMFLOAT4X4(const DirectX::XMMATRIX& xmMatrix)
{
	DirectX::XMFLOAT4X4 result;
	DirectX::XMStoreFloat4x4(&result, xmMatrix);
	return result;
}

aiMatrix4x4 ConvertXMMATRIXToaiMatrix4x4(const DirectX::XMMATRIX& xmMatrix)
{
	DirectX::XMFLOAT4X4 float4x4 = ConvertXMMATRIXToXMFLOAT4X4(xmMatrix);
	aiMatrix4x4 aiMatrix;
	aiMatrix.a1 = float4x4._11;
	aiMatrix.a2 = float4x4._12;
	aiMatrix.a3 = float4x4._13;
	aiMatrix.a4 = float4x4._14;
	aiMatrix.b1 = float4x4._21;
	aiMatrix.b2 = float4x4._22;
	aiMatrix.b3 = float4x4._23;
	aiMatrix.b4 = float4x4._24;
	aiMatrix.c1 = float4x4._31;
	aiMatrix.c2 = float4x4._32;
	aiMatrix.c3 = float4x4._33;
	aiMatrix.c4 = float4x4._34;
	aiMatrix.d1 = float4x4._41;
	aiMatrix.d2 = float4x4._42;
	aiMatrix.d3 = float4x4._43;
	aiMatrix.d4 = float4x4._44;
	return aiMatrix;
}

DirectX::XMMATRIX ConvertaiMatrixToXMMATRIX(const aiMatrix4x4& aiMatrix)
{
	return DirectX::XMMATRIX(
		aiMatrix.a1, aiMatrix.a2, aiMatrix.a3, aiMatrix.a4,
		aiMatrix.b1, aiMatrix.b2, aiMatrix.b3, aiMatrix.b4,
		aiMatrix.c1, aiMatrix.c2, aiMatrix.c3, aiMatrix.c4,
		aiMatrix.d1, aiMatrix.d2, aiMatrix.d3, aiMatrix.d4
	);
}
