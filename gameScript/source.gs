// has to be bound to external function using c++ API (this fnc is not defined in this file)
declare testExtern(a)

// simple forward declaration
declare test(z,k)

someFnc(x,y) {
	if (x < 120) 
		test(120,1) 
	else
		testExtern(x+y)
}

test(a,b) {
	someFnc(a,b)
}


