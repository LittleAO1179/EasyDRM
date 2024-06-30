#include "encrypt_func.h"
#include "model.h"
#include <qobject.h>
#include <openssl/rand.h>

std::unique_ptr<unsigned char[]>& Encrypt::GenerateAESKey(const int key_size)
{
    unsigned char key[key_size]; 
    if (RAND_bytes(key, key_size) != 1) {
    }

    Model::getInstance().SetKey(key, key_size);
    return Model::getInstance().GetKey();
}