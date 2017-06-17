#pragma once

#define _STATIC_LIB

#if defined(_STATIC_LIB)


#elif defined(_DYNAMIC_LIB)  


#else
#error Not Defined linking type
#endif