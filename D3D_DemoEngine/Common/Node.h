#pragma once

// Node Class
// �� Node�� localTM, WorldTM, NodeAnimation ������ ������Ʈ�Ǿ� ����ȴ�.

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
        // ��� ���� ����
        m_Node = node;
        m_Depth = depth;
        m_NodeName = node->mName;

        // �θ� ��尡 �ִٸ� �̸� ����
        if(node->mParent != nullptr)
        {
            m_NodeParentName = node->mParent->mName;
        }

        // ��� ���� ��ǥ�� ����
        m_NodeLocalTM = node->mTransformation;

        // ��� �ִϸ��̼��� �ִٸ�, �ִϸ��̼� �����
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
        // �ִϸ��̼� �ε��� (0���� ����)
        unsigned int animationIndex = 0; // �ִϸ��̼� �ε��� ����

        // �ִϸ��̼� ������ ��������
        const aiAnimation* animation = m_NodeInfoScene->mAnimations[animationIndex];

        // �ִϸ��̼� ��� ã��
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

    // �ִϸ��̼� ������Ʈ
    void Update(const float& deltaTime);
    void Render(ID3D11DeviceContext* device);

private:
    aiMatrix4x4 GetParentWorldTransform(aiNode* parentNode);

    // XMMATRIX�� aiMatrix4x4�� ��ȯ�ϴ� �Լ�
    DirectX::XMFLOAT4X4 ConvertXMMATRIXToXMFLOAT4X4(const DirectX::XMMATRIX& xmMatrix);
    aiMatrix4x4 ConvertXMMATRIXToaiMatrix4x4(const DirectX::XMMATRIX& xmMatrix);
};

