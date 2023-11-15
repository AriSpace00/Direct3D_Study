#include "pch.h"
#include "Node.h"
#include "Helper.h"
#include "Model.h"
#include "Mesh.h"

Node::Node()
    : m_ParentNode(nullptr)
    , m_Animation(nullptr)
{
}

Node::~Node()
{
}

void Node::Update(const float& deltaTime, Model* model)
{
    m_Animation->Update(deltaTime);

    if (m_Animation->m_AnimationKeys.size() > 0) 
    {
        Matrix position = Matrix::CreateTranslation(m_Animation->m_AnimationKeys[m_Animation->m_CurKeyIndex]->Position);
        Matrix rotation = Matrix::CreateFromQuaternion(m_Animation->m_AnimationKeys[m_Animation->m_CurKeyIndex]->Rotation);
        Matrix scale = Matrix::CreateScale(m_Animation->m_AnimationKeys[m_Animation->m_CurKeyIndex]->Scaling);

        m_NodeLocalTM = scale * rotation * position;
    }

    if (m_ParentNode != nullptr)
    {
        m_NodeWorldTM = m_NodeLocalTM * GetParentWorldTransform(m_ParentNode);
    }
    else
    {
        m_NodeWorldTM = m_NodeLocalTM;
    }

    for (int i = 0; i < m_Meshes.size(); i++)
    {
        m_Meshes[i].m_NodeWorldTM = m_NodeWorldTM;

        m_Meshes[i].m_NodeWorldTM *= model->m_Scale;
        m_Meshes[i].m_NodeWorldTM *= model->m_Rotation;
        m_Meshes[i].m_NodeWorldTM *= model->m_Position;
    }
}

void Node::Render(ID3D11DeviceContext* deviceContext, Model* model)
{
    // Node Render
    for (size_t i = 0; i < m_Meshes.size(); i++)
    {
        model->m_Transform.WorldMatrix = XMMatrixTranspose(m_Meshes[i].m_NodeWorldTM);
        deviceContext->UpdateSubresource(model->m_CBTransform, 0, nullptr, &model->m_Transform, 0, 0);
        deviceContext->VSSetConstantBuffers(0, 1, &model->m_CBTransform);
        deviceContext->PSSetConstantBuffers(0, 1, &model->m_CBTransform);

        deviceContext->IASetVertexBuffers(0, 1, &m_Meshes[i].m_VertexBuffer, &m_Meshes[i].m_VertexBufferStride, &m_Meshes[i].m_VertexBufferOffset);
        deviceContext->IASetIndexBuffer(m_Meshes[i].m_IndexBuffer, DXGI_FORMAT_R16_UINT, 0);

        deviceContext->DrawIndexed(m_Meshes[i].m_IndexCount, 0, 0);
    }
}

Matrix Node::GetParentWorldTransform(const Node* parentNode)
{
    Matrix parentLocalTM = parentNode->m_NodeLocalTM;

    if (parentNode->m_ParentNode != nullptr)
    {
        Matrix parentWorldTM = parentLocalTM * GetParentWorldTransform(parentNode->m_ParentNode);
        return parentWorldTM;
    }
    else
    {
        return parentLocalTM;
    }
}

void Node::FindNodeAnimation(const aiScene* scene, const aiNode* node)
{
    m_Animation = new Animation();

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
                        aiNodeAnim* nodeAnimPtr = scene->mAnimations[i]->mChannels[j];
                        m_Animation->Create(nodeAnimPtr);
                    }
                }
            }
        }
    }
}

void Node::Create(ID3D11Device* device, Model* model, const aiScene* scene, const aiNode* node)
{
    m_NodeName = node->mName.C_Str();
    m_NodeLocalTM = ConvertaiMatrixToXMMatrix(node->mTransformation);

    if (node->mParent != nullptr)
    {
        for (int i = 0; i < model->m_Nodes.size(); i++)
        {
            if (node->mParent->mName.C_Str() == model->m_Nodes[i]->m_NodeName)
            {
                m_ParentNode = new Node();
                m_ParentNode = model->m_Nodes[i];
                break;
            }
        }
    }

    FindNodeAnimation(scene, node);

    model->m_Animations.push_back(m_Animation);
    model->m_Nodes.push_back(this);

    if (node->mNumMeshes > 0)
    {
        int meshIndex = m_Meshes.size();
        for (int i = 0; i < node->mNumMeshes; i++)
        {
            aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
            m_Meshes.push_back(Mesh());
            m_Meshes[meshIndex].Create(device, mesh);
            meshIndex++;
        }
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
