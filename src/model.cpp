#include "model.h"

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