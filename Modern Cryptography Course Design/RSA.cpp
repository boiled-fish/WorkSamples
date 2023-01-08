#include <NTL/ZZX.h>
#include "course_design.h"

using namespace std;
using namespace NTL;

ZZ RSA_cipher(ZZ x, ZZ b, ZZ n)
{
	ZZ y;
	y = PowerMod(x, b, n);
	return y;
}
ZZ RSA_decipher(ZZ y, ZZ a, ZZ n)
{
	ZZ x;
	x = PowerMod(y, a, n);
	return x;
}