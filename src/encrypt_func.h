#include <qcontainerfwd.h>
#include <qobject.h>


class Encrypt
{
public:
    Encrypt() = default;
    ~Encrypt() = default;

    static std::unique_ptr<unsigned char[]>& GenerateAESKey(int key_size);
    static std::unique_ptr<unsigned char[]>& GenerateDESKey();
    static bool GenerateRSAKey(const QString& privateKeyPath, const QString& publicKeyPath);

    static bool EncryptByAESKey(const QString& filePath, const QString& encryptPath, const unsigned char *key);
    static bool EncryptByDESKey(const QString& filePath, const QString& encryptPath, const unsigned char *key);

    static bool WriteKeyToTxtFile(const QString& string, const QString& path);
};