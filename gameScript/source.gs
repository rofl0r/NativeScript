// has to be bound to external function using c++ API (this fnc is not defined in this file)
declare testExtern(a)

test(a,b) {
	x = a;
	a = testExtern(b);
	return if (a) b else a;
}


