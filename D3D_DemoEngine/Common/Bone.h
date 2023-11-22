#pragma once

#include <directxtk/SimpleMath.h>
#include <assimp/mesh.h>

#include <string>

using namespace std;
using namespace DirectX::SimpleMath;

class Bone
{
public:
    Bone();
    ~Bone();

public:
    string m_BoneName;
    Matrix m_OffsetMatrix;
    int m_BoneIndex;

public:
    void Create(aiBone* bone, int boneIndex);
};

