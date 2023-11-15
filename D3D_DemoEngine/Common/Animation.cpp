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
    m_NodeName = nodeAnim->mNodeName.C_Str();
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
            m_AnimationKeys[i]->Scale = ToXMFLOAT3(nodeAnim->mScalingKeys[i].mValue);
        }
    }
}

void Animation::Evaluate(const float& progressTime)
{
    int nextKeyIndex = (m_CurKeyIndex + 1) % m_AnimationKeys.size();

    const AnimationKey* curKey = m_AnimationKeys[m_CurKeyIndex];
    const AnimationKey* nextKey = m_AnimationKeys[nextKeyIndex];

    Vector3 interpolationPosition = Vector3::Lerp(curKey->Position, nextKey->Position, progressTime);
    Vector4 interpolationRotation = Quaternion::Lerp(curKey->Rotation, nextKey->Rotation, progressTime);
    Vector3 interpolationScale = Vector3::Lerp(curKey->Scale, nextKey->Scale, progressTime);

    Matrix interpolationTM = Matrix::CreateScale(interpolationScale) * Matrix::CreateFromQuaternion(interpolationRotation) * Matrix::CreateTranslation(interpolationPosition);

    m_InterpolationTM = interpolationTM;
}

void Animation::Update(const float& deltaTime)
{
    if (m_AnimationKeys.size() > 0)
    {
        m_Duration += deltaTime;
        if (m_Duration > m_AnimationKeys[m_CurKeyIndex]->Time / m_AnimFps)
        {
            m_CurKeyIndex = (m_CurKeyIndex + 1) % m_AnimationKeys.size();
            m_Duration = 0.f;
        }
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
