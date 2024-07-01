#include "licence_func.h"
#include "appdef.h"
#include "licence_model.h"
#include "model/licence_model.h"
#include <algorithm>
#include <fstream>
#include <iostream>
#include <json/reader.h>
#include <json/value.h>
#include <string>
#include <vector>

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

bool Licence::ImportLicence(QString filePath)
{
    Json::Value root;
    Json::CharReaderBuilder readerBuilder;
    readerBuilder["emitUTF8"] = true;
    std::ifstream ifs(filePath.toStdString());

    std::string error;
    if (!Json::parseFromStream(readerBuilder, ifs, &root, &error))
    {
        std::cout << error << std::endl;
        return false;
    }
    
    QString username = root["username"].asCString();
    LicenceModel::getInstance().SetUserName(username);

    std::vector<AppDef::Permission> permissionsVector;
    auto permissions = root["permissions"];
    if (permissions["read"].asBool()) permissionsVector.push_back(AppDef::Permission::read);
    if (permissions["write"].asBool()) permissionsVector.push_back(AppDef::Permission::write);
    if (permissions["execute"].asBool()) permissionsVector.push_back(AppDef::Permission::execute);
    LicenceModel::getInstance().SetPermissions(permissionsVector);

    return true;
}
