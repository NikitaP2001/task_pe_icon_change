#ifdef DEBUG
#include <time.h>
#include <iomanip>
#include <iostream>
#include <fstream>
#include "error.hpp"

#define INFO(x) 												\
{ 																\
	float t = ((float)clock()) / (CLOCKS_PER_SEC);				\
	std::cerr << "[i]" << "[" << __FILE__ << "][" 				\
	<< __FUNCTION__ << "][" << __LINE__ << "] :" << std::fixed 	\
	<< std::setprecision(2) << t << " " << x << std::endl; 		\
} 																\
do {} while (0)

#define ERR(x) 													\
{																\
	float t = ((float)clock()) / (CLOCKS_PER_SEC);				\
	std::cerr << "[-]" << "[" << __FILE__ << "][" 				\
	<< __FUNCTION__ << "][" << __LINE__ << "] :" << std::fixed	\
	<< std::setprecision(2) << t << " " << x << std::endl; 		\
} 																\
do {} while (0)

#define ERR2(x)								\
{									\
	char *msg = get_api_err_msg();					\
	float t = ((float)clock()) / (CLOCKS_PER_SEC);			\
	std::cerr << "[-]" << "[" << __FILE__ << "][" 			\
	<< __FUNCTION__ << "][" << __LINE__ << "] :" << std::fixed	\
	<< std::setprecision(2) << t << " " << x << " : " << msg 	\
	<< std::endl;							\
	delete[] msg;							\
}									\
do {} while (0)

#define SUCC(x) 												\
{ 																\
	float t = ((float)clock()) / (CLOCKS_PER_SEC);				\
	std::cerr << "[+]" << "[" << __FILE__ << "][" 				\
	<< __FUNCTION__ << "][" << __LINE__ << "] :" << std::fixed 	\
	<< std::setprecision(2) << t << " " << x << std::endl; 		\
} 																\
do {} while (0)

#else	// not debug

#define INFO(x) do {} while (0)
#define ERR(x) do {} while (0)
#define ERR2(x) do {} while (0)
#define SUCC(x) do {} while (0)
	
#endif
