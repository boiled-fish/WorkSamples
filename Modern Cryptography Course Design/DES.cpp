#include <NTL/ZZX.h>
#include<vector>
#include "course_design.h"

using namespace std;
using namespace NTL;

class DES {
private:
	/*const int key[64] = {
		0,0,0,0, 0,0,0,1, 0,0,1,0, 0,0,1,1,
		0,1,0,0, 0,1,0,1, 0,1,1,0, 0,1,1,1,
		1,0,0,0, 1,0,0,1, 1,0,1,0, 1,0,1,1,
		1,1,0,0, 1,1,0,1, 1,1,1,0, 1,1,1,1
	};*/
	//������Ź���
	const int enckey_order[16] = {
		1, 1, 2, 2 ,2, 2, 2, 2, 1, 2, 2, 2, 2, 2, 2, 1
	};
	//��ʼIP�û�
	const int IP[64] = {
		58, 50, 42, 34, 26, 18, 10, 2,
		60, 52, 44, 36, 28, 20, 12, 4,
		62, 54, 46, 38, 30, 22, 14, 6,
		64, 56, 48, 40, 32, 24, 16, 8,
		57, 49, 41, 33, 25, 17,  9, 1,
		59, 51, 43, 35, 27, 19, 11, 3,
		61, 53, 45, 37, 29, 21, 13, 5,
		63, 55, 47, 39, 31, 23, 15, 7
	};
	//IP�û������û�
	const int inverse_IP[64] = {
		40, 8, 48, 16, 56, 24, 64, 32,
		39, 7, 47, 15, 55, 23, 63, 31,
		38, 6, 46, 14, 54, 22, 62, 30,
		37, 5, 45, 13, 53, 21, 61, 29,
		36, 4, 44, 12, 52, 20, 60, 28,
		35, 3, 43, 11, 51, 19, 59, 27,
		34, 2, 42, 10, 50, 18, 58, 26,
		33, 1, 41,  9, 49, 17, 57, 25
	};//
	//�����û�
	const int key_IP[56] = {
		57, 49, 41, 33, 25, 17,  9,
		 1, 58, 50, 42, 34, 26, 18,
		10,  2, 59, 51, 43, 35, 27,
		19, 11,  3, 60, 52, 44, 36,
		//above for Ci, below for Di
		63, 55, 47, 39, 31, 23, 15,
		 7, 62, 54, 46, 38, 30, 22,
		14,  6, 61, 53, 45, 37, 29,
		21, 13,  5, 28, 20, 12,  4
	};
	//��չ�û�
	const int ep[48] = {
		32,  1,  2,  3,  4,  5,
		 4,  5,  6,  7,  8,  9,
		 8,  9, 10, 11, 12, 13,
		12, 13, 14, 15, 16, 17,
		16, 17, 18, 19, 20, 21,
		20, 21, 22, 23, 24, 25,
		24, 25, 26, 27, 28, 29,
		28, 29, 30, 31, 32,  1
	};
	//ѹ���û�
	const int cp[48] = {
		14, 17, 11, 24,  1,  5,
		 3, 28, 15,  6, 21, 10,
		23, 19, 12,  4, 26,  8,
		16,  7, 27, 20, 13,  2,
		41, 52, 31, 37, 47, 55,
		30, 40, 51, 45, 33, 48,
		44, 49, 39, 56, 34, 53,
		46, 42, 50, 36, 29, 32
	};
	//S��
	const int s_box[32][16] = {
		14,  4, 13,  1,  2, 15, 11,  8,  3, 10,  6, 12,  5,  9,  0,  7, //S1
		 0, 15,  7,  4, 14,  2, 13,  1, 10,  6, 12, 11,  9,  5,  3,  8,
		 4,  1, 14,  8, 13,  6,  2, 11, 15, 12,  9,  7,  3, 10,  5,  0,
		15, 12,  8,  2,  4,  9,  1,  7,  5, 11,  3, 14, 10,  0,  6, 13,

		15,  1,  8, 14,  6, 11,  3,  4,  9,  7,  2, 13, 12,  0,  5, 10, //S2
		 3, 13,  4,  7, 15,  2,  8, 14, 12,  0,  1, 10,  6,  9, 11,  5,
		 0, 14,  7, 11, 10,  4, 13,  1,  5,  8, 12,  6,  9,  3,  2, 15,
		13,  8, 10,  1,  3, 15,  4,  2, 11,  6,  7, 12,  0,  5, 14,  9,

		10,  0,  9, 14,  6,  3, 15,  5,  1, 13, 12,  7, 11,  4,  2,  8, //S3
		13,  7,  0,  9,  3,  4,  6, 10,  2,  8,  5, 14, 12, 11, 15,  1,
		13,  6,  4,  9,  8, 15,  3,  0, 11,  1,  2, 12,  5, 10, 14,  7,
		 1, 10, 13,  0,  6,  9,  8,  7,  4, 15, 14,  3, 11,  5,  2, 12,

		  7, 13, 14,  3,  0,  6,  9, 10,  1,  2,  8,  5, 11, 12,  4, 15, //S4
		13,  8, 11,  5,  6, 15,  0,  3,  4,  7,  2, 12,  1, 10, 14,  9,
		10,  6,  9,  0, 12, 11,  7, 13, 15,  1,  3, 14,  5,  2,  8,  4,
		 3, 15,  0,  6, 10,  1, 13,  8,  9,  4,  5, 11, 12,  7,  2, 14,

		 2, 12,  4,  1,  7, 10, 11,  6,  8,  5,  3, 15, 13,  0, 14,  9,//S5
		14, 11,  2, 12,  4,  7, 13,  1,  5,  0, 15, 10,  3,  9,  8,  6,
		 4,  2,  1, 11, 10, 13,  7,  8, 15,  9, 12,  5,  6,  3,  0, 14,
		11,  8, 12,  7,  1, 14,  2, 13,  6, 15,  0,  9, 10,  4,  5,  3,

		12,  1, 10, 15,  9,  2,  6,  8,  0, 13,  3,  4, 14,  7,  5, 11,//S6
		10, 15,  4,  2,  7, 12,  9,  5,  6,  1, 13, 14,  0, 11,  3,  8,
		 9, 14, 15,  5,  2,  8, 12,  3,  7,  0,  4, 10,  1, 13, 11,  6,
		 4,  3,  2, 12,  9,  5, 15, 10, 11, 14,  1,  7,  6,  0,  8, 13,

		 4, 11,  2, 14, 15,  0,  8, 13,  3, 12,  9,  7,  5, 10,  6,  1,//S7
		13,  0, 11,  7,  4,  9,  1, 10, 14,  3,  5, 12,  2, 15,  8,  6,
		 1,  4, 11, 13, 12,  3,  7, 14, 10, 15,  6,  8,  0,  5,  9,  2,
		 6, 11, 13,  8,  1,  4, 10,  7,  9,  5,  0, 15, 14,  2,  3, 12,

		 13,  2,  8,  4,  6, 15, 11,  1, 10,  9,  3, 14,  5,  0, 12,  7,//S8
		 1, 15, 13,  8, 10,  3,  7,  4, 12,  5,  6, 11,  0, 14,  9,  2,
		 7, 11,  4,  1,  9, 12, 14,  2,  0,  6, 10, 13, 15,  3,  5,  8,
		 2,  1, 14,  7,  4, 10,  8, 13, 15, 12,  9,  0,  3,  5,  6, 11
	};
	//S���������Ǹ��û�P
	const int P[32] = {
		16,  7, 20, 21,
		29, 12, 28, 17,
		 1, 15, 23, 26,
		 5, 18, 31, 10,
		 2,  8, 24, 14,
		32, 27,  3,  9,
		19, 13, 30,  6,
		22, 11,  4, 25,
	};
public:
	unsigned long long key;
	unsigned long long cipher_text;
	unsigned long long decipher_text;
	unsigned long long plain_text;
	unsigned long long encode_key[16];
	//���캯������ʼ����Կ������
	DES() {
		key = 0;
		cipher_text = 0;
		decipher_text = 0;
		plain_text = 0;
	}
	void set_key(unsigned long long k)
	{
		key = k;
	}
	void set_plaintext(unsigned long long p)
	{
		plain_text = p;
	}
	void set_ciphertext(unsigned long long c)
	{
		cipher_text = c;
	}
	void generate_enckey()
	{
		unsigned long long generate_key;
		generate_key = permutation(key, key_IP, 64, 56);
		for (int i = 0; i < 16; i++)
		{
			generate_key = shift(generate_key, enckey_order[i]);
			encode_key[i] = permutation(generate_key, cp, 56, 48);
		}
	}
	unsigned long long shift(unsigned long long key, int n)
	{
		unsigned long long tempKey = 0;
		unsigned long long L, R;
		L = (key & 0xFFFFFFF0000000LL) >> 28;
		//cout<<"L:"<<L<<endl;
		R = key & 0x0000000FFFFFFF;
		if (n == 1) {
			L = ((L & 0x7FFFFFF) << 1) | ((L >> 27) & 1);
			R = ((R & 0x7FFFFFF) << 1) | ((R >> 27) & 1);
			tempKey = (L << 28) | R;
		}
		else if (n == 2) {
			L = ((L & 0x3FFFFFF) << 2) | ((L >> 26) & 3);
			R = ((R & 0x7FFFFFF) << 2) | ((R >> 26) & 3);
			tempKey = (L << 28) | R;
		}
		return tempKey;
	}
	unsigned long long permutation(unsigned long long num, const int p[], int pmax, int n)
	{
		unsigned long long temp = 0;
		int i;
		for (i = 0; i < n; i++)
		{
			temp <<= 1;
			temp |= (num >> (pmax - p[i])) & 1;
		}
		return temp;
	}
	unsigned long long s_boxes(unsigned long long num)
	{
		unsigned long long temp, res = 0;
		for (int i = 0; i < 8; i++)
		{
			temp = (num >> ((7 - i) * 6)) & 0x3f;
			int x = ((temp >> 4) & 0x2) | (temp & 0x1) + i * 4;
			int y = (temp >> 1) & 0xf;
			temp = s_box[x][y];
			temp = temp << ((7 - i) * 4);
			res |= temp;
		}
		return res;
	}
	void encrypt()
	{
		unsigned long long L, R, temp_r, temp;

		temp = permutation(plain_text, IP, 64, 64);
		L = (temp & 0xFFFFFFFF00000000LL) >> 32;
		R = (temp & 0x00000000FFFFFFFFLL);
		temp_r = R;

		for (int i = 0; i < 16; i++)
		{
			temp_r = permutation(R, ep, 32, 48);
			temp_r = temp_r ^ encode_key[i];
			temp_r = s_boxes(temp_r);
			temp_r = permutation(temp_r, P, 32, 32);
			temp_r ^= L;
			L = R;
			R = temp_r;
		}
		temp = (R << 32) | L;
		temp = permutation(temp, inverse_IP, 64, 64);
		cipher_text = temp;
	}
	void decrypt()
	{
		unsigned long long L, R, temp_r, temp;

		temp = permutation(cipher_text, IP, 64, 64);
		L = (temp & 0xffffffff00000000LL) >> 32;
		R = (temp & 0x00000000ffffffffLL);
		temp_r = R;

		for (int i = 0; i < 16; i++)
		{
			temp_r = permutation(R, ep, 32, 48);
			temp_r = temp_r ^ encode_key[15 - i];
			temp_r = s_boxes(temp_r);
			temp_r = permutation(temp_r, P, 32, 32);
			temp_r ^= L;
			L = R;
			R = temp_r;
		}
		temp = (R << 32) | L;
		temp = permutation(temp, inverse_IP, 64, 64);
		decipher_text = temp;
	}
};

unsigned long long DES_cipher(unsigned long long m, unsigned long long key)
{
	DES des;
	des.set_key(key);
	des.set_plaintext(m);
	des.generate_enckey();
	des.encrypt();
	
	return des.cipher_text;
}
unsigned long long DES_decipher(unsigned long long code, unsigned long long key)
{
	DES des;
	des.set_ciphertext(code);
	des.set_key(key);
	des.generate_enckey();
	des.decrypt();
	return des.decipher_text;
}