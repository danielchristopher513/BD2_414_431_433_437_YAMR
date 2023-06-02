#include "reduceinput.h"

namespace light_map_reduce
{
    void ReduceInput::add_file(std::string filename){

//reduce input 
        int index = fs_.size();

//index
        fs_.push_back(new ifstream());
//push back

        fs_[index]->open(filename);


        while (fs_[index]->good()){


            string line, key, value;
//initialise strings

            getline(*fs_[index], line);

//getline
            if (line.back() == '\r')

//line back
                line.pop_back();


            if (!line.empty()){
//line not empty

                pq_.push(parse_line(line, index));


                return;
            }
        }
        fs_[index]->close();
    }
//close the file
    bool ReduceInput::get_next_key(string &key) {


        if (pq_.empty()){

//empty
            return false;

        } else {
            key  = key_ = get<0>(pq_.top());

//top

            return true;
        }
    }

    bool ReduceInput::get_next_value(string &value)
    //reduce input
    {
        if (pq_.empty()){


            return false;
        }

        auto top = pq_.top();
        //top


        string key = get<0>(top);



        if (key != key_){


            return false;
        }

        pq_.pop();


        value = get<1>(top);


        int index = get<2>(top);
        //get top


        while (fs_[index]->good()){


            string line;


            getline(*fs_[index], line);


            if (line.back() == '\r')

//line back
                line.pop_back();


            if (!line.empty()){
//if not empty line

                pq_.push(parse_line(line, index));


                return true;
            }
        }

        fs_[index]->close();
//close file

        return true;
    }

    tuple<string, string, int> ReduceInput::parse_line(std::string line, int index) {

//init 
        int cur = 0, key_len;
        while (cur < line.size() && isdigit(line[cur])) { cur++; }

//line size


        key_len = stoi(line.substr(0, cur));
//string to integer

        string key = line.substr(cur + 1, key_len);

        string value = line.substr(cur + 2 + key_len);

        //string value


        return make_tuple(key, value, index);
    }



    ReduceInput::~ReduceInput()
//desctructor

    {
        for (auto p : fs_)

        
            delete p;
    }
    //delete p
}



