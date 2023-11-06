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
    aiString m_NodeParentName;

    aiMatrix4x4 m_NodeLocalTM;
    aiMatrix4x4 m_NodeWorldTM;

    const aiNode* m_Node;
    const aiScene* m_NodeInfoScene;
    const aiNodeAnim* m_NodeAnimPtr;

    Animation* m_NodeAnimation;

    NodeInfo(const aiNode* node, int depth, const aiScene* scene)
    {      
        // 노드 정보 설정
        m_Node = node;
        m_Depth = depth;
        m_NodeName = node->mName;

        // 부모 노드가 있다면 이름 저장
        if(node->mParent != nullptr)
        {
            m_NodeParentName = node->mParent->mName;
        }

        // 노드 로컬 좌표계 설정
        m_NodeLocalTM = node->mTransformation;

        // 노드 애니메이션이 있다면, 애니메이션 만들기
        m_NodeInfoScene = scene;
        if(m_NodeInfoScene->mAnimations != nullptr)
        {
            this->FindNodeAnimation();
            this->CreateNodeAnimation();
        }
    }

    void CreateNodeAnimation() 
    {
        if(m_NodeAnimPtr != nullptr)
        {
            m_NodeAnimation = new Animation();
            m_NodeAnimation->Create(m_NodeAnimPtr);
        }
    }

    void FindNodeAnimation()
    {
        // 애니메이션 인덱스 (0부터 시작)
        unsigned int animationIndex = 0; // 애니메이션 인덱스 설정

        // 애니메이션 데이터 가져오기
        const aiAnimation* animation = m_NodeInfoScene->mAnimations[animationIndex];

        // 애니메이션 노드 찾기
        for (unsigned int i = 0; i < animation->mNumChannels; i++)
        {
            aiNodeAnim* anim = animation->mChannels[i];

            if (anim->mNodeName == m_NodeName)
            {
                m_NodeAnimPtr = anim;
            }
            else
            {
                m_NodeAnimPtr = nullptr;
            }
        }
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
    void Create(const aiNode* node, int depth = 0);

    // 애니메이션 업데이트
    void Update(const float& deltaTime);
    void Render(ID3D11DeviceContext* device);

private:
    aiMatrix4x4 GetParentWorldTransform(aiNode* parentNode);

    // XMMATRIX를 aiMatrix4x4로 변환하는 함수
    DirectX::XMFLOAT4X4 ConvertXMMATRIXToXMFLOAT4X4(const DirectX::XMMATRIX& xmMatrix);
    aiMatrix4x4 ConvertXMMATRIXToaiMatrix4x4(const DirectX::XMMATRIX& xmMatrix);
};

