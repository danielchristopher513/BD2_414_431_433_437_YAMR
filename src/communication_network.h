#ifndef light_map_reduce_NETCOMM_H

#define light_map_reduce_NETCOMM_H

#include <string>


#include <fstream>

#include <memory>

#include <unordered_map>


#include "event2/event.h"


#include "event2/listener.h"


#include "event2/util.h"


#include <arpa/inet.h>


#include "event2/thread.h"


#include "event2/bufferevent.h"



#include "event2/buffer.h"


#include <vector>


#include <cstring>


#include "general.h"

namespace light_map_reduce
{
    class netcomm;

    using namespace std;

    const short CURVERSION = 1;

    typedef struct{
        unsigned short version, type;
        unsigned int length, src, dst;
    } header;

    typedef void (*pcbfun)(header*, char*, netcomm*);

    enum netcomm_type {
        light_map_reduce_HELLO,
                // hello message
        
        light_map_reduce_MAPPER_DONE,  
               // finished_indices
        light_map_reduce_ASSIGN_REDUCER,  
            // assign input format(_%d_(fixed))
        light_map_reduce_REDUCER_DONE,  

              // DONE
        light_map_reduce_CHECKIN,   
            // checkin
        light_map_reduce_CLOSE,  
                     // close immediately
        light_map_reduce_ASSIGN_MAPPER,    
           // output format (_%d_%d), input_indices
    };

    class netcomm
    {   friend void event_cb(struct bufferevent *bev, short events, void *ctx);

        friend void read_cb(struct bufferevent *bev, void *ctx);

        friend void listener_cb(struct evconnlistener *listener, evutil_socket_t fd,
                                struct sockaddr *sa, int socklen, void *ctx);

    public:
        netcomm(string configfile, int index, pcbfun f);

        ~netcomm();

        void send(int dst, unsigned short type, string data);

        void send(int dst, unsigned short type, char *src, int size);

        void wait();

        int gettotalnum();

        vector<pair<string, uint16_t>> endpoints;

    private:
        void net_init();

        void net_connect(int index);


        void readconfig(string &configfile);


        vector<struct bufferevent*> net_buffer;
        


        struct evconnlistener *listener = nullptr;


        int myindex;

        unordered_map<struct bufferevent*, int> net_um;

        struct event_base *net_base = nullptr;
        pcbfun cbfun;
    };

}

#endif 
