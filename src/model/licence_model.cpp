#include "licence_model.h"

void LicenceModel::SetPrivateKeyPath(const QString& filePath)
{
    mPrivateKeyPath = filePath;
}

const QString& LicenceModel::GetPrivateKeyPath() const
{
    return mPrivateKeyPath;
}

void LicenceModel::SetFilePath(const QString& filePath)
{
    mFilePath = filePath;
}

const QString& LicenceModel::GetFilePath() const
{
    return mFilePath;
}

void LicenceModel::SetUserName(const QString& username)
{
    mUsername = username;
}

const QString& LicenceModel::GetUserName() const
{
    return mUsername;
}

void LicenceModel::SetPermissions(const std::vector<AppDef::Permission>& permissions)
{
    mPermissions = permissions;
}

const std::vector<AppDef::Permission>& LicenceModel::GetPermissions() const
{
    return mPermissions;
}