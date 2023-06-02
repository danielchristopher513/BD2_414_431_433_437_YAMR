#include "communication_network.h"

namespace light_map_reduce

{
    typedef struct
    {
        char *data;
        //struct

        pcbfun cbfun;

        netcomm *net;
        //netcomm
    } pthread_data;

    void* pthread_cb(void* _data)
    {
        pthread_data *pd = (pthread_data*) _data;
        //pthread
        pd->cbfun((header*)(pd->data), pd->data + sizeof(header), pd->net);
        //cbfun
        delete[] pd->data;
        //delete data 
        delete (char*)_data;


        return nullptr;


    }

    void read_cb(struct bufferevent *bev, void *ctx)
    {
        header h;


        int len = 0;

        //len 0
        netcomm *net = (netcomm*)ctx;
        //netcomm
        struct evbuffer *input = bufferevent_get_input(bev);

        //env buffer
        while (len = evbuffer_get_length(input))
        {
            if (len < sizeof(header))


                break;

            evbuffer_copyout(input, &h, sizeof(header));


            if (len < sizeof(header) + h.length)
                break;


            len = sizeof(header) + h.length;



            char *data = new char[len];


            bufferevent_read(bev, data, len);

            //bufferevent

            if (h.type == netcomm_type::light_map_reduce_HELLO)
            {
                int remote_index = h.src;


                net->net_buffer[remote_index] = bev;


                net->net_um[bev] = remote_index;


                delete[] data;

                //delete data
                fprintf(stderr, "connected from %d\n", h.src);

            }else if (net->cbfun){

                pthread_t ntid;

                pthread_data *pd = new pthread_data;

                pd->data = data;

                pd->cbfun = net->cbfun;


                pd->net = net;


                pthread_create(&ntid, nullptr, pthread_cb, pd);


            }
        }
    }


    void event_cb(struct bufferevent *bev, short events, void *ctx)
    {
        netcomm *net = (netcomm*)ctx;

        //netcomm net
        int remote_index = net->net_um[bev];



        if (events & BEV_EVENT_CONNECTED) {

            fprintf(stderr, "connected to target %d.\n", remote_index);


            header h;


            h.src = net->myindex;


            h.dst = remote_index;



            h.length = 0;


            h.type = netcomm_type::light_map_reduce_HELLO;


            h.version = CURVERSION;
//curversion
            bufferevent_write(bev, &h, sizeof(h));


            net->net_buffer[remote_index] = bev;


            return;
        }

        if (events & BEV_EVENT_EOF) {

            //close sonnection
            fprintf(stderr, "Connection to %d closed.\n", remote_index);


            net->net_um.erase(bev);


            net->net_buffer[remote_index] = nullptr;


            bufferevent_free(bev);




            if (remote_index == 0)


                exit(0);
        }else if (events & BEV_EVENT_ERROR) {


            fprintf(stderr, "Got an error %s, connection to %d closed.\n",
                    evutil_socket_error_to_string(EVUTIL_SOCKET_ERROR()), remote_index);

                    //buffer event
            bufferevent_free(bev);

            exit(1);
        }else{
            fprintf(stderr, "other events???\n");
            //other events
            return;
        }
    }


    void listener_cb(struct evconnlistener *listener, evutil_socket_t fd,
                     struct sockaddr *sa, int socklen, void *ctx)


    {
        netcomm *net = (netcomm*)ctx;
        //netcomm net
        struct bufferevent *bev;


        bev = bufferevent_socket_new(net->net_base, fd, BEV_OPT_CLOSE_ON_FREE | BEV_OPT_THREADSAFE | BEV_OPT_DEFER_CALLBACKS);


        evbuffer_enable_locking(bufferevent_get_output(bev), nullptr);



        bufferevent_setcb(bev, read_cb, nullptr, event_cb, net);

        //buffer setcb
        bufferevent_enable(bev, EV_READ|EV_WRITE);
    }

    void accept_error_cb(struct evconnlistener *listener, void *ctx)
    {
        struct event_base *base = (struct event_base *) ctx;


        int err = EVUTIL_SOCKET_ERROR();

        //eroor
        fprintf(stderr, "Got an error %d (%s) on the listener. "
                        "Shutting down.\n", err, evutil_socket_error_to_string(err));



        exit(1);
    }

    netcomm::netcomm(string configfile, int index, pcbfun f)

    {
        myindex = index;


        cbfun = f;


        readconfig(configfile);

        //readconfig
        net_init();
        //net init
    }

    netcomm::~netcomm()
    {
        for (int i = 0; i < net_buffer.size(); ++i) {


            if (net_buffer[i])


                bufferevent_free(net_buffer[i]);


        }
        evconnlistener_free(listener);


        event_base_loopexit(net_base, nullptr);

        //event
        event_base_free(net_base);
    }

