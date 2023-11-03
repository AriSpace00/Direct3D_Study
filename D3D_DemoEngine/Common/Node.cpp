#include "pch.h"
#include "Node.h"

Node::Node()
{
}

Node::~Node()
{
}

void Node::Update(const float& deltaTime)
{
    for (int i = 0; i < m_Nodes.size(); i++)
    {
        if (m_Nodes[i]->m_NodeAnimPtr != nullptr)
        {
            // 노드의 local Transform을 애니메이션 Transform과 곱해서 업데이트
            vector<AnimationKey*> nodeAnimKeys = m_Nodes[i]->m_NodeAnimation->m_AnimationKeys;
            for (int j = 0; j < nodeAnimKeys.size(); j++)
            {
                nodeAnimKeys[j];
            }
        }
    }
}

void Node::Render(ID3D11DeviceContext* deviceContext)
{
    // Node Render
}

aiMatrix4x4 Node::GetParentWorldTransform(aiNode* parentNode)
{
    // 부모의 WorldTransform은 NodeInfo에 저장되어 있으므로 WorldTransform 업데이트
    for (int i = 0; i < m_Nodes.size(); i++)
    {
        if (m_Nodes[i]->m_NodeName == parentNode->mName)
        {
            return m_Nodes[i]->m_NodeWorldTM;
        }
    }
}

void Node::SetScene(const aiScene* scene)
{
    m_Scene = scene;
}

//const aiNodeAnim* Node::FindNodeAnimation(const aiString& nodeName)
//{
//    // 애니메이션 인덱스 (0부터 시작)
//    unsigned int animationIndex = 0; // 애니메이션 인덱스 설정
//
//    // 애니메이션 데이터 가져오기
//    const aiAnimation* animation = m_Scene->mAnimations[animationIndex];
//
//    // 애니메이션 노드 찾기
//    for (unsigned int i = 0; i < animation->mNumChannels; i++) 
//    {
//        aiNodeAnim* anim = animation->mChannels[i];
//        
//        if (anim->mNodeName == nodeName)
//        {
//            return anim;
//        }
//        return nullptr;
//    }
//}

void Node::Create(const aiNode* node, int depth)
{
    // 노드 이름 정보 저장 
    NodeInfo* nodeInfo = new NodeInfo(node, depth, m_Scene);

    // 노드 월드 좌표계 설정
    if (node->mParent != nullptr)
    {
        nodeInfo->m_NodeWorldTM = GetParentWorldTransform(node->mParent) * nodeInfo->m_NodeLocalTM;
    }
    else
    {
        nodeInfo->m_NodeWorldTM = nodeInfo->m_NodeLocalTM;
    }

    // 노드 벡터에 추가
    m_Nodes.push_back(nodeInfo);

    // 자식 노드가 있다면 자식 노드 또한 생성
    if (node->mNumChildren > 0)
    {
        for (unsigned int i = 0; i < node->mNumChildren; ++i)
        {
            this->Create(node->mChildren[i], depth + 1);
        }
    }
    else
    {
        // 자식 노드가 없다면 depth값에 따라 m_Nodes sort
        sort(m_Nodes.begin(), m_Nodes.end(), [](const NodeInfo* a, const NodeInfo* b)-> bool {return a->m_Depth < b->m_Depth; });
    }
}
