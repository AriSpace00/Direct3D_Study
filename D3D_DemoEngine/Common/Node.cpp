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
    // 노드 이름 정보 저장
    m_NodeName = node->mName.C_Str();

    // 노드 로컬 좌표계 설정
    m_NodeLocalTM = node->mTransformation;

    // 부모 노드가 있다면 부모 노드의 월드 좌표계를 곱함
    if (node->mParent != nullptr)
    {
        m_NodeWorldTM = m_NodeLocalTM * node->mParent->mTransformation;
    }

    // 부모 노드가 없다면 로컬 좌표계가 월드 좌표계가 됨
    m_NodeWorldTM = m_NodeLocalTM;

    // 노드가 하나 이상의 메쉬에 연결되어있을 때만 NodeAnimPtr이 존재함
    if(node->mNumMeshes > 0)
    {
        //m_NodeAnimPtr = node->aiNodeAnim
    }

}
