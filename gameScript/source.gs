// has to be bound to external function using c++ API (this fnc is not defined in this file)
declare testExtern()

test() {
	return 1;
}

declare c(a) 
f() {
	return for(i=0,i<10000000,1) c(i);
}

// todo: vector syntax: x[3] (implemented as 3 simple variables)?
// todo: capability of receiving the input params changes on c++ side
