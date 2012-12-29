#include <iostream>

#define DEBUG 1

#define DMSG1(A) 
#define DMSG2(A) 

#define DBLOCK1(A) 
#define DBLOCK2(A) 

#if DEBUG >= 1
	#undef DMSG1
	#undef DBLOCK1

	#define DMSG1(A) std::cerr << "[1]: " << A << std::endl;
	#define DBLOCK1(A) A;
#endif

#if DEBUG >= 2
	#undef DMSG2
	#undef DBLOCK2

	#define DMSG2(A) std::cerr << "[2]: " << A << std::endl;
	#define DBLOCK2(A) A;
#endif

