#pragma once
#include <json/json.h>
#include <QString>
#include <qcontainerfwd.h>

class Licence
{
public:

    Licence() = default;
    ~Licence() = default;

    static bool CreateLicence(QString filePath);
    static bool ImportLicence(QString filePath);
    static const QString CalculateSHA256(QString filePath);
    static bool CreateCertification(QString keyPath, QString filePath);

};