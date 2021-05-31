#include "../ThirdParty/BigInt.hpp"
#include "../Utils/Data.h"
#include "../Circuits/UniversalCircuit.h"

#include "../ThirdParty/httplib.h"
//#include "ThirdParty/json.hpp"
#include "ethsnarks.hpp"
#include "utils.hpp"
#include "import.hpp"
#include "stubs.hpp"
#include <fstream>
#include <chrono>
#include <mutex>

#ifdef MULTICORE
#include <omp.h>
#endif

#include <unistd.h>
#include <ios>
#include <iostream>
#include <fstream>
#include <string>

#include <malloc.h>

using namespace std;
