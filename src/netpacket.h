#ifndef light_map_reduce_NETPACKET_H

//macros
#define light_map_reduce_NETPACKET_H

/* this file is for communication using a tcp-ip connection*/



//headers used
#include <string>

#include <vector>
//socketstream header file


#include <sstream>


//map-reduce namespace 
namespace light_map_reduce
{
    using namespace std;

    inline string form_assign_mapper(string output_format, vector<int> input_indices)
    // for assigning mapper
    {
        for (int i : input_indices)
        {
            output_format.push_back('\n');


            //next line in file
            output_format += to_string(i);


            //format for mapper
        }
        //return format
        return output_format;
    }

    inline void parse_assign_mapper(char* data, string& output_format, vector<int>& input_indices)
    {//parse-assign mapper


        istringstream is(data);

        int i;
        //int i

        input_indices.clear();
         //indices

        //use the output-format
        is >> output_format;

        
        while (is >> i)
            input_indices.push_back(i);
    }

    inline string form_assign_reducer(string input_format)
    {
        
        
        // assign reducer
        return input_format;
    }

    inline void parse_assign_reducer(char* data, string& input_format)
    
    {//parse-assign-reducer
        
        
        istringstream is(data);

        //isstring
        
        is >> input_format;
    }

    inline string form_mapper_done(vector<int> finished_indices)
    {
        
        
        string tmp;

        
        for (int i : finished_indices)
        {//finished indces
            
            
            tmp += to_string(i);
            //to string
            
            
            tmp.push_back('\n');
        }
        tmp.pop_back();
     //pop back

        return tmp;
    }

    inline void parse_mapper_done(char* data, vector<int>& finished_indices)
    {
        istringstream is(data);//isstringstream


        int i;

       ///int i

        finished_indices.clear();//finished indices


        while (is >> i)
            finished_indices.push_back(i);

            //finished indices
    }
}

#endif 
