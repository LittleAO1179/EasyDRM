#include "licence_func.h"
#include "appdef.h"
#include "licence_model.h"
#include "model/licence_model.h"
#include <algorithm>
#include <fstream>
#include <json/value.h>

bool Licence::CreateLicence(QString filePath)
{
    Json::Value root;
    root["username"] = LicenceModel::getInstance().GetUserName().toStdString();

    Json::Value permissions;
    auto permissionsVector = LicenceModel::getInstance().GetPermissions();
    permissions["read"] = std::find(permissionsVector.begin(), permissionsVector.end(), AppDef::Permission::read) != permissionsVector.end();
    permissions["write"] = std::find(permissionsVector.begin(), permissionsVector.end(), AppDef::Permission::write) != permissionsVector.end();
    permissions["execute"] = std::find(permissionsVector.begin(), permissionsVector.end(), AppDef::Permission::execute) != permissionsVector.end();
    root["permissions"] = permissions;

    std::ofstream ofs(filePath.toStdString());
    if (!ofs.is_open()) 
    {
        return false;
    }
    Json::StreamWriterBuilder writer;
    std::unique_ptr<Json::StreamWriter> jsonWriter(writer.newStreamWriter());
    jsonWriter->write(root, &ofs);
    ofs.close();
    return true;
}