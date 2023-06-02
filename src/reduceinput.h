#ifndef light_map_reduce_REDUCEINPUT_H
// def define

#include <queue>

// quee
#include <vector>

// vec
#include <fstream>

// fstr

#define light_map_reduce_REDUCEINPUT_H

#include <string>


// str

#include <tuple> //tupe

namespace light_map_reduce
{
    using namespace std;
// std
    class ReduceInput
    {
    public:
        void add_file(string filename);

// add file
        bool get_next_key(string &key);
// nex key

        bool get_next_value(string &value);

// nex val
        ~ReduceInput();
// dest
    private:
        tuple<string, string, int> parse_line(string line, int index);

// pric str
        string key_;

// priv key
        vector<ifstream*> fs_;

// fs
        priority_queue<tuple<string, string, int>, vector<tuple<string, string, int> >, greater<tuple<string, string, int> > > pq_;

// pio quee
    };
}

#endif 





// end