    int netcomm::gettotalnum()
    {


        return net_buffer.size();
    }

    void netcomm::send(int dst, unsigned short type, char *src, int size)
    {
        header h;
        if (!net_buffer[dst])

        //net connect
            net_connect(dst);

            //net connect
        h.src = myindex;

        h.dst = dst;

        //version
        h.version = CURVERSION;

        h.length = size;


        h.type = type;

        struct evbuffer* outputBuffer = bufferevent_get_output(net_buffer[dst]);


        evbuffer_lock(outputBuffer);


        evbuffer_add(outputBuffer, &h, sizeof(header));


        if (size && src)
            evbuffer_add(outputBuffer, src, size);


        evbuffer_unlock(outputBuffer);
    }

    void netcomm::send(int dst, unsigned short type, string data)
    {
        header h;

        //header
        if (!net_buffer[dst])


            net_connect(dst);


        h.src = myindex;


        h.dst = dst;


        h.version = CURVERSION;


        h.length = data.size() + 1;


        h.type = type;

        struct evbuffer* outputBuffer = bufferevent_get_output(net_buffer[dst]);


        evbuffer_lock(outputBuffer);


        evbuffer_add(outputBuffer, &h, sizeof(header));


        if (data.size())


            evbuffer_add(outputBuffer, data.c_str(), data.size() + 1);


        evbuffer_unlock(outputBuffer);
    }

    void netcomm::wait()
    {
        if (myindex == 0)
        {
            for (int i = 1; i < net_buffer.size(); ++i)
            {
                while (net_buffer[i])

                //sleep
                    sleep_us(1000);
            }
        }
    }

    string hostname_to_ip(string hostname)
    {
        char ip[100];
//ip addr

        struct hostent *he;


        struct in_addr **addr_list;

        he = gethostbyname(hostname.c_str());

        //gethostbyName
        addr_list = (struct in_addr **)he->h_addr_list;


        if (addr_list[0])
        {
            strcpy(ip, inet_ntoa(*addr_list[0]));

            return ip;
        }else
        {
            fprintf(stderr, "cannot resolve %s", hostname.c_str());


            return "";
        }
    }

    void netcomm::readconfig(string &configfile)
    {
        ifstream f(configfile);

        uint16_t port;


        string ip;


        getline(f, ip);  


        while (f >> ip >> port)

        {
            if (inet_addr(ip.c_str()) == INADDR_NONE)


            {
                ip = hostname_to_ip(ip);

                //hostname
                if (ip.empty())


                    continue;
            }
            endpoints.emplace_back(make_pair(ip, port));


        }
        net_buffer.resize(endpoints.size());


        if (endpoints.empty())
        {
            fprintf(stderr, "invalid endpoints configure file.\n");

            //error
            exit(1);
        }
    }

    void netcomm::net_connect(int index)
    {
        struct bufferevent *bev;


        struct sockaddr_in sin;



        if (net_buffer[index])
            return;

        memset((char*)&sin, 0, sizeof(sin));


        sin.sin_family = AF_INET;
//ip v4

        sin.sin_addr.s_addr = inet_addr(endpoints[index].first.c_str());


        sin.sin_port = htons(endpoints[index].second);



        bev = bufferevent_socket_new(net_base, -1, BEV_OPT_CLOSE_ON_FREE | BEV_OPT_THREADSAFE |BEV_OPT_DEFER_CALLBACKS);


        evbuffer_enable_locking(bufferevent_get_output(bev), nullptr);



        net_um[bev] = index;




        bufferevent_enable(bev, EV_READ|EV_WRITE);


        bufferevent_setcb(bev, read_cb, nullptr, event_cb, this);


        bufferevent_socket_connect(bev, (struct sockaddr *)&sin, sizeof(sin));



        while (!net_buffer[index])

        //sleep
            sleep_us(1000);
    }

    void netcomm::net_init()
    {
        pthread_t ntid;
//pthread

        struct sockaddr_in sin;



        evthread_use_pthreads();


        net_base = event_base_new();



        memset(&sin, 0, sizeof(sin));


        sin.sin_family = AF_INET;


        sin.sin_port = htons(endpoints[myindex].second);



        listener = evconnlistener_new_bind(net_base, listener_cb, this,
                                           LEV_OPT_REUSEABLE|LEV_OPT_CLOSE_ON_FREE|LEV_OPT_THREADSAFE, -1,
                                           (struct sockaddr*)&sin, sizeof(sin));

        if(!listener)
        {

            perror("could't not create listener");

            //error
            exit(1);
        }
        evconnlistener_set_error_cb(listener, accept_error_cb);



        pthread_create (&ntid, nullptr, [] (void *arg) -> void * {
                            event_base_dispatch((struct event_base *)arg);
                            return nullptr;
                        }, net_base);

                        //create pthread
    }

}






