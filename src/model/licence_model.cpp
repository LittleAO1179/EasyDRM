#include "licence_model.h"

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