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
//    // �ִϸ��̼� �ε��� (0���� ����)
//    unsigned int animationIndex = 0; // �ִϸ��̼� �ε��� ����
//
//    // �ִϸ��̼� ������ ��������
//    const aiAnimation* animation = m_Scene->mAnimations[animationIndex];
//
//    // �ִϸ��̼� ��� ã��
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
    // ��� �̸� ���� ���� 
    NodeInfo* nodeInfo = new NodeInfo(node, depth);

    // �ش��ϴ� ��� �ִϸ��̼� Ž��
    //nodeInfo->m_NodeAnimPtr = FindNodeAnimation(nodeInfo->m_NodeName);

    // ��� ���Ϳ� �߰�
    m_Nodes.push_back(nodeInfo);

    // �ڽ� ��尡 �ִٸ� �ڽ� ��� ���� ����
    if (node->mNumChildren > 0)
    {
        for (unsigned int i = 0; i < node->mNumChildren; ++i)
        {
            this->Create(node->mChildren[i], depth + 1);
        }
    }
    else
    {
        // �ڽ� ��尡 ���ٸ� depth���� ���� m_Nodes sort
        sort(m_Nodes.begin(), m_Nodes.end(), [](const NodeInfo* a, const NodeInfo* b)-> bool {return a->m_Depth < b->m_Depth; });
    }
}