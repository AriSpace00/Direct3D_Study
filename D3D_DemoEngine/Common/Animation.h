#pragma once

// Animation Class
// Node Animation�� ������Ʈ�ϰ�, �̸� Node Class�� �����Ѵ�.

#include <vector>
#include <directxtk/SimpleMath.h>

#include <assimp/scene.h>

using namespace std;
using namespace DirectX::SimpleMath;

struct AnimationKey
{
    float Time = 0.0f;
    Vector3 Position;
    Quaternion Rotation;
    Vector3 Scaling;
};

class Animation
{
public:
    Animation();
    ~Animation();

public:
    int m_CurKeyIndex = 0;
    int m_AnimFps = 30;
    float m_Duration = 0.0f;

    aiString m_NodeName;
    const aiNodeAnim* m_NodeAnimation;
    vector<AnimationKey*> m_AnimationKeys;

public:
    // ��� �ִϸ��̼� ����
    void Create(const aiNodeAnim* nodeAnim);
    // �ִϸ��̼� Ű ����
    void Evaluate(const float& progressTime, const Vector3& position, const Quaternion& rotation, const Vector3& Scaling);

    void Update(const float& deltaTime);

    DirectX::XMFLOAT3 ToXMFLOAT3(const aiVector3D& aiVec);
    DirectX::XMFLOAT4 ToXMFLOAT4(const aiQuaternion& aiQuat);
};