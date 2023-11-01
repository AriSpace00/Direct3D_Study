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

struct NodeInfo
{
    int m_Depth = 0;
    aiString m_NodeName;
    const aiNode* m_Node;

    aiMatrix4x4 m_NodeLocalTM;
    aiMatrix4x4 m_NodeWorldTM;

    //const aiNodeAnim* m_NodeAnimPtr;

    NodeInfo(const aiNode* node, int depth)
    {      
        // 노드 정보 설정
        m_Node = node;
        m_Depth = depth;
        m_NodeName = node->mName;

        // 노드 로컬 좌표계 설정
        m_NodeLocalTM = node->mTransformation;

        /*if(m_NodeAnimPtr != nullptr)
        {
            Animation* nodeAnimation = new Animation();

            nodeAnimation->Create(m_NodeAnimPtr);
        }*/

        // 부모 노드가 있다면 부모 노드의 월드 좌표계를 곱함
        if (node->mParent != nullptr)
        {
            m_NodeWorldTM = node->mParent->mTransformation * m_NodeLocalTM;
        }

        // 부모 노드가 없다면 로컬 좌표계가 월드 좌표계가 됨
        m_NodeWorldTM = m_NodeLocalTM;
    }
};

class Node
{
public:
    Node();
    ~Node();

public:
    vector<NodeInfo*> m_Nodes;
    const aiScene* m_Scene;

public:
    void SetScene(const aiScene* scene);
    const aiNodeAnim* FindNodeAnimation(const string& nodeName);
    void Create(const aiNode* node, int depth = 0);
    void Update(const float& deltaTime);
    void Render(ID3D11DeviceContext* device);
    void UpdateWorldTransform(aiNode* node);
};

