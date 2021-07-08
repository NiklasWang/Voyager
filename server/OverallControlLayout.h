#include <stdint.h>

#define IP_MAX_LEN             15
#define MAX_SERVER_CLIENT_PAIR 1024
#define LOCAL_SOCKET_PATH_LEN  256
#define LOCAL_SOCKET_NAME_LEN  128   

namespace voyager {

struct OverallControlLayout {
    struct Sockets {
        struct Server {
            enum Type {
                SERVER_TYPE_IP,
                SERVER_TYPE_LOCAL,
                SERVER_TYPE_MAX_INVALID
            } type;
            union Address {
                struct Ip {
                    char ip[IP_MAX_LEN];
                    int32_t port;
                } ip;
                struct Local {
                    char path[LOCAL_SOCKET_PATH_LEN];
                    char name[LOCAL_SOCKET_NAME_LEN];
                } local;
            } address;
            int32_t currentConnections;
            int32_t maxConnection;
        } server[MAX_SERVER_CLIENT_PAIR + 1];
        struct Client {
            enum Type {
                CLIENT_TYPE_IP,
                CLIENT_TYPE_LOCAL,
                CLIENT_TYPE_MAX_INVALID
            } type;
            union Address {
                struct Ip {
                    char ip[IP_MAX_LEN];
                    int32_t port;
                } ip;
                struct Local {
                    char path[LOCAL_SOCKET_PATH_LEN];
                    char name[LOCAL_SOCKET_NAME_LEN];
                } local;
            } connect;
        } client[MAX_SERVER_CLIENT_PAIR];
    } sockets;
};

};

