#include <NTL/ZZX.h>
#include <cstdlib>
#include <stdio.h>
#include "course_design.h"

using namespace std;
using namespace NTL;
struct word
{
	ZZ wordKey[4];
};
class AES
{
private:
	const int Nb = 4, Nk = 4, Nr = 10;
	ZZ cipher_key[16];
	word plain_text[4];
	word decipher_text[4];
	word Rcon[11];
	word round_key[44];
	const ZZ s_box[16][16] = {
	{to_ZZ(99),  to_ZZ(124), to_ZZ(119), to_ZZ(123), to_ZZ(242), to_ZZ(107), to_ZZ(111), to_ZZ(197), to_ZZ(48),  to_ZZ(01),  to_ZZ(103), to_ZZ(43),  to_ZZ(254), to_ZZ(215), to_ZZ(171), to_ZZ(118)},
	{to_ZZ(202), to_ZZ(130), to_ZZ(201), to_ZZ(125), to_ZZ(250), to_ZZ(89),  to_ZZ(71),  to_ZZ(240), to_ZZ(173), to_ZZ(212), to_ZZ(162), to_ZZ(175), to_ZZ(156), to_ZZ(164), to_ZZ(114), to_ZZ(192)},
	{to_ZZ(183), to_ZZ(253), to_ZZ(147), to_ZZ(38),  to_ZZ(54),  to_ZZ(63),  to_ZZ(247), to_ZZ(204), to_ZZ(52),  to_ZZ(165), to_ZZ(229), to_ZZ(241), to_ZZ(113), to_ZZ(216), to_ZZ(49),  to_ZZ(21)},
	{to_ZZ(4),   to_ZZ(199), to_ZZ(35),  to_ZZ(195), to_ZZ(24),  to_ZZ(150), to_ZZ(05),  to_ZZ(154), to_ZZ(07),  to_ZZ(18),  to_ZZ(128), to_ZZ(226), to_ZZ(235), to_ZZ(39),  to_ZZ(178), to_ZZ(117)},
	{to_ZZ(9),   to_ZZ(131), to_ZZ(44),  to_ZZ(26),  to_ZZ(27),  to_ZZ(110), to_ZZ(90),  to_ZZ(160), to_ZZ(82),  to_ZZ(59),  to_ZZ(214), to_ZZ(179), to_ZZ(41),  to_ZZ(227), to_ZZ(47),  to_ZZ(132)},
	{to_ZZ(83),  to_ZZ(209), to_ZZ(00),  to_ZZ(237), to_ZZ(32),  to_ZZ(252), to_ZZ(177), to_ZZ(91),  to_ZZ(106), to_ZZ(203), to_ZZ(190), to_ZZ(57),  to_ZZ(74),  to_ZZ(76),  to_ZZ(88),  to_ZZ(207)},
	{to_ZZ(208), to_ZZ(239), to_ZZ(170), to_ZZ(251), to_ZZ(67),  to_ZZ(77),  to_ZZ(51),  to_ZZ(133), to_ZZ(69),  to_ZZ(249), to_ZZ(02),  to_ZZ(127), to_ZZ(80),  to_ZZ(60),  to_ZZ(159), to_ZZ(168)},
	{to_ZZ(81),  to_ZZ(163), to_ZZ(64),  to_ZZ(143), to_ZZ(146), to_ZZ(157), to_ZZ(56),  to_ZZ(245), to_ZZ(188), to_ZZ(182), to_ZZ(218), to_ZZ(33),  to_ZZ(16),  to_ZZ(255), to_ZZ(243), to_ZZ(210)},
	{to_ZZ(205), to_ZZ(12),  to_ZZ(19),  to_ZZ(236), to_ZZ(95),  to_ZZ(151), to_ZZ(68),  to_ZZ(23),  to_ZZ(196), to_ZZ(167), to_ZZ(126), to_ZZ(61),  to_ZZ(100), to_ZZ(93),  to_ZZ(25),  to_ZZ(115)},
	{to_ZZ(96),  to_ZZ(129), to_ZZ(79),  to_ZZ(220), to_ZZ(34),  to_ZZ(42),  to_ZZ(144), to_ZZ(136), to_ZZ(70),  to_ZZ(238), to_ZZ(184), to_ZZ(20),  to_ZZ(222), to_ZZ(94),  to_ZZ(11),  to_ZZ(219)},
	{to_ZZ(224), to_ZZ(50),  to_ZZ(58),  to_ZZ(10),  to_ZZ(73),  to_ZZ(6),   to_ZZ(36),  to_ZZ(92),  to_ZZ(194), to_ZZ(211), to_ZZ(172), to_ZZ(98),  to_ZZ(145), to_ZZ(149), to_ZZ(228), to_ZZ(121)},
	{to_ZZ(231), to_ZZ(200), to_ZZ(55),  to_ZZ(109), to_ZZ(141), to_ZZ(213), to_ZZ(78),  to_ZZ(169), to_ZZ(108), to_ZZ(86),  to_ZZ(244), to_ZZ(234), to_ZZ(101), to_ZZ(122), to_ZZ(174), to_ZZ(8)},
	{to_ZZ(186), to_ZZ(120), to_ZZ(37),  to_ZZ(46),  to_ZZ(28),  to_ZZ(166), to_ZZ(180), to_ZZ(198), to_ZZ(232), to_ZZ(221), to_ZZ(116), to_ZZ(31),  to_ZZ(75),  to_ZZ(189), to_ZZ(139), to_ZZ(138)},
	{to_ZZ(112), to_ZZ(62),  to_ZZ(181), to_ZZ(102), to_ZZ(72),  to_ZZ(03),  to_ZZ(246), to_ZZ(14),  to_ZZ(97),  to_ZZ(53),  to_ZZ(87),  to_ZZ(185), to_ZZ(134), to_ZZ(193), to_ZZ(29),  to_ZZ(158)},
	{to_ZZ(225), to_ZZ(248), to_ZZ(152), to_ZZ(17),  to_ZZ(105), to_ZZ(217), to_ZZ(142), to_ZZ(148), to_ZZ(155), to_ZZ(30),  to_ZZ(135), to_ZZ(233), to_ZZ(206), to_ZZ(85),  to_ZZ(40),  to_ZZ(223)},
	{to_ZZ(140), to_ZZ(161), to_ZZ(137), to_ZZ(13),  to_ZZ(191), to_ZZ(230), to_ZZ(66),  to_ZZ(104), to_ZZ(65),  to_ZZ(153), to_ZZ(45),  to_ZZ(15),  to_ZZ(176), to_ZZ(84),  to_ZZ(187), to_ZZ(22)}
	};
	const ZZ inv_s_box[16][16] = {
	{to_ZZ(82),  to_ZZ(9),   to_ZZ(106), to_ZZ(213), to_ZZ(48),  to_ZZ(54),  to_ZZ(165), to_ZZ(56),  to_ZZ(191), to_ZZ(64),  to_ZZ(163), to_ZZ(158), to_ZZ(129), to_ZZ(243), to_ZZ(215), to_ZZ(251)},
	{to_ZZ(124), to_ZZ(227), to_ZZ(57),  to_ZZ(130), to_ZZ(155), to_ZZ(47),  to_ZZ(255), to_ZZ(135), to_ZZ(52),  to_ZZ(142), to_ZZ(67),  to_ZZ(68),  to_ZZ(196), to_ZZ(222), to_ZZ(233), to_ZZ(203)},
	{to_ZZ(84),  to_ZZ(123), to_ZZ(148), to_ZZ(50),  to_ZZ(166), to_ZZ(194), to_ZZ(35),  to_ZZ(61),  to_ZZ(238), to_ZZ(76),  to_ZZ(149), to_ZZ(11),  to_ZZ(66),  to_ZZ(250), to_ZZ(195), to_ZZ(78)},
	{to_ZZ(8),   to_ZZ(46),  to_ZZ(161), to_ZZ(102), to_ZZ(40),  to_ZZ(217), to_ZZ(36),  to_ZZ(178), to_ZZ(118), to_ZZ(91),  to_ZZ(162), to_ZZ(73),  to_ZZ(109), to_ZZ(139), to_ZZ(209), to_ZZ(37)},
	{to_ZZ(114), to_ZZ(248), to_ZZ(246), to_ZZ(100), to_ZZ(134), to_ZZ(104), to_ZZ(152), to_ZZ(22),  to_ZZ(212), to_ZZ(164), to_ZZ(92),  to_ZZ(204), to_ZZ(93),  to_ZZ(101), to_ZZ(182), to_ZZ(146)},
	{to_ZZ(108), to_ZZ(112), to_ZZ(72),  to_ZZ(80),  to_ZZ(253), to_ZZ(237), to_ZZ(185), to_ZZ(218), to_ZZ(94),  to_ZZ(21),  to_ZZ(70),  to_ZZ(87),  to_ZZ(167), to_ZZ(141), to_ZZ(157), to_ZZ(132)},
	{to_ZZ(144), to_ZZ(216), to_ZZ(171), to_ZZ(0),   to_ZZ(140), to_ZZ(188), to_ZZ(211), to_ZZ(10),  to_ZZ(247), to_ZZ(228), to_ZZ(88),  to_ZZ(5),   to_ZZ(184), to_ZZ(179), to_ZZ(69),  to_ZZ(6)},
	{to_ZZ(208), to_ZZ(44),  to_ZZ(30),  to_ZZ(143), to_ZZ(202), to_ZZ(63),  to_ZZ(15),  to_ZZ(2),   to_ZZ(193), to_ZZ(175), to_ZZ(189), to_ZZ(3),   to_ZZ(1),   to_ZZ(19),  to_ZZ(138), to_ZZ(107)},
	{to_ZZ(58),  to_ZZ(145), to_ZZ(17),  to_ZZ(65),  to_ZZ(79),  to_ZZ(103), to_ZZ(220), to_ZZ(234), to_ZZ(151), to_ZZ(242), to_ZZ(207), to_ZZ(206), to_ZZ(240), to_ZZ(180), to_ZZ(230), to_ZZ(115)},
	{to_ZZ(150), to_ZZ(172), to_ZZ(116), to_ZZ(34),  to_ZZ(231), to_ZZ(173), to_ZZ(53),  to_ZZ(133), to_ZZ(226), to_ZZ(249), to_ZZ(55),  to_ZZ(232), to_ZZ(28),  to_ZZ(117), to_ZZ(223), to_ZZ(110)},
	{to_ZZ(71),  to_ZZ(241), to_ZZ(26),  to_ZZ(113), to_ZZ(29),  to_ZZ(41),  to_ZZ(197), to_ZZ(137), to_ZZ(111), to_ZZ(183), to_ZZ(98),  to_ZZ(14),  to_ZZ(170), to_ZZ(24),  to_ZZ(190), to_ZZ(27)},
	{to_ZZ(252), to_ZZ(86),  to_ZZ(62),  to_ZZ(75),  to_ZZ(198), to_ZZ(210), to_ZZ(121), to_ZZ(32),  to_ZZ(154), to_ZZ(219), to_ZZ(192), to_ZZ(254), to_ZZ(120), to_ZZ(205), to_ZZ(90),  to_ZZ(244)},
	{to_ZZ(31),  to_ZZ(221), to_ZZ(168), to_ZZ(51),  to_ZZ(136), to_ZZ(7),   to_ZZ(199), to_ZZ(49),  to_ZZ(177), to_ZZ(18),  to_ZZ(16),  to_ZZ(89),  to_ZZ(39),  to_ZZ(128), to_ZZ(236), to_ZZ(95)},
	{to_ZZ(96),  to_ZZ(81),  to_ZZ(127), to_ZZ(169), to_ZZ(25),  to_ZZ(181), to_ZZ(74),  to_ZZ(13),  to_ZZ(45),  to_ZZ(229), to_ZZ(122), to_ZZ(159), to_ZZ(147), to_ZZ(201), to_ZZ(156), to_ZZ(239)},
	{to_ZZ(160), to_ZZ(224), to_ZZ(59),  to_ZZ(77),  to_ZZ(174), to_ZZ(42),  to_ZZ(245), to_ZZ(176), to_ZZ(200), to_ZZ(235), to_ZZ(187), to_ZZ(60),  to_ZZ(131), to_ZZ(83),  to_ZZ(153), to_ZZ(97)},
	{to_ZZ(23),  to_ZZ(43),  to_ZZ(4),   to_ZZ(126), to_ZZ(186), to_ZZ(119), to_ZZ(214), to_ZZ(38),  to_ZZ(225), to_ZZ(105), to_ZZ(20),  to_ZZ(99),  to_ZZ(85),  to_ZZ(33),  to_ZZ(12),  to_ZZ(125)}
	};
public:
	word cipher_text[4];
	AES() {
		ini_Rcon();
	}
	void ini_Rcon()
	{
		for (int i = 0; i < 10; i++)
			for (int j = 0; j < 4; j++)
				Rcon[i].wordKey[j] = 0;
		Rcon[1].wordKey[0] = 01;
		Rcon[2].wordKey[0] = 02;
		Rcon[3].wordKey[0] = 04;
		Rcon[4].wordKey[0] = 8;
		Rcon[5].wordKey[0] = 16;
		Rcon[6].wordKey[0] = 32;
		Rcon[7].wordKey[0] = 64;
		Rcon[8].wordKey[0] = 128;
		Rcon[9].wordKey[0] = 27;
		Rcon[10].wordKey[0] = 54;
	}
	void set_key(ZZ key)
	{
		for (int i = 0; i < 16; i++)
		{
			ZZ temp;
			int temp_1 = 0;
			for (int j = 0; j < 8; j++)
				if (bit(key, j) == 1)
					temp_1 += pow(2, j);
			temp = temp_1;
			//cout << temp << endl;
			cipher_key[i] = temp;
			//cout << cipher_key[i] << endl;
			key >>= 8;
		}
		//show_key();
		//cout << endl;
		key_expansion(cipher_key, round_key);
		//show_round_key();
		return;
	}
	void set_plaintext(ZZ p_text)
	{
		//cout << p_text << endl;
		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 4; j++)
			{
				ZZ temp;
				int temp_1 = 0;
				for (int k = 0; k < 8; k++)
					if (bit(p_text, k) == 1)
						temp_1 += pow(2, k);

				temp = temp_1;
				//cout << temp << endl;
				plain_text[i].wordKey[j] = temp;
				//cout << plain_text[i].wordKey[j] << endl;
				p_text >>= 8;
			}
		}
	}
	void set_cipher(ZZ cipher)
	{
		//cout << cipher << endl;
		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 4; j++)
			{
				ZZ temp, v2 = to_ZZ(2);
				int temp_1 = 0;
				for (int k = 0; k < 8; k++)
					if (bit(cipher, k) == 1)
						temp += power(v2, k);

				cipher_text[i].wordKey[j] = temp;
				//cout << cipher_text[i].wordKey[j] << endl;
				cipher >>= 8;
			}
		}
	}

	word rotWord(word w)
	{
		word temp;
		for (int i = 0; i < 4; i++)
			temp.wordKey[(i + 3) % 4] = w.wordKey[i];
		return temp;
	}
	word subWord(word w)
	{
		ZZ t_l, t_r;
		int L = 0, R = 0;
		for (int i = 0; i < 4; i++)
		{
			t_l = w.wordKey[i] >> 4;
			t_r = w.wordKey[i] & 0x0F;
			//cout << t_l << ' ' << t_r << endl;
			conver_zz(t_l, 8, L);
			conver_zz(t_r, 8, R);
			//cout << L << ' ' << R << endl;
			w.wordKey[i] = s_box[L][R];
		}
		return w;
	}
	word wordXor(word w1, word w2)
	{
		word temp;
		for (int i = 0; i < 4; i++)
			temp.wordKey[i] = w1.wordKey[i] ^ w2.wordKey[i];
		return temp;
	}
	void key_expansion(ZZ key[], word w[])
	{
		int i = 0;
		word temp;
		for (int i = 0; i < 4; i++)
			for (int j = 0; j < 4; j++)
				w[j].wordKey[i] = key[j + 4 * i];

		i = 4;
		while (i < 44)
		{
			temp = w[i - 1];
			if ((i % 4) == 0)
			{
				/*for(int k = 0; k < 4; k++)
					cout << temp.wordKey[k] << ' ';
				cout << endl;*/
				temp = rotWord(temp);
				/*for (int k = 0; k < 4; k++)
					cout << temp.wordKey[k] << ' ';
				cout << endl;*/
				temp = subWord(temp);
				/*for (int k = 0; k < 4; k++)
					cout << temp.wordKey[k] << ' ';
				cout << endl;*/
				temp = wordXor(temp, Rcon[i / 4]);
				/*for (int k = 0; k < 4; k++)
					cout << temp.wordKey[k] << ' ';
				cout << endl;*/
				//temp = wordXor(temp, w[i - 4]);
				/*for (int k = 0; k < 4; k++)
					cout << temp.wordKey[k] << ' ';
				cout << endl;*/
			}
			w[i] = wordXor(w[i - 4], temp);
			/*for (int k = 0; k < 4; k++)
				cout << w[i].wordKey[k] << ' ';
			cout << endl;*/
			i++;
		}
	}

	void sub_byte(word w[])
	{
		ZZ t_l, t_r;
		int l, r;
		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 4; j++)
			{
				//cout << w[i].wordKey[j] << endl;
				t_l = w[i].wordKey[j] >> 4;
				t_r = w[i].wordKey[j] & 0x0F;
				//cout << t_l << endl << t_r << endl;
				conver_zz(t_l, 8, l);
				conver_zz(t_r, 8, r);
				w[i].wordKey[j] = s_box[l][r];
			}
		}
	}
	void sub_shift(word& sub_w)
	{
		ZZ temp = sub_w.wordKey[0];
		for (int i = 1; i <= 3; i++)
			sub_w.wordKey[i - 1] = sub_w.wordKey[i];
		sub_w.wordKey[3] = temp;
	}
	void shift_rows(word w[])
	{
		for (int i = 1; i < 4; i++)
			for (int j = 0; j < i; j++)
				sub_shift(w[i]);
	}
	ZZ field_mul(ZZ x)
	{
		ZZ y, t;
		t = x & 0b10000000;

		if (t != 0)
			y = ((x - 128) << 1) ^ 27;
		else
			y = x << 1;

		return y;
	}
	void mix_columns(word w[])
	{
		for (int i = 0; i < 4; i++)
		{
			ZZ x0, x1, x2, x3;
			x0 = w[0].wordKey[i];
			x1 = w[1].wordKey[i];
			x2 = w[2].wordKey[i];
			x3 = w[3].wordKey[i];
			//cout << x0 << endl;
			//cout << x1 << endl;
			//cout << x2 << endl;
			//cout << x3 << endl << endl;

			w[0].wordKey[i] = x1 ^ x2 ^ x3;
			w[1].wordKey[i] = x0 ^ x2 ^ x3;
			w[2].wordKey[i] = x0 ^ x1 ^ x3;
			w[3].wordKey[i] = x0 ^ x1 ^ x2;
			/*		cout << w[0].wordKey[i] << endl;
					cout << w[1].wordKey[i] << endl;
					cout << w[2].wordKey[i] << endl;
					cout << w[3].wordKey[i] << endl << endl;*/

			x0 = field_mul(x0);
			x1 = field_mul(x1);
			x2 = field_mul(x2);
			x3 = field_mul(x3);

			w[0].wordKey[i] = w[0].wordKey[i] ^ x0 ^ x1;
			w[1].wordKey[i] = w[1].wordKey[i] ^ x1 ^ x2;
			w[2].wordKey[i] = w[2].wordKey[i] ^ x2 ^ x3;
			w[3].wordKey[i] = w[3].wordKey[i] ^ x3 ^ x0;
		}
	}
	void add_round_key(word w[], int round)
	{
		for (int i = 0; i < 4; i++)
			for (int j = 0; j < 4; j++)
			{
				//cout << round_key[i + 4 * round].wordKey[j] << ' ' << w[j].wordKey[i] << endl;
				w[j].wordKey[i] ^= round_key[i + 4 * round].wordKey[j];
				//cout << w[j].wordKey[i] << endl;
			}
	}
	void encrypt()
	{
		for (int i = 0; i < 4; i++)
			for (int j = 0; j < 4; j++)
				cipher_text[i].wordKey[j] = plain_text[i].wordKey[j];
		//show_cipher();
		add_round_key(cipher_text, 0);
		//show_cipher();
		for (int i = 1; i < 10; i++)
		{
			sub_byte(cipher_text);
			//show_cipher();
			shift_rows(cipher_text);
			//show_cipher();
			mix_columns(cipher_text);
			//show_cipher();
			add_round_key(cipher_text, i);
			//show_cipher();
		}
		sub_byte(cipher_text);
		//show_cipher();
		shift_rows(cipher_text);
		//show_cipher();
		add_round_key(cipher_text, 10);
		//show_cipher();
	}

	void inv_sub_byte(word w[])
	{
		ZZ t_l, t_r;
		int l, r;
		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 4; j++)
			{
				t_l = w[i].wordKey[j] >> 4;
				t_r = w[i].wordKey[j] & 0x0F;
				//cout << t_l << ' ' << t_r << endl;
				conver_zz(t_l, 8, l);
				conver_zz(t_r, 8, r);
				//cout << l << ' ' << r << endl;
				w[i].wordKey[j] = inv_s_box[l][r];
				//cout << w[i].wordKey[j] << endl;
			}
		}
	}
	void inv_shoft_rows(word w[])
	{
		for (int i = 1; i < 4; i++)
			for (int j = 4; j > i; j--)
				sub_shift(w[i]);
	}
	void inv_mix_columns(word w[])
	{
		for (int i = 0; i < 4; i++)
		{
			ZZ x0, x1, x2, x3;
			x0 = w[0].wordKey[i];
			x1 = w[1].wordKey[i];
			x2 = w[2].wordKey[i];
			x3 = w[3].wordKey[i];

			w[0].wordKey[i] = x1 ^ x2 ^ x3;
			w[1].wordKey[i] = x0 ^ x2 ^ x3;
			w[2].wordKey[i] = x0 ^ x1 ^ x3;
			w[3].wordKey[i] = x0 ^ x1 ^ x2;

			x0 = field_mul(x0);
			x1 = field_mul(x1);
			x2 = field_mul(x2);
			x3 = field_mul(x3);

			w[0].wordKey[i] = w[0].wordKey[i] ^ x0 ^ x1;
			w[1].wordKey[i] = w[1].wordKey[i] ^ x1 ^ x2;
			w[2].wordKey[i] = w[2].wordKey[i] ^ x2 ^ x3;
			w[3].wordKey[i] = w[3].wordKey[i] ^ x3 ^ x0;

			x0 = field_mul(x0 ^ x2);
			x1 = field_mul(x1 ^ x3);

			w[0].wordKey[i] = w[0].wordKey[i] ^ x0;
			w[1].wordKey[i] = w[1].wordKey[i] ^ x1;
			w[2].wordKey[i] = w[2].wordKey[i] ^ x0;
			w[3].wordKey[i] = w[3].wordKey[i] ^ x1;

			x0 = field_mul(x0 ^ x1);

			w[0].wordKey[i] = w[0].wordKey[i] ^ x0;
			w[1].wordKey[i] = w[1].wordKey[i] ^ x0;
			w[2].wordKey[i] = w[2].wordKey[i] ^ x0;
			w[3].wordKey[i] = w[3].wordKey[i] ^ x0;
		}
	}
	void decrypt()
	{
		for (int i = 0; i < 4; i++)
			for (int j = 0; j < 4; j++)
				decipher_text[i].wordKey[j] = cipher_text[i].wordKey[j];
		//show_decipher();
		add_round_key(decipher_text, 10);
		//show_decipher();
		for (int i = 9; i > 0; i--)
		{
			inv_shoft_rows(decipher_text);
			//show_decipher();
			inv_sub_byte(decipher_text);
			//show_decipher();
			add_round_key(decipher_text, i);
			//show_decipher();
			inv_mix_columns(decipher_text);
			//show_decipher();
		}
		inv_shoft_rows(decipher_text);
		//show_decipher();
		inv_sub_byte(decipher_text);
		//show_decipher();
		add_round_key(decipher_text, 0);
		//show_decipher();
	}

	ZZ get_cipher(int len)
	{
		ZZ res = to_ZZ(0);
		//int start = 0, flag = 0, count = 0, space = 128 - len;
		//start = space / 8;
		for (int i = 3; i >= 0; i--)
		{
			for (int j = 3; j >= 0; j--)
			{
				res += cipher_text[i].wordKey[j];
				if (i == 0 && j == 0)
					break;
				else
					res <<= 8;
				//cout << NumBits(res) << endl;
			}
		}
		//cout << NumBits(res) << endl;
		return res;
	}
	ZZ get_decipher_text(int len)
	{
		ZZ res = to_ZZ(0);
		int start = 0, flag = 0, count = 0, space = 128 - len;
		start = space / 8;
		space %= 8;
		for (int i = 3; i >= 0; i--)
		{
			for (int j = 3; j >= 0; j--)
			{
				count++;
				//cout << count << endl;
				//cout << decipher_text[i].wordKey[j] << endl << endl;
				if (count > start)
				{
					/*ZZ ttt;
					ttt = 255 >> space;
					cout << ttt << endl;*/
					//cout << decipher_text[i].wordKey[j] << endl;
					//decipher_text[i].wordKey[j] &= ttt;
					//cout << decipher_text[i].wordKey[j] << endl;
					res += decipher_text[i].wordKey[j];
					//cout << res << endl << endl;
					if (count < 16)
						res <<= 8;
					//cout << res << endl << endl;
				}
				//cout << count << endl;
				//cout << decipher_text[i].wordKey[j] << endl;
				//res += decipher_text[i].wordKey[j];
			//	cout << res << endl;

				//cout << res << endl << endl;
			}
		}
		//cout << res << endl << endl;
		return res;
	}
	void show_cipher()
	{
		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 4; j++)
				cout << cipher_text[i].wordKey[j] << ' ';
			cout << endl;
		}
		cout << endl;
	}
	void show_decipher()
	{
		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 4; j++)
				cout << decipher_text[i].wordKey[j] << ' ';
			cout << endl;
		}
		cout << endl;
	}
	void show_key()
	{
		for (int i = 0; i < 16;)
		{
			for (int j = 0; j < 4; j++)
				cout << cipher_key[i++] << ' ';
			cout << endl;
		}

	}
	void show_plaintext()
	{
		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 4; j++)
				cout << plain_text[i].wordKey[j] << ' ';
			cout << endl;
		}

	}
	void show_round_key()
	{
		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 44; j++)
				cout << round_key[j].wordKey[i] << ' ';
			cout << endl;
		}
	}
};

