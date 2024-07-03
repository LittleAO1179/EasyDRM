#include <qobject.h>

class Decrypt
{
public:
    Decrypt() = default;
    ~Decrypt() = default;

    static bool DecryptByAESKey(const QString& encryptedFilePath, const QString& decryptPath, std::vector<unsigned char> key);
    //static bool DecryptByRC4Key(const QString& encryptedFilePath, const QString& decryptPath, std::vector<unsigned char> *key);
    static bool DecryptByRESPrivKey(const QString& encryptedFilePath, const QString& decryptPath, const QString& privateKeyPath);

    static QString GetFileExtension(const QString& encryptedFilePath);
};