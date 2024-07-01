#include <qobject.h>

class Decrypt
{
public:
    Decrypt() = default;
    ~Decrypt() = default;

    static bool DecryptByAESKey(const QString& encryptedFilePath, const QString& decryptPath, const unsigned char *key);
    static bool DecryptByDESKey(const QString& encryptedFilePath, const QString& decryptPath, const unsigned char *key);

    static QString GetFileExtension(const QString& encryptedFilePath);
};