#include <iostream>

using std :: cerr;

#define DEBUG 0

#if DEBUG == 1

#define DMSG1(A) cerr << A << endl;
#define DMSG2(A) ;

#define DBLOCK1(A) A;
#define DBLOCK2(A) ;

#elif DEBUG == 2

#define DMSG1(A) cerr << A << endl;
#define DMSG2(A) cerr << A << endl;

#define DBLOCK1(A) A;
#define DBLOCK2(A) A;

#else

#define DMSG1(A) ;
#define DMSG2(A) ;

#define DBLOCK1(A) ;
#define DBLOCK2(A) ;

#endif

