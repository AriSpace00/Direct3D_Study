#include "pch.h"
#include "Animation.h"
#include "TimeSystem.h"

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

    // 애니메이션 키가 있을 때 해당 키 값을 가져옴
    if (nodeAnim->mNumPositionKeys > 0)
    {
        for (int i = 0; i < nodeAnim->mNumPositionKeys; i++)
        {
            m_AnimationKeys[i] = new AnimationKey();

            // 위치 키프레임 정보
            m_AnimationKeys[i]->Time = nodeAnim->mPositionKeys[i].mTime;
            m_AnimationKeys[i]->Position = ToXMFLOAT3(nodeAnim->mPositionKeys[i].mValue);

            // 회전 키프레임 정보
            m_AnimationKeys[i]->Rotation = ToXMFLOAT4(nodeAnim->mRotationKeys[i].mValue);

            // 스케일 키프레임 정보
            m_AnimationKeys[i]->Scaling = ToXMFLOAT3(nodeAnim->mScalingKeys[i].mValue);

        }
    }
}

void Animation::Evaluate(const float& progressTime, const Vector3& position, const Quaternion& rotation,
    const Vector3& Scaling)
{
    // 키 보간하기
}

void Animation::Update(const float& deltaTime)
{
    m_Duration += deltaTime;
    if (m_Duration > m_AnimationKeys[m_CurKeyIndex]->Time / m_AnimFps)
    {
        m_CurKeyIndex = (m_CurKeyIndex + 1) % 32;
        m_Duration = 0.f;
    }
}

DirectX::XMFLOAT3 Animation::ToXMFLOAT3(const aiVector3D& aiVec)
{
    return DirectX::XMFLOAT3(aiVec.x, aiVec.y, aiVec.z);
}

DirectX::XMFLOAT4 Animation::ToXMFLOAT4(const aiQuaternion& aiQuat)
{
    return DirectX::XMFLOAT4(aiQuat.x, aiQuat.y, aiQuat.z, aiQuat.w);
}
