#include "mapinput.h"

namespace light_map_reduce

{
    bool MapInput::get_next(string &key, string &value)
    {
        while (true)

        //if good
        {
            while (!f.good())


            {
                if (++file_index >= files.size())
                  //increment file index



                    return false;



                f.close();



                f.open(files[file_index]);
                //open files

            }

            while (getline(f, value))


            {
                if (value.back() == '\r')
                //value back



                    value.pop_back();
                    //pop back

                if (!value.empty())


                {
                    key = files[file_index] + "_" + to_string(line_index++);
                    //output file with index



                    return true;
                }
            }
        }
    }
}




