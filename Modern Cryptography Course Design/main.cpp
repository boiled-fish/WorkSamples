#include <NTL/ZZX.h>
#include <time.h>
#include <Windows.h>
#include <fstream>
#include <sstream>
#include "course_design.h"

typedef unsigned long long ull;

using namespace std;
using namespace NTL;
//����ģ�飬 ȫ����
int main()
{
	// ��������
	int bit_len = 0;
	char in_path[100] = { 0 }, pub_key_path[100] = { 0 };
	ZZ p, q, n, b, a, f_n;
	ull seed, key1, key2;
	//�ļ�����
	input_path(in_path, pub_key_path);
	ifstream cinfile(in_path, ios::in | ios::binary);
	ofstream out(pub_key_path, ios::out | ios::binary);
	if (!cinfile.is_open())
	{
		cerr << "���ļ�ʧ�ܣ�����·���Ƿ���ȷ�������˳���" << endl;
		return -1;
	}
	//ZZ temp_k, temp_m, ek, dk;
	//�����������
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
	////���ɼ���ָ��b
	//generate_b(b, f_n);
	//cout << "b:" << b << endl;
	//cout << endl;
	////���ɽ���ָ��a
	//a = exp_euclidean(b, f_n);
	//cout << "a:" << a << endl;
	//cout << endl;
	////������ʱ�Ự��Կming
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
	cerr << "Alice����RSA��Կ���̣�" << endl;
	cerr << "1.�����������p��q:" << endl;
	//�����������
	//cerr�������ùܵ�������ض����ļ�ʱʹ����ʾ�����Կ���ʾ��cmd����
	cerr << "���ļ�����64bits������seedΪ��";
	cinfile >> seed;
	cerr << hex << seed << endl;
	cerr << "���ļ��������ԿK1Ϊ��";
	cinfile >> seed;
	cerr << hex << key1 << endl;
	cerr << "���ļ��������ԿK2Ϊ��";
	cinfile >> key2;
	cerr << hex << key2 << endl;
	cerr << "��������Ҫ���ɵ����������λ��(512 bits��1024 bits)��";
	cin >> bit_len;
	//�����������
	generate_prime(p, q, bit_len, seed, key1, key2);
	//p = to_ZZ("11308274039571871879213303540810720367622598707612097610287524053328842566881284447358317607824848003669788437564968801375489384937384301233712584640001519");
	//q = to_ZZ("1077078719345926614527948488060604017856626843110143191873167280747252411569827222937161450824420262668772631272500277006188312581347861889607758930102691");
	n = p * q;
	f_n = (p - 1) * (q - 1);
	//���ɼ���ָ��b
	generate_b(b, f_n);
	cerr << "2.���ɼ���ָ��b = " << b << endl;
	cerr << "��Կ(n, b) = (" << n << ',' << b << ')' << endl;
	out << n << ' ' << b << endl;
	cerr << endl;
	//���ɽ���ָ��a
	a = exp_euclidean(b, f_n);
	cerr << "3.���ɽ���ָ��a = " << a << endl;
	cerr << "˽Կ(a, p, q) = ";
	cerr << "(" << a << ',' << p << ',' << q << ')' << endl;;
	cerr << endl;
	//////////////////////////////////////////////////////////////////////////////////////////////////////
	//Bob���ܹ���
	ZZ m, pub_b, pub_n, k, c1, c2;
	char cipher_path[100] = { 0 }, plaintext_path[100] = { 0 };
	cerr << "Alice����RSA��Կ�ɹ���Bob������ļ��ж�ȡ��Կ" << endl;

	//��������ض���
	// ��ȡ��Կ
	ifstream pkey_in(pub_key_path, ios::in | ios::binary);
	if (!pkey_in.is_open())
	{
		cerr << "�򿪹�Կ�ļ�ʧ�ܣ������˳�" << endl;
		return -1;
	}
	pkey_in >> pub_n >> pub_b;
	cerr << "1.����Ĺ�Կ(n, b)Ϊ:" << endl;
	cerr << '(' << pub_n << ',' << pub_b << ')' << endl << endl;
	//��ȡ����
	cerr << "2.������洢���ĵ��ļ������ļ�������ͬ�ϣ���";
	cin >> plaintext_path;
	ifstream plaintext_in(plaintext_path, ios::in | ios::binary);
	if (!plaintext_in.is_open())
	{
		cerr << "�������ļ�ʧ�ܣ������˳�" << endl;
		return -1;
	}
	cerr << "3.���������mΪ��";
	plaintext_in >> m;
	cerr << m << endl << endl;
	//�������
	cerr << "4.��������������ļ������ļ�������ͬ�ϣ���";
	cin >> cipher_path;
	ofstream cipher_out(cipher_path, ios::in | ios::binary);
	
	//������ʱ�Ự��Կ
	k = generate_random_num(seed, 2, key2, key1);
	cerr << "5.������ʱ�Ự��ԿkΪ��" << k << endl << endl;
	//����c1
	c1 = RSA_cipher(k, pub_b, pub_n);
	cerr << "6.����c1Ϊ��" << c1 << endl;
	//����c2
	c2 = AES_cipher(m, k);
	cerr << "7.����c2Ϊ��" << c2 << endl << endl;
	cerr << "8.���(c1,c2)���ļ�" << cipher_path << "..." << endl;
	cipher_out << c1 << ' ' << c2 << endl;
	cerr << "Bob������ɣ�" << endl << endl;
	////////////////////////////////////////////////////////////////////////////////////////////////////
	//Alice���ܹ���
	cerr << "Alice���ܹ��̣�" << endl;
	ZZ Bob_c1, Bob_c2, decipher_k, decipher_m;
	ifstream cipher_in(cipher_path, ios::in | ios::binary);
	if (!cipher_in.is_open())
	{
		cerr << "�������ļ�ʧ�ܣ������˳�" << endl;
		return -1;
	}

	cerr << "1.���ļ��ж�ȡ��(c1, c2)Ϊ��";
	cipher_in >> Bob_c1 >> Bob_c2;
	cerr << '(' << Bob_c1 << ',' << Bob_c2 << ')' << endl;
	decipher_k = RSA_decipher(Bob_c1, a, n);///////////////////////////////////////////////////
	
	cerr << endl;
	cerr << "2.����c1����Կk��"<< endl;
	cerr << decipher_k << endl;
	cerr << "  Դ��Կk��" << endl;
	cerr << k << endl << endl;
	
	decipher_m = AES_decipher(Bob_c2, decipher_k);
	cerr << "3.���ܳ����ļ�Ϊ: ";
	cerr << decipher_m << endl;
	cerr << "  Դ�ļ�Ϊ:       ";
	cerr << m << endl;
	return 0;
}