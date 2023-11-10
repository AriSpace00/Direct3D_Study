#include "pch.h"
#include "Node.h"
#include "Helper.h"

Node::Node()
{
}

Node::~Node()
{
}

void Node::Update(const float& deltaTime)
{
    // �ִϸ��̼��� ���� ��� Ʈ������ ������Ʈ
    for (int i = 0; i < m_Nodes.size(); i++) 
    {
        if (m_Nodes[i]->m_NodeAnimPtr != nullptr)
        {
            m_Nodes[i]->m_NodeAnimation->Update(deltaTime);

            // ����� local Transform�� �ִϸ��̼� Transform�� ���ؼ� ������Ʈ
            vector<AnimationKey*>& nodeAnimKeys = m_Nodes[i]->m_NodeAnimation->m_AnimationKeys;
            int curKeyIndex = m_Nodes[i]->m_NodeAnimation->m_CurKeyIndex;
            if (curKeyIndex > 0) curKeyIndex--;

            Matrix animTM = Matrix::CreateScale(nodeAnimKeys[curKeyIndex]->Scaling) * Matrix::CreateFromQuaternion(nodeAnimKeys[curKeyIndex]->Rotation) * Matrix::CreateTranslation(nodeAnimKeys[curKeyIndex]->Position);

            aiMatrix4x4 nodeWorldTM = ConvertXMMATRIXToaiMatrix4x4(animTM);

            if(m_Nodes[i]->m_Node->mParent != nullptr)
            {
                aiMatrix4x4 parentWorldTM = GetParentWorldTransform(m_Nodes[i]->m_Node->mParent);
                m_Nodes[i]->m_NodeWorldTM = parentWorldTM * nodeWorldTM;
            }
            else
            {
                m_Nodes[i]->m_NodeWorldTM = nodeWorldTM;
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
    // �θ��� WorldTransform�� NodeInfo�� ����Ǿ� �����Ƿ� WorldTransform ������Ʈ
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

void Node::Create(const aiNode* node, int depth)
{
    // ��� �̸� ���� ���� 
    NodeInfo* nodeInfo = new NodeInfo(node, depth, m_Scene);

    // ��� ���� ��ǥ�� ����
    if (node->mParent != nullptr)
    {
        nodeInfo->m_NodeWorldTM = GetParentWorldTransform(node->mParent) * nodeInfo->m_NodeLocalTM;
    }
    else
    {
        nodeInfo->m_NodeWorldTM = nodeInfo->m_NodeLocalTM;
    }

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