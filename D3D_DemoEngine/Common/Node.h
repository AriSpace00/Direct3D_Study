#pragma once

// Node Class
// 각 Node의 localTM, WorldTM, NodeAnimation 정보가 업데이트되어 저장된다.

#include <d3d11.h>
#include <wrl/client.h>
#include <directxtk/SimpleMath.h>

#include <string>
#include <vector>

#include <assimp/scene.h>
#include <assimp/matrix4x4.h>

#include "Animation.h"

using namespace Microsoft::WRL;
using namespace DirectX::SimpleMath;
using namespace std;

class Model;

class Node
{
public:
    Node();
    ~Node();

public:
    const aiScene* m_Scene;
    const aiNode* m_Node;

    string m_NodeName;
    string m_ParentNodeName;

    Matrix m_NodeLocalTM;
    Matrix m_NodeWorldTM;

    const aiNodeAnim* m_NodeAnimPtr;

    vector<Node> m_Childrens;

public:
    void SetScene(const aiScene* scene);
    void Create(ID3D11Device* device, Model* model, const aiNode* node);

    // 애니메이션 업데이트
    void Update(const float& deltaTime, Model* model, const aiNode* rootNode);
    void Render(ID3D11DeviceContext* deviceContext);

private:
    Matrix GetParentWorldTransform(const aiNode* parentNode);
    void FindNodeAnimation(const aiNode* node);
};

