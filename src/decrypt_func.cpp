#include "decrypt_func.h"
#include <iostream>
#include <ios>
#include <cstdlib>
#include <fstream>
#include <cstddef>
#include <openssl/aes.h>
#include <openssl/crypto.h>
#include <openssl/evp.h>
#include <openssl/pem.h>
#include <qobject.h>
#include <qstringview.h>
#include <vector>

std::vector<unsigned char> decryptChunk(const unsigned char* chunk, int chunkSize, RSA* rsaPrivKey) {
    std::vector<unsigned char> decryptedChunk(RSA_size(rsaPrivKey));
    int result = RSA_private_decrypt(chunkSize, chunk, decryptedChunk.data(), rsaPrivKey, RSA_PKCS1_OAEP_PADDING);
    if (result == -1) {
        return std::vector<unsigned char>();
    }
    decryptedChunk.resize(result);
    return decryptedChunk;
}

bool Decrypt::DecryptByRESPrivKey(const QString& encryptedFilePath, const QString& decryptPath, const QString& privateKeyPath)
{
    // 读取私钥
    FILE* privKeyFile = fopen(privateKeyPath.toStdString().c_str(), "rb");
    if (!privKeyFile) {
        return false;
    }
    RSA* rsaPrivKey = PEM_read_RSAPrivateKey(privKeyFile, nullptr, nullptr, nullptr);
    fclose(privKeyFile);

    if (!rsaPrivKey) {
        return false;
    }

    // 读取加密数据
    std::ifstream file(encryptedFilePath.toStdString(), std::ios::binary);
    if (!file) {
        return false;
    }
    std::vector<unsigned char> data((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    
    // 删除前8字节的后缀名数据
    if (data.size() < 8) {
        RSA_free(rsaPrivKey);
        return false;
    }
    data.erase(data.begin(), data.begin() + 8);

    int rsaSize = RSA_size(rsaPrivKey);

    std::vector<unsigned char> decryptedData;
    decryptedData.reserve(data.size()); // 预留空间

    // 分块解密数据
    for (size_t i = 0; i < data.size(); i += rsaSize) {
        size_t chunkSize = std::min(rsaSize, static_cast<int>(data.size() - i));
        std::vector<unsigned char> decryptedChunk = decryptChunk(data.data() + i, chunkSize, rsaPrivKey);
        if (decryptedChunk.empty())
        {
            return false;
        }
        decryptedData.insert(decryptedData.end(), decryptedChunk.begin(), decryptedChunk.end());
    }

    RSA_free(rsaPrivKey);

    // 将解密的数据保存
    std::ofstream ofs(decryptPath.toStdString(), std::ios::binary);
    if (!ofs.is_open()) {
        return false;
    }
    ofs.write(reinterpret_cast<const char*>(decryptedData.data()), decryptedData.size());
    ofs.close();

    return true;
}


QString Decrypt::GetFileExtension(const QString& encryptedFilePath)
{
    std::ifstream ifs(encryptedFilePath.toStdString(), std::ios::binary);
    // 读取文件后缀名（固定8字节）
    char file_extension[8];
    ifs.read(file_extension, 8);
    if (ifs.gcount() != 8) {
        std::cerr << "Error reading file extension" << std::endl;
        return "ALL File *.*";
    }

    ifs.close();
    return QString(file_extension);
}

bool Decrypt::DecryptByDESKey(const QString& encryptPath, const QString& decryptPath, const unsigned char *key)
{
    std::ifstream ifs(encryptPath.toStdString(), std::ios::binary);
    std::ofstream ofs(decryptPath.toStdString(), std::ios::binary);

    if (!ifs.is_open() || !ofs.is_open()) {
        std::cerr << "Error opening file" << std::endl;
        return false;
    }

    std::string file_extension(8, '\0');
    unsigned char iv[8];

    // 读取文件扩展名和IV
    ifs.read(&file_extension[0], 8);
    ifs.read(reinterpret_cast<char*>(iv), sizeof(iv));

    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
    EVP_DecryptInit_ex(ctx, EVP_des_cbc(), NULL, key, iv);

    const int buffer_size = 4096;
    std::vector<char> buffer(buffer_size);
    std::vector<char> plain_buffer(buffer_size);
    int out_len;

    while (ifs.read(buffer.data(), buffer.size())) {
        if (EVP_DecryptUpdate(ctx, reinterpret_cast<unsigned char*>(plain_buffer.data()), &out_len, reinterpret_cast<const unsigned char*>(buffer.data()), ifs.gcount()) != 1) {
            std::cerr << "Error during decryption" << std::endl;
            EVP_CIPHER_CTX_free(ctx);
            return false;
        }
        ofs.write(plain_buffer.data(), out_len);
    }

    // 处理最后一块数据
    if (ifs.gcount() > 0) {
        if (EVP_DecryptUpdate(ctx, reinterpret_cast<unsigned char*>(plain_buffer.data()), &out_len, reinterpret_cast<const unsigned char*>(buffer.data()), ifs.gcount()) != 1) {
            std::cerr << "Error during decryption" << std::endl;
            EVP_CIPHER_CTX_free(ctx);
            return false;
        }
        ofs.write(plain_buffer.data(), out_len);
    }

    // 完成解密过程
    if (EVP_DecryptFinal_ex(ctx, reinterpret_cast<unsigned char*>(plain_buffer.data()), &out_len) != 1) {
        std::cerr << "Error finalizing decryption" << std::endl;
        EVP_CIPHER_CTX_free(ctx);
        return false;
    }

    ofs.write(plain_buffer.data(), out_len);

    // 清理
    EVP_CIPHER_CTX_free(ctx);
    ifs.close();
    ofs.close();

    return true;
}

bool Decrypt::DecryptByAESKey(const QString& encryptedFilePath, const QString& decryptPath, const unsigned char *key)
{
    unsigned char iv[16];

    std::ifstream ifs(encryptedFilePath.toStdString(), std::ios::binary);
    if (!ifs.is_open()) {
        std::cerr << "Could not open encrypted file" << std::endl;
        return false;
    }

    // 读取文件后缀名（固定8字节）
    char file_extension[8];
    ifs.read(file_extension, 8);
    if (ifs.gcount() != 8) {
        std::cerr << "Error reading file extension" << std::endl;
        return false;
    }

    // 读取IV
    ifs.read(reinterpret_cast<char*>(iv), sizeof(iv));
    if (ifs.gcount() != sizeof(iv)) {
        std::cerr << "Error reading IV" << std::endl;
        return false;
    }

    // 获取文件大小
    ifs.seekg(0, std::ios::end);
    std::streamoff fileSize = ifs.tellg();
    ifs.seekg(sizeof(iv) + 8, std::ios::beg); // 跳过IV和后缀

    // 计算实际加密内容的大小
    std::streamoff encryptedContentSize = fileSize - static_cast<std::streamoff>(sizeof(iv) + 8);

    // 创建输出文件
    std::ofstream ofs(decryptPath.toStdString(), std::ios::binary);
    if (!ofs.is_open()) {
        std::cerr << "Could not open output file" << std::endl;
        return false;
    }

    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
    EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key, iv);

    const int buffer_size = 4096;
    std::vector<unsigned char> buffer(buffer_size);
    std::vector<unsigned char> plain_buffer(buffer_size + AES_BLOCK_SIZE);
    int out_len;

    std::streamoff bytesRead = 0;
    while (bytesRead < encryptedContentSize && ifs.read(reinterpret_cast<char*>(buffer.data()), buffer.size())) {
        std::streamsize readSize = ifs.gcount();
        bytesRead += readSize;

        if (EVP_DecryptUpdate(ctx, plain_buffer.data(), &out_len, buffer.data(), readSize) != 1) {
            std::cerr << "Error during decryption" << std::endl;
            EVP_CIPHER_CTX_free(ctx);
            return false;
        }
        ofs.write(reinterpret_cast<char*>(plain_buffer.data()), out_len);
    }

    // 处理最后一块数据
    if (bytesRead < encryptedContentSize) {
        std::streamsize remaining = encryptedContentSize - bytesRead;
        ifs.read(reinterpret_cast<char*>(buffer.data()), remaining);
        if (EVP_DecryptUpdate(ctx, plain_buffer.data(), &out_len, buffer.data(), remaining) != 1) {
            std::cerr << "Error during decryption" << std::endl;
            EVP_CIPHER_CTX_free(ctx);
            return false;
        }
        ofs.write(reinterpret_cast<char*>(plain_buffer.data()), out_len);
    }

    // 完成解密过程
    if (EVP_DecryptFinal_ex(ctx, plain_buffer.data(), &out_len) != 1) {
        std::cerr << "Error finalizing decryption" << std::endl;
        EVP_CIPHER_CTX_free(ctx);
        return false;
    }

    ofs.write(reinterpret_cast<char*>(plain_buffer.data()), out_len);

    // 清理
    EVP_CIPHER_CTX_free(ctx);
    ifs.close();
    ofs.close();

    return true;
}