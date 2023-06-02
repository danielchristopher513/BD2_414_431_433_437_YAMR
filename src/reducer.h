#ifndef light_map_reduce_REDUCER_H

#define light_map_reduce_REDUCER_H
// defines
#include <fstream>
// includes
#include <algorithm>

#include <string>

#include <vector>
// vec


#include <set>

#include "reduceinput.h"

// reduce input
#include <map>

#include "register.h"

// register
#include "encoder.h"

namespace light_map_reduce
{
    using namespace std;
// std namespace
    class Reducer
    {
    public:
        virtual void init() {}
          //init
        virtual void Reduce(const string& key, ReduceInput* reduceInput) = 0;
        //virtual func

        virtual ~Reducer(){}

//desctructor of reducer
        virtual void combine() {}
//combine

        void set_reduceinput(ReduceInput* _reduceinput) { reduceinput_ = _reduceinput; }

//set reduce input


        void set_hashfunc(HashFunction _hashfunc) { hashfunc_ = _hashfunc; }
//set hash func

        void set_outputfile(const string& _outputfile) { outputfile_ = _outputfile; }
//set output file

        void set_nummapper(int _num) { num_mapper_ = _num; }

//set mapper
        void reducework();
        //reduce work
        void output(string key, string value);
// private



    private:
        HashFunction hashfunc_ = JSHash;
        ofstream out_;
        int num_mapper_ = 0;
        ReduceInput* reduceinput_ = nullptr;
        string outputfile_;
// of stram
        

        
    };

    BASE_CLASS_REGISTER(Reducer)
// base class
   #define REGISTER_REDUCER(reducer_name) \
        CHILD_CLASS_REGISTER(Reducer,reducer_name)



// defines

    #define CREATE_REDUCER(reducer_name)   \
        CHILD_CLASS_CREATOR(Reducer,reducer_name)
}

#endif  // the end