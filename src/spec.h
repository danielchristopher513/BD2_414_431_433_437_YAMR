#ifndef light_map_reduce_SPEC_H

// map input addedd
#include "mapinput.h"


#define light_map_reduce_SPEC_H

// string
#include <string>

// encoder
#include "encoder.h"

// using namespace
namespace light_map_reduce
{
    using namespace std;

    typedef struct
    {
        // jHash
        HashFunction hashfunc = JSHash;
// number of input

        int num_inputs = 0;int num_mappers = 0; int num_reducers = 0, index = 0;


        // string input
        string input_format, output_format;string mapper_class, reducer_class, config_file;string program_file;

// done
    } MapReduceSpecification;
}

// endif
#endif