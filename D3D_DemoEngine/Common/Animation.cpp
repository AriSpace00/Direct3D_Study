#include "pch.h"
#include "Animation.h"

Animation::Animation()
{
}

Animation::~Animation()
{
}

void Animation::Create(const aiNodeAnim* nodeAnim)
{
    m_NodeAnimation = nodeAnim;
    m_NodeName = nodeAnim->mNodeName;

    m_AnimationKeys.resize(nodeAnim->mNumPositionKeys);

    // �ִϸ��̼� Ű�� ���� �� �ش� Ű ���� ������
    if (nodeAnim->mNumPositionKeys > 0)
    {
        for (int i = 0; i < nodeAnim->mNumPositionKeys; i++)
        {
            m_AnimationKeys[i] = new AnimationKey();

            // ��ġ Ű������ ����
            m_AnimationKeys[i]->Time = nodeAnim->mPositionKeys[i].mTime;
            m_AnimationKeys[i]->Position = ToXMFLOAT3(nodeAnim->mPositionKeys[i].mValue);

            // ȸ�� Ű������ ����
            m_AnimationKeys[i]->Rotation = ToXMFLOAT4(nodeAnim->mRotationKeys[i].mValue);

            // ������ Ű������ ����
            m_AnimationKeys[i]->Scaling = ToXMFLOAT3(nodeAnim->mScalingKeys[i].mValue);

        }
    }
}

void Animation::Evaluate(const float& progressTime, const Vector3& position, const Quaternion& rotation,
    const Vector3& Scaling)
{
    // Ű �����ϱ�
}

DirectX::XMFLOAT3 Animation::ToXMFLOAT3(const aiVector3D& aiVec)
{
    return DirectX::XMFLOAT3(aiVec.x, aiVec.y, aiVec.z);
}

DirectX::XMFLOAT4 Animation::ToXMFLOAT4(const aiQuaternion& aiQuat)
{
    return DirectX::XMFLOAT4(aiQuat.x, aiQuat.y, aiQuat.z, aiQuat.w);
}
