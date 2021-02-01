#pragma once
#ifdef _DEBUG
#define D(x) x
#else
#define D(x)
#endif
#ifdef D
#include <stdexcept>
#endif