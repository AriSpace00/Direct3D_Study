#include "pch.h"
#include "Node.h"
#include "Helper.h"
#include "Model.h"
#include "Mesh.h"

Node::Node()
    : m_NodeAnimPtr(nullptr)
{
}

Node::~Node()
{
}

void Node::Update(const float& deltaTime, Model* model, const aiNode* rootNode)
{
    
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

void Node::FindNodeAnimation(const aiScene* scene, const aiNode* node)
{
    if (scene->mNumAnimations > 0)
    {
        for (int i = 0; i < scene->mNumAnimations; i++)
        {
            if (scene->mAnimations[i]->mNumChannels > 0)
            {
                for (int j = 0; j < scene->mAnimations[i]->mNumChannels; j++)
                {
                    if (m_NodeName == scene->mAnimations[i]->mChannels[j]->mNodeName.C_Str())
                    {
                        m_NodeAnimPtr = scene->mAnimations[i]->mChannels[j];
                    }
                }
            }
        }
    }
}

void Node::Create(ID3D11Device* device, Model* model, const aiScene* scene, const aiNode* node)
{
    m_Node = node;
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
    model->m_Nodes.push_back(this);

    int meshIndex = model->m_Meshes.size();
    for (int i = 0; i < node->mNumMeshes; i++)
    {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        model->m_Meshes.push_back(Mesh());
        model->m_Meshes[meshIndex].Create(device, mesh);
        meshIndex++;
    }

    if (node->mNumChildren > 0)
    {
        m_Childrens.resize(node->mNumChildren);
        for (int i = 0; i < node->mNumChildren; i++)
        {
            m_Childrens[i].Create(device, model, scene, node->mChildren[i]);
        }
    }
}
