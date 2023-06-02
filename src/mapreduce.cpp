#include "mapreduce.h"
//incuding header

namespace light_map_reduce

{//from netpacket.h
    MapReduce* instance = nullptr;

    //initialising mapreduce instance
    pthread_mutex_t mutex;

    //using mutex for protecting shared data
    int number_checkin = 0, real_total = -1;


    //checking number,real total
    void cb(header* h, char* data, netcomm* net)
    {
        string input_format, output_format;


        //format of input and output
        vector<int> input_index, finished_indices;


        //finished,input indices
        switch (h->type)
        {//switch case
            case netcomm_type::light_map_reduce_CHECKIN:// light_map_reduce checking


                pthread_mutex_lock(&mutex);//using library function for mutex lock


                ++number_checkin;//checking number
                if (number_checkin % (real_total - 1) == 0)//checking if


                    instance->isready = true;//intsnace is ready


                pthread_mutex_unlock(&mutex);//unlocking mutex lock


                break;
            case netcomm_type::light_map_reduce_CLOSE://check if light_map_reduce is close



                instance->stopflag = true;//stop the instance
                break;
            case netcomm_type::light_map_reduce_ASSIGN_MAPPER://assign the  mapper


                parse_assign_mapper(data, output_format, input_index);//parse assign the mapper


                instance->assign_mapper(output_format, input_index);//assign the instance to mapper

                break;
            case netcomm_type::light_map_reduce_ASSIGN_REDUCER://assign reducer

                
                parse_assign_reducer(data, input_format);//parse assign the reducer


                instance->assign_reducer(input_format);//assign the reducer to the instance

                break;
            case netcomm_type::light_map_reduce_MAPPER_DONE://light_map_reduce mapper is done


                parse_mapper_done(data, finished_indices);//parse the finished mapper


                instance->mapper_done(h->src, finished_indices);//assign the finished mapper to the instance

                break;
            case netcomm_type::light_map_reduce_REDUCER_DONE:


                instance->reducer_done(h->src);//assign the instance of the reducer

                break;

            default:

            //default

                break;
        }
    }

    inline int MapReduce::net_mapper_index(int i)

    {
        return i - 1;

        //decrease by one
    }

    inline int MapReduce::net_reducer_index(int i)
    {
        return i - 1 - spec->num_mappers;

        //decrease by one and also mappers specification
    }

    inline int MapReduce::mapper_net_index(int i)
    {

        return i + 1;

        //increase by one
    }

    inline int MapReduce::reducer_net_index(int i)
    {

        return i + 1 + spec->num_mappers;


        //increase by one and 
    }

    void MapReduce::assign_reducer(const string& input_format)
    {
        char tmp[1024];


        //array of 1024 char
        ReduceInput ri;

        //create a reduceinput of type ri
        Reducer *reducer = CREATE_REDUCER(spec->reducer_class);


        //crete a type of reducer
        for (int i = 0; i < spec->num_inputs; ++i)
        {
            
            
            sprintf(tmp, input_format.c_str(), i);


            //sends formatted output to a string pointed to, by str
            ri.add_file(tmp);



            //add file from tmp
        }

        char tmp2[1024];
          //array of 1024 char
        strcpy(tmp2, spec->output_format.c_str());


        //copy string
        sprintf(tmp, "mkdir -p %s", dirname(tmp2));


        //printing with format
        system(tmp); // make output directory

        string outputfile = spec->output_format;


        //give outputformat 
        sprintf(tmp, outputfile.c_str(), net_reducer_index(index));


        //print output str
        reducer->set_reduceinput(&ri);


         //set reducer input
        reducer->set_outputfile(tmp);


        //
        reducer->reducework();


       //reducer work
        net->send(0, netcomm_type::light_map_reduce_REDUCER_DONE, nullptr, 0);


       //  delete reducer
        delete reducer;
    }

    void MapReduce::reducer_done(int net_index)
    {
        bool finished = false;


         //set finished as false
        pthread_mutex_lock(&mutex);



        //mutex lock

        if (++reducer_finished_cnt == spec->num_reducers)
            //spec num of reducers
            finished = true;


            //set finished to true
        pthread_mutex_unlock(&mutex);


        //unlock mutex

        if (finished)
        {// finished is 1
            fprintf(stderr, "Reduce Time: %fs.\n", duration_cast<duration<double>>(high_resolution_clock::now() - time_cnt).count());


            //duration of reducer 
            fprintf(stderr, "ALL WORK DONE!\n");


            //if work is done
            for (int i = 1; i < real_total; ++i)


                net->send(i, netcomm_type::light_map_reduce_CLOSE, nullptr, 0);//send the packet


            system("rm -rf tmp/");//remove tmp files
            stopflag = true;//stop is true
        }
    }

