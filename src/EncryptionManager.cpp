#include "../include/EncryptionManager.h"
#include <openssl/evp.h>
#include <openssl/rand.h>
#include <openssl/err.h>
#include <stdexcept>
#include <iostream>
#include <cstring>

EncryptionManager::EncryptionManager(const std::string &key) {
    if (key.size() != 32) {
        throw std::runtime_error("Key size must be 32 bytes");
    }
    memcpy(this->key, key.data(), 32);
}

EncryptionManager::~EncryptionManager() {}

std::vector<unsigned char> EncryptionManager::generateIV() {
    std::vector<unsigned char> iv(16);
    if (!RAND_bytes(iv.data(), iv.size())) {
        handleErrors();
    }
    return iv;
}

std::vector<unsigned char> EncryptionManager::encrypt(const std::vector<unsigned char> &data) {
    if (data.empty()) {
        throw std::runtime_error("Cannot encrypt an empty buffer");
    }

    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
    if (!ctx) handleErrors();

    std::vector<unsigned char> iv = generateIV();

    if (EVP_EncryptInit_ex(ctx, EVP_aes_256_ctr(), NULL, key, iv.data()) != 1)
        handleErrors();

    std::vector<unsigned char> ciphertext(data.size() + 16);
    int len;
    int ciphertext_len = 0;

    if (EVP_EncryptUpdate(ctx, ciphertext.data(), &len, data.data(), data.size()) != 1)
        handleErrors();

    ciphertext_len += len;

    if (EVP_EncryptFinal_ex(ctx, ciphertext.data() + ciphertext_len, &len) != 1)
        handleErrors();

    ciphertext_len += len;
    ciphertext.resize(ciphertext_len);

    EVP_CIPHER_CTX_free(ctx);

    // Prepend IV to ciphertext
    std::vector<unsigned char> output(iv.begin(), iv.end());
    output.insert(output.end(), ciphertext.begin(), ciphertext.end());

    return output;
}

std::vector<unsigned char> EncryptionManager::decrypt(const std::vector<unsigned char> &data) {
    if (data.size() < 16) {
        throw std::runtime_error("Invalid encrypted data");
    }

    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
    if (!ctx) handleErrors();

    // Extract IV from the start of the data
    std::vector<unsigned char> iv(data.begin(), data.begin() + 16);
    std::vector<unsigned char> encryptedData(data.begin() + 16, data.end());

    // Initialize CTR mode
    if (EVP_DecryptInit_ex(ctx, EVP_aes_256_ctr(), NULL, key, iv.data()) != 1)
        handleErrors();

    std::vector<unsigned char> plaintext(encryptedData.size());
    int len;
    int plaintext_len = 0;

    if (EVP_DecryptUpdate(ctx, plaintext.data(), &len, encryptedData.data(), encryptedData.size()) != 1)
        handleErrors();

    plaintext_len += len;

    if (EVP_DecryptFinal_ex(ctx, plaintext.data() + plaintext_len, &len) != 1)
        handleErrors();

    plaintext_len += len;
    plaintext.resize(plaintext_len);

    EVP_CIPHER_CTX_free(ctx);

    return plaintext;
}

void EncryptionManager::handleErrors() {
    ERR_print_errors_fp(stderr);
    throw std::runtime_error("Encryption/Decryption error");
}
