#include <iostream>

#define DEBUG 2

#if DEBUG == 0
	#define DMSG1(A) ;
	#define DMSG2(A) ;

	#define DBLOCK1(A) ;
	#define DBLOCK2(A) ;
#endif

#if DEBUG >= 1
	#define DMSG1(A) std::cerr << "[1]: " << A << std::endl;
	#define DBLOCK1(A) A;
#endif

#if DEBUG >= 2
	#define DMSG2(A) std::cerr << "[2]: " << A << std::endl;
	#define DBLOCK2(A) A;
#endif

