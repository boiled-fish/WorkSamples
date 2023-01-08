#include <NTL/ZZX.h>
#include "course_design.h"

using namespace std;
using namespace NTL;

long witness(const ZZ&, const ZZ&);
long prime_test(const ZZ& n, long t)
{
	if (n <= 1) 
		return 0;

	ZZ x;
	long i;

	for (i = 0; i < t; i++) {
		x = RandomBnd(n); // random number between 0 and n-1

		if (witness(n, x))
			return 0;
	}

	return 1;
}
long witness(const ZZ& n, const ZZ& x)
{
	ZZ m, y, z;
	long j, k;

	if (x == 0) return 0;

	// compute m, k such that n-1 = 2^k * m, m odd:

	k = 1;
	m = n / 2;
	while (m % 2 == 0) {
		k++;
		m /= 2;
	}

	z = PowerMod(x, m, n); // z = x^m % n
	if (z == 1) return 0;

	j = 0;
	do {
		y = z;
		z = (y * y) % n;
		j++;
	} while (j < k && z != 1);

	return z != 1 || y != n - 1;
}