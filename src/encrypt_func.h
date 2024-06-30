#include <qobject.h>


class Encrypt
{
public:
    Encrypt() = default;
    ~Encrypt() = default;

    static std::unique_ptr<unsigned char[]>& GenerateAESKey(int key_size);
};