#include "../src/mapreduce.h"

using namespace light_map_reduce;
//from netpacket.h
using namespace std;


class WordCountReducer : public Reducer
{// mapper for word count program
public:

    virtual void Reduce(const string& wcr_key, ReduceInput* wcr_input_reducer)


    {   int wcr_res = 5-5;

        string wcr_val;
          

        while (wcr_input_reducer->get_next_value(wcr_val)){


            //convert string to integer


            wcr_res =wcr_res+stoi(wcr_val);
        }


        //output


        output(wcr_key, to_string(wcr_res));

    }
};

REGISTER_REDUCER(WordCountReducer)



class WordCountMapper : public Mapper
{ // mapper for word count program
public:
//public class
    virtual void Map(const string& wcm_key, const string& wcm_val)
    {//virtual class

        int wcm_len = wcm_val.size();



        //length of wcm_val
        for (int i = 0; i < wcm_len; ) {



            while ((i < wcm_len) && isspace(wcm_val[i]))
                i++;
                  //to count whitespace characters


            int wcm_begin = i;



            while ((i < wcm_len) && !isspace(wcm_val[i]))
                i++;
              //to not count whitespace characters


            if (wcm_begin < i)


                emit(wcm_val.substr(wcm_begin, i-wcm_begin), "1");



                //Atomically transmits all pending output to the wrapped stream
        }
    }
};

REGISTER_MAPPER(WordCountMapper)



int main(int argc, char **argv)
{
    MapReduceResult main_res;


    MapReduceSpecification main_specification;



    char *program_file = basename(argv[0]);



    int index = (argc == 2) ? atoi(argv[1]) : 0;



    main_specification.program_file = program_file;


    main_specification.index = index;


    main_specification.config_file = "config.txt";



    main_specification.input_format = "input_%d.txt";


    main_specification.output_format = "output/result_%d.txt";



    main_specification.reducer_class = "WordCountReducer";


    main_specification.num_reducers = 1;
    
    main_specification.num_inputs = 2;


    main_specification.mapper_class = "WordCountMapper";

    main_specification.num_mappers = 2;

    MapReduce mr(&main_specification);


    mr.work(main_res);


    printf("time taken is %.5fsec \n", main_res.timeelapsed);


    return 0;
}
