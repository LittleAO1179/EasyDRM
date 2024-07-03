#include <qcontainerfwd.h>
#include <qobject.h>
#include <vector>


class Encrypt
{
public:
    Encrypt() = default;
    ~Encrypt() = default;

    static std::vector<unsigned char> GenerateAESKey(int key_size);
    static bool GenerateRSAKey(const QString& privateKeyPath, const QString& publicKeyPath);

    static bool EncryptByAESKey(const QString& filePath, const QString& encryptPath, std::vector<unsigned char> key);
    //static bool EncryptByAES512Key(const QString& filePath, const QString& encryptPath, std::vector<unsigned char> key);
    static bool EncryptByRSAPubKey(const QString& filePath, const QString& encryptPath, const QString& pubKeyPath);

    static bool WriteKeyToTxtFile(const std::vector<unsigned char>& string, const QString& path);
};