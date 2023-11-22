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

HRESULT CompileShaderFromFileWithMacro(const WCHAR* szFileName, const D3D_SHADER_MACRO* szDefines, LPCSTR szEntryPoint,
                                       LPCSTR szShaderModel, ID3DBlob** ppBlobOut)
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
	hr = D3DCompileFromFile(szFileName, szDefines, D3D_COMPILE_STANDARD_FILE_INCLUDE, szEntryPoint, szShaderModel,
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

DirectX::XMMATRIX ConvertaiMatrixToXMMatrix(const aiMatrix4x4& aiMatrix)
{
	DirectX::XMMATRIX xmMatrix;
	xmMatrix.r[0] = DirectX::XMVectorSet(aiMatrix.a1, aiMatrix.b1, aiMatrix.c1, aiMatrix.d1);
	xmMatrix.r[1] = DirectX::XMVectorSet(aiMatrix.a2, aiMatrix.b2, aiMatrix.c2, aiMatrix.d2);
	xmMatrix.r[2] = DirectX::XMVectorSet(aiMatrix.a3, aiMatrix.b3, aiMatrix.c3, aiMatrix.d3);
	xmMatrix.r[3] = DirectX::XMVectorSet(aiMatrix.a4, aiMatrix.b4, aiMatrix.c4, aiMatrix.d4);
	return xmMatrix;
}
