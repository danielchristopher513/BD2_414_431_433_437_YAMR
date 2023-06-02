#include "reducer.h"
// calling
namespace light_map_reduce
{
    void Reducer::reducework()
    {
        string key, value;

// string key and val
        remove(outputfile_.c_str());


        out_.open(outputfile_);

// open op file
        if (!reduceinput_)
        {
            fprintf(stderr, "no assigned mapinput.\n");

// terminate
            exit(1);
        }
        init();
        
        
        
        // initt
        while (reduceinput_->get_next_key(key))

// loop
            (Reduce(key, reduceinput_));


        combine();

// close out
        out_.close();
    }


// new func
    void Reducer::output(string key, string value)
    {
        if (key.size() > 0){
// size greater than 0

            out_ << key.size() << "\t" << key << "\t" << value << "\n";
        }
    }
}