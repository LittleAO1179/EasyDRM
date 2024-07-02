#include "licence_func.h"
#include "appdef.h"
#include "licence_model.h"
#include "model/licence_model.h"
#include <algorithm>
#include <fstream>
#include <iostream>
#include <json/reader.h>
#include <json/value.h>
#include <qobject.h>
#include <Qfile>
#include <QCryptographicHash>
#include <qstringview.h>
#include <string>
#include <vector>
#include <openssl/sha.h>
#include <openssl/pem.h>
#include <openssl/x509.h>
#include <openssl/evp.h>
#include <openssl/rsa.h>
#include <openssl/bn.h>


bool Licence::CreateCertification(QString keyPath, QString filePath)
{
    // 读取现有的私钥
    FILE* pkeyFile = fopen(keyPath.toStdString().c_str(), "rb");
    if (!pkeyFile) {
        return false;
    }

    EVP_PKEY* pkey = PEM_read_PrivateKey(pkeyFile, nullptr, nullptr, nullptr);
    fclose(pkeyFile);

    if (!pkey) {
        return false;
    }

    // 创建 X509 证书
    X509* x509 = X509_new();
    X509_set_version(x509, 2);
    ASN1_INTEGER_set(X509_get_serialNumber(x509), 1);
    X509_gmtime_adj(X509_get_notBefore(x509), 0);
    X509_gmtime_adj(X509_get_notAfter(x509), 31536000L); // 1 year
    X509_set_pubkey(x509, pkey);

    // 设置证书的主题名
    X509_NAME* name = X509_get_subject_name(x509);
    X509_NAME_add_entry_by_txt(name, "C", MBSTRING_ASC, (unsigned char*)"CUC", -1, -1, 0);
    X509_NAME_add_entry_by_txt(name, "O", MBSTRING_ASC, (unsigned char*)"CUC", -1, -1, 0);
    X509_NAME_add_entry_by_txt(name, "CN", MBSTRING_ASC, (unsigned char*)"CUC", -1, -1, 0);
    X509_set_issuer_name(x509, name);

    // 用私钥签名证书
    X509_sign(x509, pkey, EVP_sha256());

    // 将证书写入文件
    FILE* certFile = fopen(filePath.toStdString().c_str(), "wb");
    if (!certFile) {
        EVP_PKEY_free(pkey);
        X509_free(x509);
        return false;
    }

    PEM_write_X509(certFile, x509);
    fclose(certFile);

    // 释放资源
    EVP_PKEY_free(pkey);
    X509_free(x509);

    return true;
}

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

const QString Licence::CalculateSHA256(QString filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        // 文件打开失败，返回空字符串或抛出异常
        return QString();
    }

    QCryptographicHash hash(QCryptographicHash::Sha256);
    if (!hash.addData(&file)) {
        // 添加数据失败，返回空字符串或抛出异常
        return QString();
    }

    return hash.result().toHex();
}