ZZ AES_cipher(ZZ m, ZZ k)
{
	ZZ cipher = to_ZZ(0);
	ZZ temp = m;
	ZZ y;
	int bit_len = NumBits(m);
	while (bit_len > 0)
	{
		AES aes;
		ZZ temp_m = to_ZZ(0), v2 = to_ZZ(2);

		for (int i = 0; i < 128; i++)
		{
			if (bit(m, i) == 1)
				temp_m += power(v2, i);
		}
		temp_m ^= y;
		//cout << NumBits(temp_m);
		aes.set_plaintext(temp_m);
		aes.set_key(k);
		aes.show_plaintext();
		//aes.show_key();
		aes.encrypt();
		//aes.show_cipher();
		y = aes.get_cipher(bit_len);
		cipher += y;
		if (bit_len > 128)
			cipher <<= 128;
		m >>= 128;
		bit_len -= 128;
	}
	//cout << NumBits(cipher);
	return cipher;
}
ZZ AES_decipher(ZZ cipher, ZZ k)
{
	ZZ decipher_text = to_ZZ(0);
	ZZ temp = cipher;
	ZZ y;
	//int bit_len = NumBits(cipher);
	int bit_len = NumBits(cipher);
	while (bit_len > 0)
	{
		AES aes;
		ZZ temp_m = to_ZZ(0), v2 = to_ZZ(2);

		for (int i = 0; i < 128; i++)
		{
			if (bit(cipher, i) == 1)
				temp_m += power(v2, i);
		}
		aes.set_cipher(temp_m);
		//aes.show_cipher();
		aes.set_key(k);
		//aes.show_key();
		aes.decrypt();
		//aes.show_decipher();
		//132449862177579093398041
		//56890841008899361800713
		y = aes.get_decipher_text(bit_len) ^ y;
		//temp_m ^= y;
		//cout << y << endl;
		//cout << decipher_text << endl;
		decipher_text += y;
		//cout << decipher_text << endl;
		if (bit_len > 128)
			decipher_text <<= 128;
		cipher >>= 128;
		bit_len -= 128;
	}
	return decipher_text;
}