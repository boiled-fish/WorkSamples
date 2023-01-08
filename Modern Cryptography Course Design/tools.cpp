#include <NTL/ZZX.h>
#include "course_design.h"

using namespace std;
using namespace NTL;


//����·��
void input_path(char *in_path, char *ciphertext_path)
{
	cerr << "�ļ���������ʽ�����ԣ�" << endl;
	cerr << "    a.txt������·����ʽ" << endl;
	cerr << "    ..\\data\\b.dat�����·����ʽ" << endl;
	cerr << "    C:\\Windows\\System32\\c.dat���������·����ʽ" << endl;

	cerr << "�������������ݵ��ļ���: ";
	cin >> in_path;
	cerr << "�����������Կ���ļ���: ";
	cin >> ciphertext_path;

	return;
}
//�����������
void generate_prime(ZZ &p, ZZ &q, int bit_len, unsigned long long seed, unsigned long long key1, unsigned long long key2)
{
	cerr << "�����С�����" << endl;
	double time = clock();
	while (1)
	{
		p = generate_random_num(seed, bit_len / 64, key1, key2);
		if (p % 2 == 0)
			p += 1;
		if (prime_test(p, bit_len))
			break;
	}
	while (1)
	{
		q = generate_random_num(seed, bit_len / 64, key1, key2);
		if (q % 2 == 0)
			q += 1;
		if (prime_test(q, bit_len))
		{
			time = (clock() - time) / 1000;
			cerr << "���ɵ�p��qͨ�����Լ��, ��ʱ " << time << 's';
			cerr << endl << endl;
			break;
		}
	}
}
//���ɼ���ָ��b
void generate_b(ZZ &b, ZZ f_n)
{
	srand(time(0));
	while (1)
	{
		b = rand();
		if (GCD(b, f_n) == 1)
			break;
	}
}
//��չEuclidean�㷨
ZZ ex_gcd(ZZ a, ZZ b, ZZ& x, ZZ& y)
{
	if (b == 0)
	{
		x = 1;
		y = 0;
		return a;
	}
	ZZ r = ex_gcd(b, a % b, x, y);
	ZZ t = x;
	x = y;
	y = t - a / b * y;
	return r;
}
ZZ exp_euclidean(ZZ b, ZZ n)
{
	ZZ d, x, y, err;
	err = -1;
	d = ex_gcd(b, n, x, y);
	if (d == 1)
		return (x % n + n) % n;
	else
		return err;
}
//convert ZZ to int
void conver_zz(ZZ a, int len, int &b)
{
	b = 0;
	//cout << a << ' ' << b << endl;
	for (int i = 0; i < len; i++)
		if (bit(a, i) == 1)
		{
			//cout << b << endl;
			//b = 0;
			b += pow(2, i);
			//cout << b << endl;
		}
}
