#include <NTL/ZZX.h>
#include <time.h>
#include <Windows.h>
#include <fstream>
#include <sstream>
#include "course_design.h"

typedef unsigned long long ull;

using namespace std;
using namespace NTL;
//顶层模块， 全流程
int main()
{
	// 变量定义
	int bit_len = 0;
	char in_path[100] = { 0 }, pub_key_path[100] = { 0 };
	ZZ p, q, n, b, a, f_n;
	ull seed, key1, key2;
	//文件处理
	input_path(in_path, pub_key_path);
	ifstream cinfile(in_path, ios::in | ios::binary);
	ofstream out(pub_key_path, ios::out | ios::binary);
	if (!cinfile.is_open())
	{
		cerr << "打开文件失败，请检查路径是否正确，程序退出。" << endl;
		return -1;
	}
	//ZZ temp_k, temp_m, ek, dk;
	//生成随机素数
	cinfile >> seed;
	cinfile >> key1;
	cinfile >> key2;
	//bit_len = 512;
	//generate_prime(p, q, bit_len, seed, key1, key2);
	//p = to_ZZ("11308274039571871879213303540810720367622598707612097610287524053328842566881284447358317607824848003669788437564968801375489384937384301233712584640001519");
	//q = to_ZZ("1077078719345926614527948488060604017856626843110143191873167280747252411569827222937161450824420262668772631272500277006188312581347861889607758930102691");
	//cout << "p:" << p << endl;
	//cout << "q:" << q << endl << endl; 
	//n = p * q;
	//f_n = (p - 1) * (q - 1);
	//cout << "n:" << n << endl;
	//cout << endl;
	//cout << "f_n:" << f_n << endl;
	//cout << endl;
	////生成加密指数b
	//generate_b(b, f_n);
	//cout << "b:" << b << endl;
	//cout << endl;
	////生成解密指数a
	//a = exp_euclidean(b, f_n);
	//cout << "a:" << a << endl;
	//cout << endl;
	////生成临时会话密钥ming
	//temp_k = generate_random_num(seed, 2, key2, key1);
	//cout << "k:" << temp_k << endl;
	//cout << endl;
	//ek = PowerMod(temp_k, b, n);
	//cout << "ek:" << ek << endl;
	//cout << endl;
	//dk = PowerMod(ek, a, n);
	//cout << "dk:" << dk;
	/////////////////////////////////////////////////////////////////////////////////////////////////////////
	cerr << endl;
	cerr << "Alice生成RSA密钥过程：" << endl;
	cerr << "1.生成随机素数p，q:" << endl;
	//生成随机素数
	//cerr可以在用管道运算符重定向到文件时使得提示文字仍可显示在cmd窗口
	cerr << "从文件读入64bits的种子seed为：";
	cinfile >> seed;
	cerr << hex << seed << endl;
	cerr << "从文件读入的密钥K1为：";
	cinfile >> seed;
	cerr << hex << key1 << endl;
	cerr << "从文件读入的密钥K2为：";
	cinfile >> key2;
	cerr << hex << key2 << endl;
	cerr << "请输入需要生成的随机素数的位数(512 bits或1024 bits)：";
	cin >> bit_len;
	//生成随机素数
	generate_prime(p, q, bit_len, seed, key1, key2);
	//p = to_ZZ("11308274039571871879213303540810720367622598707612097610287524053328842566881284447358317607824848003669788437564968801375489384937384301233712584640001519");
	//q = to_ZZ("1077078719345926614527948488060604017856626843110143191873167280747252411569827222937161450824420262668772631272500277006188312581347861889607758930102691");
	n = p * q;
	f_n = (p - 1) * (q - 1);
	//生成加密指数b
	generate_b(b, f_n);
	cerr << "2.生成加密指数b = " << b << endl;
	cerr << "公钥(n, b) = (" << n << ',' << b << ')' << endl;
	out << n << ' ' << b << endl;
	cerr << endl;
	//生成解密指数a
	a = exp_euclidean(b, f_n);
	cerr << "3.生成解密指数a = " << a << endl;
	cerr << "私钥(a, p, q) = ";
	cerr << "(" << a << ',' << p << ',' << q << ')' << endl;;
	cerr << endl;
	//////////////////////////////////////////////////////////////////////////////////////////////////////
	//Bob加密过程
	ZZ m, pub_b, pub_n, k, c1, c2;
	char cipher_path[100] = { 0 }, plaintext_path[100] = { 0 };
	cerr << "Alice生成RSA密钥成功！Bob从输出文件中读取公钥" << endl;

	//输入输出重定向
	// 读取公钥
	ifstream pkey_in(pub_key_path, ios::in | ios::binary);
	if (!pkey_in.is_open())
	{
		cerr << "打开公钥文件失败，程序退出" << endl;
		return -1;
	}
	pkey_in >> pub_n >> pub_b;
	cerr << "1.读入的公钥(n, b)为:" << endl;
	cerr << '(' << pub_n << ',' << pub_b << ')' << endl << endl;
	//读取明文
	cerr << "2.请输入存储明文的文件名（文件名规则同上）：";
	cin >> plaintext_path;
	ifstream plaintext_in(plaintext_path, ios::in | ios::binary);
	if (!plaintext_in.is_open())
	{
		cerr << "打开明文文件失败，程序退出" << endl;
		return -1;
	}
	cerr << "3.读入的明文m为：";
	plaintext_in >> m;
	cerr << m << endl << endl;
	//密文输出
	cerr << "4.请输入输出密文文件名（文件名规则同上）：";
	cin >> cipher_path;
	ofstream cipher_out(cipher_path, ios::in | ios::binary);
	
	//生成临时会话密钥
	k = generate_random_num(seed, 2, key2, key1);
	cerr << "5.生成临时会话密钥k为：" << k << endl << endl;
	//生成c1
	c1 = RSA_cipher(k, pub_b, pub_n);
	cerr << "6.生成c1为：" << c1 << endl;
	//生成c2
	c2 = AES_cipher(m, k);
	cerr << "7.生成c2为：" << c2 << endl << endl;
	cerr << "8.输出(c1,c2)到文件" << cipher_path << "..." << endl;
	cipher_out << c1 << ' ' << c2 << endl;
	cerr << "Bob加密完成！" << endl << endl;
	////////////////////////////////////////////////////////////////////////////////////////////////////
	//Alice解密过程
	cerr << "Alice解密过程：" << endl;
	ZZ Bob_c1, Bob_c2, decipher_k, decipher_m;
	ifstream cipher_in(cipher_path, ios::in | ios::binary);
	if (!cipher_in.is_open())
	{
		cerr << "打开密文文件失败，程序退出" << endl;
		return -1;
	}

	cerr << "1.从文件中读取的(c1, c2)为：";
	cipher_in >> Bob_c1 >> Bob_c2;
	cerr << '(' << Bob_c1 << ',' << Bob_c2 << ')' << endl;
	decipher_k = RSA_decipher(Bob_c1, a, n);///////////////////////////////////////////////////
	
	cerr << endl;
	cerr << "2.解密c1得密钥k："<< endl;
	cerr << decipher_k << endl;
	cerr << "  源密钥k：" << endl;
	cerr << k << endl << endl;
	
	decipher_m = AES_decipher(Bob_c2, decipher_k);
	cerr << "3.解密出的文件为: ";
	cerr << decipher_m << endl;
	cerr << "  源文件为:       ";
	cerr << m << endl;
	return 0;
}