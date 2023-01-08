#include <NTL/ZZX.h>
#include "course_design.h"

using namespace std;
using namespace NTL;


//输入路径
void input_path(char *in_path, char *ciphertext_path)
{
	cerr << "文件名以下形式均可以：" << endl;
	cerr << "    a.txt：不带路径形式" << endl;
	cerr << "    ..\\data\\b.dat：相对路径形式" << endl;
	cerr << "    C:\\Windows\\System32\\c.dat：绝对相对路径形式" << endl;

	cerr << "请输入输入数据的文件名: ";
	cin >> in_path;
	cerr << "请输入输出公钥的文件名: ";
	cin >> ciphertext_path;

	return;
}
//生成随机素数
void generate_prime(ZZ &p, ZZ &q, int bit_len, unsigned long long seed, unsigned long long key1, unsigned long long key2)
{
	cerr << "生成中。。。" << endl;
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
			cerr << "生成的p，q通过素性检测, 用时 " << time << 's';
			cerr << endl << endl;
			break;
		}
	}
}
//生成加密指数b
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
//扩展Euclidean算法
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
