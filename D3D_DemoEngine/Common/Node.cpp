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
}

void Node::Render()
{
    // Node Render
}

void Node::SetScene(const aiScene* scene)
{
    m_Scene = scene;
}

//const aiNodeAnim* Node::FindNodeAnimation(const string& nodeName)
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
//        const aiNodeAnim* anim = animation->mChannels[i];
//        if (anim->mNodeName.C_Str() == nodeName)
//        {
//            return anim;
//        }
//        return nullptr;
//    }
//}

void Node::Create(const aiNode* node, int depth)
{
    // 노드 이름 정보 저장 
    NodeInfo* nodeInfo = new NodeInfo(node, depth);

    // 해당하는 노드 애니메이션 탐색
    //nodeInfo->m_NodeAnimPtr = FindNodeAnimation(nodeInfo->m_NodeName);

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
