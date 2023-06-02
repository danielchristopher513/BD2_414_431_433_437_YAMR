#ifndef light_map_reduce_MAPINPUT_H

#include <vector>
#include <fstream>


#define light_map_reduce_MAPINPUT_H

#include <string>

//map input


namespace light_map_reduce
{
    using namespace std;


    class MapInput
    {


    public:


        void add_file(string filename) { files.push_back(filename); }
//add file

        bool get_next(string &key, string &value);

    private:

   //file index
        int file_index = -1, line_index = 0;

        //vector string
        vector<string> files;


        ifstream f;
    };
}

#endif 
