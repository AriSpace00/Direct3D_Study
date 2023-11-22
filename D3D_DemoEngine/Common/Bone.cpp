#include "pch.h"
#include "Bone.h"
#include "Mesh.h"
#include "Helper.h"

Bone::Bone()
{
}

Bone::~Bone()
{
}

void Bone::Create(aiBone* bone, int boneIndex)
{
    m_BoneName = bone->mName.C_Str();
    m_OffsetMatrix = ConvertaiMatrixToXMMatrix(bone->mOffsetMatrix);
    m_BoneIndex = boneIndex;
}
