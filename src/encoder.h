#ifndef light_map_reduce_HASH_H

#include <string> 

#define light_map_reduce_HASH_H

#include <vector>



namespace light_map_reduce
{
    using namespace std;

//hash func

    typedef unsigned int (*HashFunction)(const std::string&);

//jshash


    unsigned int JSHash(const std::string& str);
//unsigned int

}


#endif 
//end if




