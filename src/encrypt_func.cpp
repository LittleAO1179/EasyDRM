#include "encrypt_func.h"
#include "model.h"
#include <cstdlib>
#include <fstream>
#include <ios>
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
#include <openssl/evp.h>
#include <openssl/aes.h>
#include <openssl/des.h>
#include <vector>

bool Encrypt::WriteKeyToTxtFile(const QString& string, const QString& path)
{
    if (string.isEmpty())
    {
        return false;
    }
    std::ofstream ofs(path.toStdString());
    ofs << string.toStdString();

    ofs.close();
    return true;
}

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

bool Encrypt::EncryptByAESKey(const QString& filePath, const QString& encryptPath, const unsigned char *key)
{
    unsigned char iv[16];
    RAND_bytes(iv, sizeof(iv));

    std::ifstream ifs(filePath.toStdString(), std::ios::binary);
    std::ofstream ofs(encryptPath.toStdString(), std::ios::binary);

    // 将IV写在文件的开头
    ofs.write(reinterpret_cast<char*>(iv), sizeof(iv));

    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
    EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key, iv);

    const int buffer_size = 4096;
    std::vector<unsigned char> buffer(buffer_size);
    std::vector<unsigned char> cipher_buffer(buffer_size + AES_BLOCK_SIZE);
    int out_len;

    while (ifs.read(reinterpret_cast<char*>(buffer.data()), buffer.size())) {
        if (EVP_EncryptUpdate(ctx, cipher_buffer.data(), &out_len, buffer.data(), ifs.gcount()) != 1) {
            std::cerr << "Error during encryption" << std::endl;
            EVP_CIPHER_CTX_free(ctx);
            return false;
        }
        ofs.write(reinterpret_cast<char*>(cipher_buffer.data()), out_len);
    }

    // 处理最后一块数据
    if (ifs.gcount() > 0) {
        if (EVP_EncryptUpdate(ctx, cipher_buffer.data(), &out_len, buffer.data(), ifs.gcount()) != 1) {
            std::cerr << "Error during encryption" << std::endl;
            EVP_CIPHER_CTX_free(ctx);
            return false;
        }
        ofs.write(reinterpret_cast<char*>(cipher_buffer.data()), out_len);
    }

    // 完成加密过程
    if (EVP_EncryptFinal_ex(ctx, cipher_buffer.data(), &out_len) != 1) {
        std::cerr << "Error finalizing encryption" << std::endl;
        EVP_CIPHER_CTX_free(ctx);
        return false;
    }

    ofs.write(reinterpret_cast<char*>(cipher_buffer.data()), out_len);

    // 将文件后缀名写入文件末尾（固定8字节）
    std::string file_extension = std::filesystem::path(filePath.toStdString()).extension().string();
    if (file_extension.length() > 8) {
        std::cerr << "File extension too long" << std::endl;
        return false;
    }
    std::string padded_extension = file_extension;
    padded_extension.resize(8, '\0'); // 填充或截断为8字节
    ofs.write(padded_extension.c_str(), 8);

    // 清理
    EVP_CIPHER_CTX_free(ctx);
    ifs.close();
    ofs.close();

    return true;
}

bool Encrypt::EncryptByDESKey(const QString& filePath, const QString& encryptPath, const unsigned char *key)
{
    unsigned char iv[8]; // DES-64 IV大小为8字节
    RAND_bytes(iv, sizeof(iv));

    std::ifstream ifs(filePath.toStdString(), std::ios::binary);
    std::ofstream ofs(encryptPath.toStdString(), std::ios::binary);

    // 将IV写在文件的开头
    ofs.write(reinterpret_cast<char*>(iv), sizeof(iv));

    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
    EVP_EncryptInit_ex(ctx, EVP_des_cbc(), NULL, key, iv);

    const int buffer_size = 4096;
    std::vector<unsigned char> buffer(buffer_size);
    std::vector<unsigned char> cipher_buffer(buffer_size + 8);
    int out_len;

    while (ifs.read(reinterpret_cast<char*>(buffer.data()), buffer.size())) {
        if (EVP_EncryptUpdate(ctx, cipher_buffer.data(), &out_len, buffer.data(), ifs.gcount()) != 1) {
            std::cerr << "Error during encryption" << std::endl;
            EVP_CIPHER_CTX_free(ctx);
            return false;
        }
        ofs.write(reinterpret_cast<char*>(cipher_buffer.data()), out_len);
    }

    // 处理最后一块数据
    if (ifs.gcount() > 0) {
        if (EVP_EncryptUpdate(ctx, cipher_buffer.data(), &out_len, buffer.data(), ifs.gcount()) != 1) {
            std::cerr << "Error during encryption" << std::endl;
            EVP_CIPHER_CTX_free(ctx);
            return false;
        }
        ofs.write(reinterpret_cast<char*>(cipher_buffer.data()), out_len);
    }

    // 完成加密过程
    if (EVP_EncryptFinal_ex(ctx, cipher_buffer.data(), &out_len) != 1) {
        std::cerr << "Error finalizing encryption" << std::endl;
        EVP_CIPHER_CTX_free(ctx);
        return false;
    }

    ofs.write(reinterpret_cast<char*>(cipher_buffer.data()), out_len);

    // 将文件后缀名写入文件末尾（固定8字节）
    std::string file_extension = std::filesystem::path(filePath.toStdString()).extension().string();
    if (file_extension.length() > 8) {
        std::cerr << "File extension too long" << std::endl;
        return false;
    }
    std::string padded_extension = file_extension;
    padded_extension.resize(8, '\0'); // 填充或截断为8字节
    ofs.write(padded_extension.c_str(), 8);

    // 清理
    EVP_CIPHER_CTX_free(ctx);
    ifs.close();
    ofs.close();

    return true;
}