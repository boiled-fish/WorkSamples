#include <NTL/ZZX.h>
#pragma once
using namespace std;
using namespace NTL;

//tools
void conver_zz(ZZ a, int len, int& b);
void generate_prime(ZZ& p, ZZ& q, int bit_len, unsigned long long seed, unsigned long long key1, unsigned long long key2);
void generate_b(ZZ& b, ZZ f_n);
void input_path(char* in_path, char* ciphertext_path);
ZZ exp_euclidean(ZZ b, ZZ n);
//RSA
ZZ RSA_cipher(ZZ x, ZZ b, ZZ n);
ZZ RSA_decipher(ZZ y, ZZ a, ZZ n);
//素性检测
long prime_test(const ZZ& n, long t);
//随机数产生器
ZZ generate_random_num(unsigned long long s, int num, unsigned long long k1, unsigned long long k2);
unsigned long long DES_cipher(unsigned long long m, unsigned long long key);
unsigned long long DES_decipher(unsigned long long code, unsigned long long key);
//AES
ZZ AES_cipher(ZZ m, ZZ k);
ZZ AES_decipher(ZZ cipher, ZZ k);