#pragma once

#include <d3d11.h>
#include <wrl/client.h>
#include <directxtk/SimpleMath.h>

#include <string>
#include <memory>

#include <assimp/scene.h>
#include <assimp/matrix4x4.h>

using namespace Microsoft::WRL;
using namespace DirectX::SimpleMath;
using namespace std;

class Node
{
public:
    Node();
    ~Node();

public:
    string m_NodeName;

    aiMatrix4x4 m_NodeLocalTM;
    aiMatrix4x4 m_NodeWorldTM;

    unique_ptr<aiNodeAnim> m_NodeAnimPtr;


public:
    void Create(ID3D11Device* device, aiNode* node);
    void Update(const float& deltaTime);
};

