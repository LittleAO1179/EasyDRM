#pragma once

#include <QString>
#include <qcontainerfwd.h>
#include <appdef.h>

class LicenceModel
{
public:
    

    LicenceModel() = default;
    // 单例模式
    LicenceModel(const LicenceModel&) = delete;
    LicenceModel& operator=(const LicenceModel&) = delete;

    // 全局访问点
    static LicenceModel& getInstance()
    {
        static LicenceModel instance;
        return instance;
    }

    void SetUserName(const QString& username);
    const QString& GetUserName() const;
    void SetPermissions(const std::vector<AppDef::Permission>& permissions);
    const std::vector<AppDef::Permission>& GetPermissions() const;
    void SetFilePath(const QString& filePath);
    const QString& GetFilePath() const;
    void SetPrivateKeyPath(const QString& filePath);
    const QString& GetPrivateKeyPath() const;

private:
    QString mUsername;
    QString mGetLicencePath;
    QString mSaveLicencePath;
    QString mFilePath;
    QString mPrivateKeyPath;
    QString mSaveCertificationPath;
    QString mSaveSignaturePath;
    QString mGetSignaturePath;
    std::vector<AppDef::Permission> mPermissions;
};