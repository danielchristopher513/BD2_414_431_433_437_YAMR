#ifndef light_map_reduce_MAPREDUCE_H

#define light_map_reduce_MAPREDUCE_H

#include <unordered_map>

#include <vector>

#include <queue>

#include "unistd.h"

#include <chrono>

#include <termios.h>

#include "libgen.h"

#include <libssh/libssh.h>


#include "spec.h"


#include "communication_network.h"


#include "netpacket.h"


#include "mapper.h"

#include "reducer.h"

namespace light_map_reduce
{//from netpacket.h
    using namespace std;

    using namespace std::chrono;
    //Chrono library is used to deal with date and time

    typedef struct
    {//struct for timeelapsed
        double timeelapsed;


    } MapReduceResult;

    class MapReduce
    {
        friend void cb(header* h, char* data, netcomm* net);


        //friend function
    public:
        MapReduce(MapReduceSpecification* _spec = nullptr);


        //map reduce constructor
        ~MapReduce();
       //map reduce destructor
        void set_spec(MapReduceSpecification* _spec);


     //setting specification to MapReduceSpecification
        int work(MapReduceResult& result);


       //worker nodes
        MapReduceSpecification* get_spec() { return spec; }


        //get MapReduceSpecification 

    private:
        bool dist_run_files();


        //run all the files
        void start_work();


        //start workers
        inline int net_mapper_index(int i);


         //index of net mapper
        inline int net_reducer_index(int i);



          //index of net reducer
        inline int mapper_net_index(int i);



             //index of mapper net
        inline int reducer_net_index(int i);


            //index of reducer net
        void assign_mapper(const string& output_format, const vector<int>& input_index);


        //asign mapper
        void mapper_done(int net_index, const vector<int>& finished_index);


        //mapper done 
        void assign_reducer(const string& input_format);


        //asign reducer
        void reducer_done(int net_index);


        //reducer done

        bool stopflag = false, isready = false, firstrun = true, firstspec = true;


        //stop,isready,firstrun,furst spec flags are initialised


        int index, total, mapper_finished_cnt = 0, reducer_finished_cnt = 0;


        //setting index,total,mapper,finished count ,reducer finished count to zero
        MapReduceSpecification* spec = nullptr;


        //setting mapreduce specification to null


        time_point<chrono::high_resolution_clock> time_cnt;


        //using  chrono library to get time elapsed
        netcomm *net = nullptr;


        //initialling netcomm for tcp/ip connection
        queue<int> jobs;


        ///using a queue for jobs
    };
}
//ending the if statement
#endif 
