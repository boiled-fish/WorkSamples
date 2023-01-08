#include <NTL/ZZX.h>
#include <iomanip>
#include <time.h>
#include "course_design.h"

using namespace std;
using namespace NTL;

// µœ÷À„∑® ANSI X9.17
ZZ generate_random_num(unsigned long long s, int num, unsigned long long k1, unsigned long long k2)
{
	unsigned long long l, D;
	unsigned long long * bits = new unsigned long long[num];// [64] = { 0 };
	char** t = new char*[num];
	for (int i = 0; i < num; i++)
		t[i] = new char[64];
	char *temp_zz = new char[num * 64];
	ZZ test = to_ZZ(0);
	memset(bits, 0, sizeof(ZZ) * num);
	memset(temp_zz, 0, sizeof(char) * num * 64);

	//srand(time(0));
	//D = rand();
	D = time(0);
	l = DES_cipher((DES_decipher((DES_cipher(D, k1)), k2)), k1);
	
	for (int i = 0; i < num; i++)
	{
		bits[i] = DES_cipher((DES_decipher((DES_cipher(l ^ s, k1)), k2)), k1);
		s = DES_cipher(DES_decipher(DES_cipher(bits[i] ^ l, k1), k2), k1);
	}
	for (int i = 0; i < num; i++)
	{
		unsigned long long temp = bits[i];
		int len = 0;
		while (temp != 0)
		{
			len++;
			temp /= 10;
		}
		for (int j = len - 1; j >= 0; j--)
		{
			t[i][j] = (bits[i] % 10) + '0';
			bits[i] /= 10;
		}
	}
	for (int i = 0; i < num; i++)
	{
		ZZ plus = to_ZZ(t[i]);
		//cout << test << endl;
		test += plus;
		//cout << test << endl;
		if(i < num -1)
			test <<= 64;
		//cout << test << endl;
	}

	return test;
}
