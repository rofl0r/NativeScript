//#include <glib/glib.h>
#include <mono/jit/jit.h>
#include <mono/metadata/assembly.h>

MonoDomain *domain;



int main(int argc, char* argv[]) {
	//return simple();
	//return game();
	domain = mono_jit_init("scriptingDomain");
}
