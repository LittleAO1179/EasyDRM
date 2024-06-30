#include "model.h"
#include <algorithm>
#include <cstddef>

Model::Model()
{

}

Model::~Model()
{

}

void Model::SetSavePath(const QString& path)
{
    mSavePath = path;
}

const QString& Model::GetSavePath() const
{
    return mSavePath;
}

void Model::SetChoosePath(const QString& path)
{
    mChoosePath = path;
}

const QString& Model::GetChoosePath() const
{
    return mChoosePath;
}

void Model::SetKey(unsigned char* key, size_t keySize)
{
    mKey = std::unique_ptr<unsigned char[]>(new unsigned char[keySize]);  // 分配内存
    std::copy(key, key + keySize, mKey.get());
    mKeySize = keySize;
}

std::unique_ptr<unsigned char[]>& Model::GetKey()
{
    return mKey;
}

const size_t& Model::GetKeySize() const
{
    return mKeySize;
}