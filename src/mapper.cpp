#include "mapper.h"

namespace light_map_reduce


{
    void Mapper::set_numreducer(int _num)
    {


//number of reducer

        num_reducer = _num;

//clear
        out.clear();


        out.resize(_num);
        //resize
    }

    void Mapper::emit(string key, string value)
    {

        if (num_reducer == 1)


            out[0].insert(make_pair(key, value));
            //make pair
        else


            out[hashfunc(key) % num_reducer].insert(make_pair(key, value));
            //hash func
    }

    void Mapper::mapwork()
    {
        auto start = chrono::high_resolution_clock::now();
        //time calculation


        string key, value;
//strings
        if (!mapinput)


        {
            fprintf(stderr, "Map input was not assigned \n");

//error
            exit(1);
        }
        init();
        //init
        while (mapinput->get_next(key, value))


            Map(key, value);
            //map key,value


        combine();
        //combine

        output();


        fprintf(stderr, "Time taken to compute Map: %f.\n", chrono::duration_cast<chrono::duration<double>>(chrono::high_resolution_clock::now() - start).count());
    }

    void Mapper::output()
    {
        ofstream f;
         //tmp

        char *tmp = new char[100 + 2 * outputfile.size()];


        for (int i = 0; i < num_reducer; ++i)
        {
            sprintf(tmp, outputfile.c_str(), i);
            //print with output format

            f.open(tmp);


            for (auto &j : out[i])
            {


                f << j.first.size() << "\t" << j.first << "\t" << j.second << "\n";
                //size of first
            }

            f.close();






        }
        delete[] tmp;//delete tmp
    }
}
