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

void Node::Create(ID3D11Device* device, aiNode* node)
{
    // ��� �̸� ���� ����
    m_NodeName = node->mName.C_Str();

    // ��� ���� ��ǥ�� ����
    m_NodeLocalTM = node->mTransformation;

    // �θ� ��尡 �ִٸ� �θ� ����� ���� ��ǥ�踦 ����
    if (node->mParent != nullptr)
    {
        m_NodeWorldTM = m_NodeLocalTM * node->mParent->mTransformation;
    }

    // �θ� ��尡 ���ٸ� ���� ��ǥ�谡 ���� ��ǥ�谡 ��
    m_NodeWorldTM = m_NodeLocalTM;

    // ��尡 �ϳ� �̻��� �޽��� ����Ǿ����� ���� NodeAnimPtr�� ������
    if(node->mNumMeshes > 0)
    {
        //m_NodeAnimPtr = node->aiNodeAnim
    }

}