    void MapReduce::mapper_done(int net_index, const vector<int>& finished_index)
    {
        int job_index = -1;


          //index of job =-1

        pthread_mutex_lock(&mutex);


        if (!jobs.empty())
        {
            job_index = jobs.front();


            //if empty jobs
            jobs.pop();

        }
        mapper_finished_cnt += finished_index.size();//finsihed jobs
        if (mapper_finished_cnt == spec->num_inputs) // all the mappers finished.


        {
            fprintf(stderr, "ALL MAPPER DONE!\n");


            //all mappers finished
            fprintf(stderr, "Map Time: %fs.\n", duration_cast<duration<double>>(high_resolution_clock::now() - time_cnt).count());

            //duration of mappers
            time_cnt = high_resolution_clock::now();


            //time
            for (int i = 0; i < spec->num_reducers; ++i)


                net->send(reducer_net_index(i), netcomm_type::light_map_reduce_ASSIGN_REDUCER,


                        form_assign_reducer(string("tmp/tmp_%d_") + to_string(i) +".txt"));//send packet
        }
        pthread_mutex_unlock(&mutex);//unlock mutex


        if (job_index >= 0)//job index is zero
            net->send(net_index, netcomm_type::light_map_reduce_ASSIGN_MAPPER,
                      form_assign_mapper("tmp/tmp_%d_%d.txt", {job_index}));//send packet
    }


    void MapReduce::assign_mapper(const string& output_format, const vector<int>& input_index)
    {
        char *tmp = new char[spec->input_format.size() + 1024];


        //char array for sending
        MapInput mi;


        //map input type mi




        Mapper *mapper = CREATE_MAPPER(spec->mapper_class);


        //mapper class
        for (int i : input_index)
        {
            sprintf(tmp, spec->input_format.c_str(), i);


            //print also with string format

            mi.add_file(tmp);


            //add file 
        }
        string outputfile = output_format;
        //add output file
        outputfile.replace(outputfile.find("%d"), 2, to_string(input_index[0]));


        //replace output file

        mapper->set_mapinput(&mi);



        //set map input
        mapper->set_numreducer(spec->num_reducers);



        //set number of reducers
        mapper->set_outputfile(outputfile);


        //set output file
        mapper->mapwork();

        net->send(0, netcomm_type::light_map_reduce_MAPPER_DONE, form_mapper_done(input_index));//send packet



        delete mapper;//delete mapper


        delete[] tmp;//delete tmp
    }

    MapReduce::MapReduce(MapReduceSpecification* _spec)
    {
        instance = this;


        //create instance
        setbuf(stdout, nullptr);


        //set buffer
        pthread_mutex_init(&mutex, nullptr);


        //initialse mutex
        set_spec(_spec);


        //set specification
    }

    void MapReduce::set_spec(MapReduceSpecification *_spec)
    {
        if (!_spec) return;


        //if not spec return


        spec = _spec;



        total = spec->num_mappers + spec->num_reducers + 1;


        //total mappers +reducers +1
        if (firstspec)
        {
            firstspec = false;


            //first spec is false
            real_total = total;


            //read total is total
            index = spec->index;


            //index is spec index
            net = new netcomm(spec->config_file, spec->index, cb);


            //netcomm
        }

        if (total > net->gettotalnum())
        {
            fprintf(stderr, "Too many mappers and reducers. Please add workers in configuration file.\n");


            //workers in config file
            exit(1);
        }

        if (spec->num_mappers < 1 || spec->num_reducers < 1)
        {
            fprintf(stderr, "Number of both mappers and reducers must be at least one.\n");


            //numbers of mappers and reducers
            exit(1);
        }
    }

    void MapReduce::start_work()
    {
        printf("Start word from %d.\n", index);


        //word count begins here
        if (!spec)
        {
            fprintf(stderr, "No specification.\n");


            //specification not given
            exit(1);
        }

        reducer_finished_cnt = mapper_finished_cnt = 0;
        //reducer and mapper finished count

        if (index > 0)
        {
            printf("Checkin in %d.\n", index);


            //checkin
            firstrun = false;


            //firstrun is false
            net->send(0, netcomm_type::light_map_reduce_CHECKIN, nullptr, 0);

            //send packet
        }
        else
        {
            printf("Wait for checkin in %d.\n", index);


            //wait for checkin
            if (firstrun && !dist_run_files())


            {
                fprintf(stderr, "distribution error. cannot run workers.\n");

                //distribution error
                net->wait();


                exit(1);
            }
            firstrun = false;

            for (int i = 0; i < spec->num_inputs; ++i)


                jobs.push(i);//add the jobs


            system("rm -rf tmp/ && mkdir tmp");//remove tmp

            while (!isready)


                sleep_us(1000);//sleep 


            isready = false;//isready is false

            printf("All checked in.\n");//all checked in


            time_cnt = high_resolution_clock::now();


            pthread_mutex_lock(&mutex); // protect jobs queue


            for (int i = 0; i < spec->num_mappers; ++i)
            {
                if (jobs.empty())
                    break;
                else
                {
                    net->send(mapper_net_index(i), netcomm_type::light_map_reduce_ASSIGN_MAPPER,


                              form_assign_mapper("tmp/tmp_%d_%d.txt", {jobs.front()}));//send mappers


                    jobs.pop();//remove the job
                }
            }


            pthread_mutex_unlock(&mutex);//unlock mutex
        }
    }

