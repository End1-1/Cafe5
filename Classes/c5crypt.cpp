#include "c5crypt.h"
#include "des.h"

C5Crypt::C5Crypt()
{

}

void C5Crypt::cryptData(const QByteArray &k, QByteArray &inData, QByteArray &outData)
{
    outData.clear();
    quint8 jsonFillCount = 8 - (inData.length() % 8);
    for (int i = 0; i < jsonFillCount; i++) {
        inData.append((char)jsonFillCount);
    }

    size_t key_len = 24;
    unsigned char key[24];
    unsigned char block_key[9];
    DES_key_schedule ks,ks2,ks3;

    memcpy(key, k.data(), key_len);
    memset(key + key_len, 0x00, 24 - key_len);

    memset(block_key, 0, sizeof(block_key));
    memcpy(block_key, key + 0, 8);
    DES_set_key_unchecked((const_DES_cblock*)block_key, &ks);
    memcpy(block_key, key + 8, 8);
    DES_set_key_unchecked((const_DES_cblock*)block_key, &ks2);
    memcpy(block_key, key + 16, 8);
    DES_set_key_unchecked((const_DES_cblock*)block_key, &ks3);

    qint16 srcLen = inData.length();
    char *out = new char[srcLen];
    memset(out, 0, srcLen);
    for (int i = 0; i < srcLen; i += 8) {
        DES_ecb3_encrypt((const_DES_cblock*)&inData.data()[i], (DES_cblock*)&out[i], &ks, &ks2, &ks3, DES_ENCRYPT);
    }
    outData.append(out, srcLen);
    delete [] out;
}

void C5Crypt::decryptData(const QByteArray &k, QByteArray &inData, QByteArray &outData)
{
    outData.clear();

    size_t key_len = 24;
    unsigned char key[24];
    unsigned char block_key[9];
    DES_key_schedule ks,ks2,ks3;

    memcpy(key, k.data(), key_len);
    memset(key + key_len, 0x00, 24 - key_len);

    memset(block_key, 0, sizeof(block_key));
    memcpy(block_key, key + 0, 8);
    DES_set_key_unchecked((const_DES_cblock*)block_key, &ks);
    memcpy(block_key, key + 8, 8);
    DES_set_key_unchecked((const_DES_cblock*)block_key, &ks2);
    memcpy(block_key, key + 16, 8);
    DES_set_key_unchecked((const_DES_cblock*)block_key, &ks3);

    char *out = new char[inData.length()];
    memset(out, 0, inData.length());
    for (int i = 0; i < inData.length(); i += 8) {
        DES_ecb3_encrypt((const_DES_cblock*)&inData.data()[i], (DES_cblock*)&out[i], &ks, &ks2, &ks3, DES_DECRYPT);
    }
    outData.append(out, inData.length());
    delete [] out;
}
