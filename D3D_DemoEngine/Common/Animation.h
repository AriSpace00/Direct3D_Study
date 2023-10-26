#pragma once

#include <string>
#include <vector>
#include <directxtk/SimpleMath.h>

using namespace std;
using namespace DirectX::SimpleMath;

struct AnimationKey
{
    float Time = 0.0f;
    Vector3 Position;
    Quaternion Rotation;
    Vector3 Scaling;
};

struct NodeAnimation
{
    string m_NodeName;
    vector<AnimationKey> m_AnimationKeys;

    // ��� �ִϸ��̼� ����
    void Create();
    // ��� �ִϸ��̼��� ���������� �Ǵ� 
    void Evaluate(const float& progressTime, const Vector3& position, const Quaternion& rotation, const Vector3& Scaling);
};

class Animation
{
public:
    Animation();
    ~Animation();

public:
    float m_Duration = 0.0f;
    vector<NodeAnimation> m_NodeAnimations;

public:
    void Update(const float& deltaTime);
};