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
    // 위치, 회전 및 스케일 키 프레임 가져오기
    AnimationKey* animKey = new AnimationKey();

    for (unsigned int i = 0; i < nodeAnim->mNumPositionKeys; i++) {
        animKey->Position = ToXMFLOAT3(nodeAnim->mPositionKeys[i].mValue);
        // 위치 키프레임 정보 사용
    }

    for (unsigned int i = 0; i < nodeAnim->mNumRotationKeys; i++) {
        animKey->Rotation = ToXMFLOAT4(nodeAnim->mRotationKeys[i].mValue);
        // 회전 키프레임 정보 사용
    }

    for (unsigned int i = 0; i < nodeAnim->mNumScalingKeys; i++) {
        animKey->Scaling = ToXMFLOAT3(nodeAnim->mScalingKeys[i].mValue);
        // 스케일 키프레임 정보 사용
    }

    m_AnimationKeys.push_back(animKey);
    
}

void Animation::Evaluate(const float& progressTime, const Vector3& position, const Quaternion& rotation,
    const Vector3& Scaling)
{
}

void Animation::Update(const float& deltaTime)
{
}

DirectX::XMFLOAT3 Animation::ToXMFLOAT3(const aiVector3D& aiVec)
{
    return DirectX::XMFLOAT3(aiVec.x, aiVec.y, aiVec.z);
}

DirectX::XMFLOAT4 Animation::ToXMFLOAT4(const aiQuaternion& aiQuat)
{
    return DirectX::XMFLOAT4(aiQuat.x, aiQuat.y, aiQuat.z, aiQuat.w);
}
