#include "encrypt_func.h"
#include "model.h"
#include <cstdlib>
#include <iostream>
#include <string>
#include <cstddef>
#include <openssl/asn1.h>
#include <openssl/bio.h>
#include <openssl/cryptoerr_legacy.h>
#include <qobject.h>
#include <openssl/rand.h>
#include <openssl/des.h>
#include <openssl/rsa.h>

std::unique_ptr<unsigned char[]>& Encrypt::GenerateAESKey(const int keySize)
{
    unsigned char key[keySize]; 
    if (RAND_bytes(key, keySize) != 1) 
    {

    }

    Model::getInstance().SetKey(key, keySize);
    return Model::getInstance().GetKey();
}

std::unique_ptr<unsigned char[]>& Encrypt::GenerateDESKey()
{
    const size_t keySize = 8;
    unsigned char key[keySize];

    if (RAND_bytes(key, keySize) != 1)
    {

    }

    // 奇偶校验修正
    DES_set_odd_parity(&key);

    Model::getInstance().SetKey(key, keySize);
    return Model::getInstance().GetKey();
}

bool Encrypt::GenerateRSAKey(const QString& privateKeyPath, const QString& publicKeyPath)
{
        // 生成私钥
    std::string generate_private_key_cmd = "openssl genpkey -algorithm RSA -out " + privateKeyPath.toStdString() + " -pkeyopt rsa_keygen_bits:1024";
    if (system(generate_private_key_cmd.c_str()) != 0) {
        std::cerr << "Error generating private key" << std::endl;
        return false;
    }

    // 从私钥生成公钥
    std::string generate_public_key_cmd = "openssl rsa -pubout -in " + privateKeyPath.toStdString() + " -out " + publicKeyPath.toStdString();
    if (system(generate_public_key_cmd.c_str()) != 0) {
        std::cerr << "Error generating public key" << std::endl;
        return false;
    }

    return true;
}