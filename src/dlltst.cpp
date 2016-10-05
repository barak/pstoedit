#include "pstoedll.h"
#define LEANDYNLOAD
#include "dynload.cpp"
int main (int argc, const char * const* argv){
	cout << "loading " << argv[1] << endl;
	DynLoader l(argv[1]);

	pstoedit_checkversion_func * fpcheck = (pstoedit_checkversion_func*)  l.getSymbol("pstoedit_checkversion");
	fpcheck(pstoeditdllversion);	
	
	pstoedit_plainC_func *fprun = (pstoedit_plainC_func*) l.getSymbol("pstoedit_plainC");
	fprun(argc,argv,0);

////	l.getSymbol("asdasdsad",1);
//	l.getSymbol("_ZN7drvTGIFD0Ev",1);
	cout << "test finished " << endl;
	return 0;
}
