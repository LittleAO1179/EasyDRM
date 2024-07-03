#include "model.h"
#include <algorithm>
#include <cstddef>
#include <qobject.h>

Model::Model():mFileExtension("ALL File *.*")
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
    mKey.clear();
    mKey.resize(keySize);
    std::memcpy(mKey.data(), key, keySize);
    mKeySize = keySize;
}

std::vector<unsigned char>& Model::GetKey()
{
    return mKey;
}

const size_t& Model::GetKeySize() const
{
    return mKeySize;
}

void Model::SetFileExtension(const QString& extension)
{
    mFileExtension = extension;
}

const QString& Model::GetFileExtension() const
{
    return mFileExtension;
}

void Model::clear()
{
    mSavePath.clear();
    mChoosePath.clear();
    mKey.clear();
    mKeySize = 0;
    mFileExtension = "ALL File *.*";
}