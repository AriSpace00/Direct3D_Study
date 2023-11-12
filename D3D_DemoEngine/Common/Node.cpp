#include "pch.h"
#include "Node.h"
#include "Helper.h"
#include "Model.h"

Node::Node()
    : m_NodeAnimPtr(nullptr)
{
}

Node::~Node()
{
}

void Node::Update(ID3D11DeviceContext* deviceContext)
{
    deviceContext->UpdateSubresource(m_WorldTMBuffer, 0, nullptr, &m_NodeWorldTM, 0, 0);
    deviceContext->VSSetConstantBuffers(0, 1, &m_WorldTMBuffer);
    deviceContext->PSSetConstantBuffers(0, 1, &m_WorldTMBuffer);

    for (int i = 0; i < m_Childrens.size(); i++)
    {
        m_Childrens[i].Update(deviceContext);
    }
}

void Node::Render(ID3D11DeviceContext* deviceContext)
{
    // Node Render
}

Matrix Node::GetParentWorldTransform(const aiNode* parentNode)
{
    Matrix parentLocalTM = ConvertaiMatrixToXMMatrix(parentNode->mTransformation);

    if (parentNode->mParent != nullptr)
    {
        Matrix parentWorldTM = parentLocalTM * GetParentWorldTransform(parentNode->mParent);
        return parentWorldTM;
    }
    else
    {
        return parentLocalTM;
    }
}

void Node::FindNodeAnimation(const aiNode* node)
{
    if (m_Scene->mNumAnimations > 0)
    {
        for (int i = 0; i < m_Scene->mNumAnimations; i++)
        {
            if (m_Scene->mAnimations[i]->mNumChannels > 0)
            {
                for (int j = 0; j < m_Scene->mAnimations[i]->mNumChannels; j++)
                {
                    if (m_NodeName == m_Scene->mAnimations[i]->mChannels[j]->mNodeName.C_Str())
                    {
                        m_NodeAnimPtr = m_Scene->mAnimations[i]->mChannels[j];
                    }
                }
            }
        }
    }
}

void Node::SetScene(const aiScene* scene)
{
    m_Scene = scene;
}

void Node::Create(const aiNode* node, Model* model)
{
    model->m_Nodes.push_back(this);

    m_NodeName = node->mName.C_Str();

    m_NodeLocalTM = ConvertaiMatrixToXMMatrix(node->mTransformation);

    if (node->mParent != nullptr)
    {
        m_NodeWorldTM = m_NodeLocalTM * GetParentWorldTransform(node->mParent);
    }
    else
    {
        m_NodeWorldTM = m_NodeLocalTM;
    }

    if (node->mNumChildren > 0)
    {
        m_Childrens.resize(node->mNumChildren);
        for (int i = 0; i < node->mNumChildren; i++)
        {
            Create(node->mChildren[i], model);
        }
    }
}