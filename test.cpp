#include <iostream>
#include <string>
#include <vector>
#include <cassert>

#include "meta_ptr.h"

template<typename T>
void align_check() { static_assert((1 << meta_ptr<T>::meta_bits) == alignof(T), "alignment error"); }

int main()
{
	align_check<char>();
	align_check<unsigned short>();
	align_check<int>();
	align_check<unsigned long>();
	align_check<long long>();
	align_check<float>();
	align_check<double>();
	align_check<long double>();
	align_check<void*>();
	align_check<int*const*>();
	align_check<std::string>();
	align_check<std::vector<int>>();
	align_check<std::vector<std::string>>();

	int _a, _b, _c, _d;
	int *a = &_a, *b = &_b, *c = &_c, *d = &_d;

	meta_ptr<int> ma, mb;
	meta_ptr<int> mc(c);
	meta_ptr<int> md(mc);

	assert(ma.get() == nullptr && mb.get() == nullptr);
	assert(!(bool)ma && !ma);
	assert(!(bool)mb && !mb);
	assert(ma.read() == 0 && mb.read() == 0);
	assert(ma == mb && !(ma != mb));

	assert(mc.get() == c && md.get() == c);
	assert((bool)mc && !!mc);
	assert((bool)md && !!md);
	assert(mc.read() == 0 && md.read() == 0);
	assert(mc == md && !(mc != md));

	mc.write(2);
	md.write(3);
	assert(mc.read() == 2 && md.read() == 3);
	assert(mc.get() == c && md.get() == c);
	assert(mc != md && !(mc == md));

	mc = d;
	md.reset(d);
	assert(mc.read() == 2 && md.read() == 3);
	assert(mc.get() == d && md.get() == d);
	assert(mc != md && !(mc == md));

	ma = mc;
	mb = md;
	assert(ma.get() == mb.get());
	assert(ma.read() == mc.read());
	assert(mb.read() == md.read());
	assert(ma == mc);
	assert(mb == md);
	assert(ma != mb);

	ma.clear();
	assert(ma.read() == 0);
	assert(ma.get() == d);

	assert(mb.read() != 0);
	assert(mb.get() != a);
	mb.reset_clear(a);
	assert(mb.read() == 0);
	assert(mb.get() == a);

	assert(ma.read() == mb.read());
	assert(ma.get() != mb.get());
	assert(ma != mb);

	ma.set<1>();
	assert(!ma.read<0>());
	assert(ma.read<1>());
	assert(ma.read() == 2);
	ma.set<0>();
	assert(ma.read<0>());
	assert(ma.read<1>());
	assert(ma.read() == 3);
	ma.clear<1>();
	assert(ma.read<0>());
	assert(!ma.read<1>());
	assert(ma.read() == 1);
	ma.clear<0>();
	assert(!ma.read<0>());
	assert(!ma.read<1>());
	assert(ma.read() == 0);

	ma.write(0);
	mb.write(1);
	mc.write(2);
	md.write(3);
	assert(ma.read() == 0 && mb.read() == 1 && mc.read() == 2 && md.read() == 3);

	mb.reset(c);
	assert(mb.get() == c && mb.read() == 1);
	assert(mb != mc);

	mb.write(0x125fe);
	assert(mb.get() == c && mb.read() == 2);

	mb.write<0>(true);
	assert(mb.get() == c && mb.read() == 3);

	mb.write<1>(false);
	assert(mb.get() == c && mb.read() == 1);

	mb.write<1>(true);
	assert(mb.get() == c && mb.read() == 3);

	mb.write<0>(false);
	assert(mb.get() == c && mb.read() == 2);

	mb.flip<0>();
	assert(mb.get() == c && mb.read() == 3);

	mb.flip<0>();
	assert(mb.get() == c && mb.read() == 2);

	mb.set();
	assert(mb.get() == c && mb.read() == 3);

	mb.flip();
	assert(mb.get() == c && mb.read() == 0);

	mb.set<1>();
	mb.flip();
	assert(mb.get() == c && mb.read() == 1);

	mb.flip();
	assert(mb.get() == c && mb.read() == 2);

	assert(mb == c);

	assert(mb++.get() == c);
	assert(mb.get() == c + 1);
	assert(mb == c + 1);

	assert(mb--.get() == c + 1);
	assert(mb.get() == c);
	assert(mb == c);

	assert((++mb).get() == c + 1);
	assert(mb.get() == c + 1);
	assert(mb == c + 1);

	assert((--mb).get() == c);
	assert(mb.get() == c);
	assert(mb == c);

	assert((mb += 10) == c + 10);
	assert(mb == c + 10);

	assert((mb -= 20) == c - 10);
	assert(mb == c - 10);

	assert((mb += 10) == c);
	assert(mb == c);

	assert(mb + 10 == c + 10); assert(mb == c);
	assert(10 + mb == c + 10); assert(mb == c);
	assert(10 + mb - 10 == c); assert(mb == c);
	assert(mb - 10 == c - 10); assert(mb == c);

	assert(mb + -10 == c - 10); assert(mb == c);
	assert(-10 + mb + 10 == c); assert(mb == c);
	assert(mb - -10 == c + 10); assert(mb == c);

	ma = a;
	std::cerr << a << ' ' << ma << '\n';

	const char *str = "hello";
	meta_ptr<const char> mcstr = str;
	meta_ptr<char> mstr = const_cast<char*>(str);

	std::cerr << str << ' ' << mcstr << ' ' << mstr << '\n';

	std::cerr << "\n\ntests completed\n";

	std::cin.get();
	return 0;
}
