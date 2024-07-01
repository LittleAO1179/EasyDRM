#pragma once
#include <json/json.h>
#include <QString>

class Licence
{
public:

    Licence() = default;
    ~Licence() = default;

    static bool CreateLicence(QString filePath);

};