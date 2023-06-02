#ifndef light_map_reduce_MAPPER_H

#define light_map_reduce_MAPPER_H

#include <vector>
  

#include <chrono>

#include <fstream>

#include <string>

#include <algorithm>

#include "register.h"

#include "mapinput.h"

#include <set>



#include "encoder.h"

namespace light_map_reduce
{//from netpacket.h
    using namespace std;

    class Mapper
    //Mapper class
    {
    public:
        virtual void init() {}
        //init

        virtual void Map(const string& key, const string& value) = 0;
        //map

        virtual void combine() {}
        //combine

        virtual ~Mapper(){}
        //mapper

        void set_mapinput(MapInput* _mapinput) { mapinput = _mapinput; }
        //set mapinput

        void set_hashfunc(HashFunction _hashfunc) { hashfunc = _hashfunc; }
        //hash func

        void set_outputfile(const string& _outputfile) { outputfile = _outputfile; }
        //outputfile

        void set_numreducer(int _num);
        //numreducer

        void mapwork();
        //map work

    protected:
        void emit(string key, string value);
        //emit


    private:
        void output();
        //output

        HashFunction hashfunc = JSHash;
        //jshash

        int num_reducer = 0;

        string outputfile;
        //outputfile

        MapInput* mapinput = nullptr;
        //map input

        vector<multiset<pair<string, string>>> out;

    };

    BASE_CLASS_REGISTER(Mapper)

     #define REGISTER_MAPPER(mapper_name) \
        CHILD_CLASS_REGISTER(Mapper,mapper_name)

    #define CREATE_MAPPER(mapper_name)   \
        CHILD_CLASS_CREATOR(Mapper,mapper_name)
}

#endif 
