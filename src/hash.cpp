#include "encoder.h"

#include <vector>

namespace light_map_reduce

{//from netpacket.h

    unsigned int JSHash(const string& str)
    {//js hash function


    
        unsigned int hash_function = 1315423911;


        
        //hashing
        for (char d : str) {


        
            hash_function =hash_function ^ ((hash_function << 5) +  (hash_function >> 2) + d);//hash function
            
            
        }
        
        


        return hash_function;//return hash
    }
}
