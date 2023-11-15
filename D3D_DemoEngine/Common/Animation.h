#pragma once

// Animation Class
// Node Animation을 업데이트하고, 이를 Node Class에 연결한다.

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
    Vector3 Scale;
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

    string m_NodeName;
    vector<AnimationKey*> m_AnimationKeys;

    Matrix m_InterpolationTM;

public:
    // 노드 애니메이션 생성
    void Create(const aiNodeAnim* nodeAnim);
    // 애니메이션 키 보간
    void Evaluate(const float& progressTime);

    void Update(const float& deltaTime);

    DirectX::XMFLOAT3 ToXMFLOAT3(const aiVector3D& aiVec);
    DirectX::XMFLOAT4 ToXMFLOAT4(const aiQuaternion& aiQuat);
};