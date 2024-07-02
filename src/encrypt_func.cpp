#include "encrypt_func.h"
#include "model.h"
#include <array>
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
#include <openssl/evp.h>
#include <openssl/aes.h>
#include <openssl/des.h>
#include <openssl/ess.h>
#include <openssl/rsa.h>
#include <openssl/pem.h>
#include <openssl/err.h>
#include <vector>

std::vector<unsigned char> encryptChunk(const unsigned char* chunk, int chunkSize, RSA* rsaPubKey) 
{
    std::vector<unsigned char> encryptedChunk(RSA_size(rsaPubKey));
    int result = RSA_public_encrypt(chunkSize, chunk, encryptedChunk.data(), rsaPubKey, RSA_PKCS1_OAEP_PADDING);
    if (result == -1) {
        return std::vector<unsigned char>();
    }
    encryptedChunk.resize(result);
    return encryptedChunk;
}

bool Encrypt::EncryptByRSAPubKey(const QString& filePath, const QString& encryptPath, const QString& pubKeyPath)
{
    // 读取公钥
    FILE* pubKeyFile = fopen(pubKeyPath.toStdString().c_str(), "rb");
    if (!pubKeyFile) {
        return false;
    }
    RSA* rsaPubKey = PEM_read_RSA_PUBKEY(pubKeyFile, nullptr, nullptr, nullptr);
    fclose(pubKeyFile);

    if (!rsaPubKey) {
        return false;
    }

    // 读取文件数据
    std::ifstream file(filePath.toStdString(), std::ios::binary);
    if (!file) {
        return false;
    }
    std::vector<unsigned char> data((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

    int rsaSize = RSA_size(rsaPubKey);
    int maxDataSize = rsaSize - 42; // 最大数据块大小

    std::vector<unsigned char> finalData;
    finalData.reserve(data.size() + (data.size() / maxDataSize + 1) * rsaSize);

    // 获取后缀名
    std::string file_extension = std::filesystem::path(filePath.toStdString()).extension().string();
    if (file_extension.length() > 8) {
        std::cerr << "File extension too long" << std::endl;
        RSA_free(rsaPubKey);
        return false;
    }
    std::string padded_extension = file_extension;
    padded_extension.resize(8, '\0'); // 填充或截断为8字节

    // 添加后缀信息
    finalData.insert(finalData.end(), padded_extension.begin(), padded_extension.end());

    // 分块加密数据
    for (size_t i = 0; i < data.size(); i += maxDataSize) {
        size_t chunkSize = std::min(maxDataSize, static_cast<int>(data.size() - i));
        std::vector<unsigned char> encryptedChunk = encryptChunk(data.data() + i, chunkSize, rsaPubKey);
        if (encryptedChunk.empty())
        {
            return false;
        }
        finalData.insert(finalData.end(), encryptedChunk.begin(), encryptedChunk.end());
    }

    RSA_free(rsaPubKey);

    // 写入加密数据
    std::ofstream ofs(encryptPath.toStdString(), std::ios::binary);
    if (!ofs.is_open()) {
        return false;
    }
    ofs.write(reinterpret_cast<const char*>(finalData.data()), finalData.size());
    ofs.close();

    return true;
}


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
    std::ifstream ifs(filePath.toStdString(), std::ios::binary);
    std::ofstream ofs(encryptPath.toStdString(), std::ios::binary);

    // 将文件后缀名写入文件末尾（固定8字节）
    std::string file_extension = std::filesystem::path(filePath.toStdString()).extension().string();
    if (file_extension.length() > 8) {
        std::cerr << "File extension too long" << std::endl;
        return false;
    }
    std::string padded_extension = file_extension;
    padded_extension.resize(8, '\0'); // 填充或截断为8字节
    ofs.write(padded_extension.c_str(), 8);

    unsigned char iv[16];
    RAND_bytes(iv, sizeof(iv));

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

    // 清理
    EVP_CIPHER_CTX_free(ctx);
    ifs.close();
    ofs.close();

    return true;
}

bool Encrypt::EncryptByDESKey(const QString& filePath, const QString& encryptPath, const unsigned char *key)
{
    std::ifstream ifs(filePath.toStdString(), std::ios::binary);
    std::ofstream ofs(encryptPath.toStdString(), std::ios::binary);

    if (!ifs.is_open() || !ofs.is_open()) {
        std::cerr << "Error opening file" << std::endl;
        return false;
    }

    std::string file_extension = std::filesystem::path(filePath.toStdString()).extension().string();
    if (file_extension.length() > 8) {
        std::cerr << "File extension too long" << std::endl;
        return false;
    }
    std::string padded_extension = file_extension;
    padded_extension.resize(8, '\0'); // 填充或截断为8字节

    unsigned char iv[8]; // DES-64 IV大小为8字节
    RAND_bytes(iv, sizeof(iv));

    // 将文件扩展名写在文件的开头
    ofs.write(padded_extension.c_str(), 8);

    // 将IV写在文件的开头
    ofs.write(reinterpret_cast<const char*>(iv), sizeof(iv));

    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
    EVP_EncryptInit_ex(ctx, EVP_des_cbc(), NULL, key, iv);

    const int buffer_size = 4096;
    std::vector<char> buffer(buffer_size);
    std::vector<char> cipher_buffer(buffer_size + 8);
    int out_len;

    while (ifs.read(buffer.data(), buffer.size())) {
        if (EVP_EncryptUpdate(ctx, reinterpret_cast<unsigned char*>(cipher_buffer.data()), &out_len, reinterpret_cast<const unsigned char*>(buffer.data()), ifs.gcount()) != 1) {
            std::cerr << "Error during encryption" << std::endl;
            EVP_CIPHER_CTX_free(ctx);
            return false;
        }
        ofs.write(cipher_buffer.data(), out_len);
    }

    // 处理最后一块数据
    if (ifs.gcount() > 0) {
        if (EVP_EncryptUpdate(ctx, reinterpret_cast<unsigned char*>(cipher_buffer.data()), &out_len, reinterpret_cast<const unsigned char*>(buffer.data()), ifs.gcount()) != 1) {
            std::cerr << "Error during encryption" << std::endl;
            EVP_CIPHER_CTX_free(ctx);
            return false;
        }
        ofs.write(cipher_buffer.data(), out_len);
    }

    // 完成加密过程
    if (EVP_EncryptFinal_ex(ctx, reinterpret_cast<unsigned char*>(cipher_buffer.data()), &out_len) != 1) {
        std::cerr << "Error finalizing encryption" << std::endl;
        EVP_CIPHER_CTX_free(ctx);
        return false;
    }

    ofs.write(cipher_buffer.data(), out_len);

    // 清理
    EVP_CIPHER_CTX_free(ctx);
    ifs.close();
    ofs.close();

    return true;
}