    MapReduce::~MapReduce()//destruct map reduce


    {
        if (net)
        {


            net->wait();//wait the packet


            //delete netwait

            delete net;


        }
        pthread_mutex_destroy(&mutex);//destroy mutex
    }

    int MapReduce::work(MapReduceResult& result)

    {

        time_point<chrono::high_resolution_clock> start = high_resolution_clock::now();


        //use chrono for duration of map-reduce
        start_work();//strat work


        while (!stopflag)
            sleep_us(1000);//sleep


        stopflag = false;//set stopflag to false


        result.timeelapsed = duration_cast<duration<double>>(high_resolution_clock::now() - start).count();//time elapsed of result

        return 0;
    }

    bool run_sshcmd(const string &ip, const string &username, const string &password, const string &cmd)
    {
        ssh_session my_ssh_session;

        //start ssh session
        ssh_channel channel;

        //use ssh channel
        int rc = 0;


        my_ssh_session = ssh_new();


        //create new ssh session
        ssh_options_set(my_ssh_session, SSH_OPTIONS_HOST, ip.c_str());


        //set ssh options as host
        ssh_options_set(my_ssh_session, SSH_OPTIONS_USER, username.c_str());


        //set ssh options as user
        rc = ssh_connect(my_ssh_session);


        //connect ssh
        if (rc != SSH_OK)
        {
            fprintf(stderr, "Error connecting to localhost: %s\n", ssh_get_error(my_ssh_session));

            //ssh error connection to localhost
            ssh_free(my_ssh_session);

            //free ssh
            return false;
        }
        if (password.empty())
            rc = ssh_userauth_publickey_auto(my_ssh_session, nullptr, nullptr);//password empty


        else
            rc = ssh_userauth_password(my_ssh_session, nullptr, password.c_str());//authrorize password


        if (rc != SSH_AUTH_SUCCESS)
        {
            fprintf(stderr, "Authentication failure.\n");//authentication failure


            //disconnect ssh
            ssh_disconnect(my_ssh_session);


            //free ssh
            ssh_free(my_ssh_session);\



            return false;
        }
        channel = ssh_channel_new(my_ssh_session);
        //new channel




        ssh_channel_open_session(channel);
        //open session


        ssh_channel_request_exec(channel, cmd.c_str());


//ssh channel request
        ssh_channel_send_eof(channel);


        //send eof
        ssh_channel_close(channel);
        //ssh close


        ssh_channel_free(channel);


        //ssh free channel

        ssh_disconnect(my_ssh_session);


        //ssh disconnect
        ssh_free(my_ssh_session);
        //ssh free


        return true;
    }

    int getch()
    {
        struct termios oldattr, newattr;


        //termos..h for terminal interface
        int ch;

        tcgetattr(STDIN_FILENO, &oldattr);



        //get attr
        newattr = oldattr;


        newattr.c_lflag &= ~(ICANON | ECHO);//newattr



        tcsetattr(STDIN_FILENO, TCSANOW, &newattr);//set attr



        ch = getchar();

        tcsetattr(STDIN_FILENO, TCSANOW, &oldattr);//tcset attr



        return ch;
    }

    void getpass(string& pass)
    {
        char c;


        //get password
        pass.clear();


        //clear
        printf("please input your password: ");


        while ((c = getch()) != '\n')
            pass.push_back(c);


        putchar('\n');//put password
    }



    bool MapReduce::dist_run_files()
    {
        // get   cwd


        char *tmp = getcwd(nullptr, 0);

        string cwd(tmp), temp, username, password;



        free(tmp);

        // get username and password
        ifstream f(spec->config_file);


        getline(f, temp);
        //getline
        size_t pos = temp.find(':');
        //pos


        username = temp.substr(0, pos);
        //get substr


        password = temp.substr(pos + 1);


        if (!password.empty() && password.back() == '\r')
               //if empty password
            password.pop_back();


        if (password == "*")
           //show password as *
            getpass(password);


        else
            password = base64_decode(password);
            //decode password



        // run workers
        unordered_map<string, vector<pair<int,int>>> um;
        //string vector


        for (int i = 1; i < real_total; ++i)
            um[net->endpoints[i].first].push_back(make_pair(i, net->endpoints[i].second));
            //endpoints

        for (auto &p : um)


        {
            string cmd = "cd " + cwd + " && mkdir -p output";


            //cmd for output
            for (auto &p2 : p.second)


            {
                cmd += " && (./" + spec->program_file + " " + to_string(p2.first) +
                       " > output/output_" + to_string(p2.first) + ".txt 2>&1 &)";//to string


            }
            if (p.first == "127.0.0.1")//localhost


                system(cmd.c_str());//system cmd
            else if (!run_sshcmd(p.first, username, password, cmd))//username,password


                return false;


        }
        return true;

        
    }
}
