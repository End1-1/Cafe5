#include "c5crypt.h"
#include "des.h"
#include <QSettings>
#include <QCryptographicHash>
#include <Windows.h>
#include <memory>
#include <cstdio>
#include <array>

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

void C5Crypt::ede3_cbc(QByteArray &in,
                       QByteArray &out,
                       const QString &key,
                       bool encrypt)
{
    if (in.isEmpty()) {
        out = "";
        return;
    }
    DES_key_schedule des_key1;
    DES_key_schedule des_key2;
    DES_key_schedule des_key3;

    const_DES_cblock key_SSL1, key_SSL2, key_SSL3;
    DES_cblock ivec;

    if (in.length() % 8) {
        quint8 remain = 8 - (in.length() % 8);
        quint8 jsonFillCount = remain;
        for (int i = 0; i < jsonFillCount; i++) {
            in.append((char)jsonFillCount);
        }
    }
    int in_data_len = in.length();

    QByteArray hash256 = QCryptographicHash::hash(key.toLatin1(), QCryptographicHash::Sha256);
    unsigned char *key_value = new unsigned char[24];
    memcpy(key_value, hash256.mid(0, 24).data(), 24);

   // The key as passed in is a 24 byte string containing 3 keys
   // pick it apart and create the key schedules
   memcpy(&key_SSL1, key_value, (size_t)8);
   memcpy(&key_SSL2, key_value+8, (size_t)8);
   memcpy(&key_SSL3, key_value+16, (size_t)8);
   DES_set_key_unchecked(&key_SSL1, &des_key1);
   DES_set_key_unchecked(&key_SSL2, &des_key2);
   DES_set_key_unchecked(&key_SSL3, &des_key3);

   unsigned char init_v[] = {0x01,0x02,0x03,0x04,0x05,0x06,0x02,0x01};
   memcpy(ivec, init_v, sizeof(ivec));

   // Encrypt or decrypt the data
   int *out_data_len = new int();
   unsigned char *out_data = new unsigned char[in_data_len];
   if (encrypt) {
       DES_ede3_cbc_encrypt((const unsigned char*)in.data(),
               out_data,
               in_data_len,
               &des_key1,
               &des_key2,
               &des_key3,
               &ivec,
               DES_ENCRYPT);
       *out_data_len = in_data_len;
   } else {
       DES_ede3_cbc_encrypt((const unsigned char*)in.data(),
               out_data,
               in_data_len,
               &des_key1,
               &des_key2,
               &des_key3,
               &ivec,
               DES_DECRYPT);

       *out_data_len = in_data_len;
   }

   out.append((char*)out_data, *out_data_len);
   if ((int) out[*out_data_len - 1] < (int) 0x08) {
       out.remove(out.length() - out[*out_data_len - 1], out[*out_data_len - 1]);
   }
   delete [] key_value;
   delete [] out_data;
   delete out_data_len;
}

const QString C5Crypt::driveKey()
{
    std::array<char, 128> buffer;
    std::string result;
    std::shared_ptr<FILE> pipe(_popen("wmic path win32_physicalmedia get SerialNumber", "r"), _pclose);
    if (!pipe)  {
        return "";
    }
    while (!feof(pipe.get())) {
        if (fgets(buffer.data(), 128, pipe.get()) != NULL) {
            result += buffer.data();
        }
    }
    QString key = QString::fromStdString(result).replace("\r\n", "").replace(" ", "");
    return key;
}

const QString C5Crypt::machineUuid()
{
    QSettings s("HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Cryptography", QSettings::Registry64Format);
    //QSettings s("HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Cryptography");
    return s.value("MachineGuid").toString();
}


