#include <sys/socket.h>
#include <sys/un.h>
#include <poll.h>

#include "common.h"
#include "configuration.h"
#include "socket_server.h"

namespace voyager {

int32_t start_server(int32_t *socketfd, const char *socketName)
{
    struct sockaddr_un server_addr;
    socklen_t addr_len;
    int sockfd = -1;
    int option = 1;
    mode_t permission = 0;
    int32_t rc = NO_ERROR;

    if (SUCCEED(rc)) {
        rc = access(SERVER_SOCKET_PATH, R_OK | W_OK);
        if (FAILED(rc)) {
            LOGI(MODULE_SOCKET, "Socket dir %s does not exist, create it.", DUMP_PATH);
            unlink(SERVER_SOCKET_PATH);
            rc = mkdir(SERVER_SOCKET_PATH, 0755);
            if (FAILED(rc)) {
                LOGE(mModule, "mkdir %s failed, %s.",
                    SERVER_SOCKET_PATH, strerror(errno));
                rc = UNKNOWN_ERROR;
            }
        }
    }

    if (SUCCEED(rc)) {
        sockfd = socket(AF_UNIX, SOCK_STREAM, 0);
        if (sockfd < 0) {
            LOGE(MODULE_SOCKET,
                "Failed to open listening socket for server, %s",
                strerror(errno));
            rc = SYS_ERROR;
        }
    }

    if (SUCCEED(rc)) {
        memset((unsigned char *)&server_addr, 0, sizeof(server_addr));
        server_addr.sun_family = AF_UNIX;
        strcpy(server_addr.sun_path, SERVER_SOCKET_PATH);
        strcat(server_addr.sun_path, socketName);
        addr_len = strlen(server_addr.sun_path) + sizeof(server_addr.sun_family);
        LOGD(MODULE_SOCKET,
            "Connection socket name %s", server_addr.sun_path);
        if (!access(server_addr.sun_path, F_OK)) {
            rc = unlink(server_addr.sun_path);
            if (FAILED(rc)) {
                LOGE(MODULE_SOCKET, "Unlink failed, %s", strerror(errno));
                rc = SYS_ERROR;
            }
        }
    }

    if (SUCCEED(rc)) {
        //reuse in case it is in timeout
        option = 1;
        rc = setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR,
            &option, sizeof(option));
        if (FAILED(rc)) {
            LOGE(MODULE_SOCKET,
                "Can't set SO_REUSEADDR for server socket, %s", strerror(errno));
            rc = SYS_ERROR;
        }
    }

    if (SUCCEED(rc)) {
        rc = bind(sockfd, (struct sockaddr *)&server_addr, addr_len);
        if (FAILED(rc)) {
            LOGE(MODULE_SOCKET,
                "Failed to bind for server socket, %s", strerror(errno));
            rc = SYS_ERROR;
        }
    }

    if (SUCCEED(rc)) {
        struct stat status;
        rc = stat(server_addr.sun_path, &status);
        if (FAILED(rc)) {
            LOGE(MODULE_SOCKET, "Failed to stat, %s", strerror(errno));
            rc = SYS_ERROR;
        } else {
            permission = status.st_mode;
        }
    }

    if (SUCCEED(rc)) {
        rc = chmod(server_addr.sun_path, permission | S_IROTH | S_IWOTH);
        if (FAILED(rc)) {
            LOGE(MODULE_SOCKET,"Failed to add permission, %s",
                strerror(errno));
            rc = SYS_ERROR;
        }
    }

    if (SUCCEED(rc)) {
        rc = listen(sockfd, MAX_CLIENT_ALLOWED);
        if (FAILED(rc)) {
            LOGE(MODULE_SOCKET, "Failed to listen, %s", strerror(errno));
            rc = SYS_ERROR;
        } else {
            LOGD(MODULE_SOCKET,
                "Listen client connection on fd %d", sockfd);
            *socketfd = sockfd;
        }
    }

    if (FAILED(rc)) {
        if (sockfd != -1) {
            close(sockfd);
        }
    }

    return rc;

}

int32_t poll_accept(int32_t sockfd, int32_t *clientfd)
{
    int32_t rc = NO_ERROR;
    int32_t connected_client_fd = -1;
    struct sockaddr_un client_addr;
    socklen_t client_addr_len;
    struct pollfd connected_pollfd;

    *clientfd = -1;
    connected_pollfd.fd = sockfd;
    connected_pollfd.events = POLLIN | POLLPRI;
    rc = poll(&connected_pollfd, 1, SERVER_PULL_CLIENT_TIMEOUT);
    if (rc == -1) {
        LOGE(MODULE_SOCKET,
            "Server poll client error, %s", strerror(errno));
        rc = SYS_ERROR;
    } else if (rc == 0) {
        // Server poll client timeout, check if need to exit outside
        rc = TIMEDOUT;
    } else {
        if (connected_pollfd.revents & POLLPRI) {
            LOGE(MODULE_SOCKET, "Server poll client return, POLLPRI");
            rc = UNKNOWN_ERROR;
        } else if (connected_pollfd.revents & POLLIN) {
            client_addr_len = sizeof(client_addr);
            connected_client_fd = accept(sockfd,
                (struct sockaddr *)&client_addr, &client_addr_len);
            if (connected_client_fd == -1) {
                LOGE(MODULE_SOCKET,
                    "Client connect failed to server poll, %s", strerror(errno));
                rc = SYS_ERROR;
            } else {
                *clientfd = connected_client_fd;
                LOGI(MODULE_SOCKET,
                    "Client connected to server, fd %d", *clientfd);
                rc = NO_ERROR;
            }
        }
    }

    return rc;
}

int32_t poll_read(int32_t clientfd,
    char *dat, int32_t max_len, int32_t *read_len)
{
    int32_t rc = NO_ERROR;
    struct pollfd connected_pollfd;
    connected_pollfd.fd = clientfd;
    connected_pollfd.events = POLLIN | POLLPRI;

    rc = poll(&connected_pollfd, 1, SERVER_POLL_DATA_TIMEOUT);
    if (rc == -1) {
        LOGE(MODULE_SOCKET,
            "Server poll data error, %s", strerror(errno));
    } else if (rc == 0) {
        // Server poll data timeout, check if need to exit outside
        rc = TIMEDOUT;
    } else {
        if (connected_pollfd.revents & POLLHUP) {
            LOGE(MODULE_SOCKET, "Server poll data return, POLLHUP");
            rc = UNKNOWN_ERROR;
        } else if (connected_pollfd.revents & POLLPRI) {
            LOGE(MODULE_SOCKET, "Server poll data return, POLLPRI");
            rc = UNKNOWN_ERROR;
        } else if (connected_pollfd.revents & POLLIN) {
            rc = sc_read_data(clientfd, dat, max_len, read_len);
            if (FAILED(rc)) {
                LOGE(MODULE_SOCKET,
                    "Server failed to read client data, %d", rc);
            }
        }
    }

    return rc;
}

int32_t disconnect_client(int32_t clientfd)
{
    shutdown(clientfd, SHUT_RDWR);
    close(clientfd);

    return NO_ERROR;
}

int32_t stop_server(int32_t sockfd, const char *socketName)
{
    char path[PATH_MAX];

    shutdown(sockfd, SHUT_RDWR);
    close(sockfd);

    strcpy(path, SERVER_SOCKET_PATH);
    strcat(path, socketName);
    if (!access(path, F_OK)) {
        unlink(path);
    }

    return NO_ERROR;
}

int32_t poll_accept_wait(int32_t sockfd,
    int32_t *clientfd, bool *cancel)
{
    int32_t rc = NO_ERROR;

    *clientfd = -1;
    do {
        rc = poll_accept(sockfd, clientfd);
        if (SUCCEED(rc)) {
            LOGI(MODULE_SOCKET, "Accepted client with fd %d", *clientfd);
            break;
        } else if (rc != TIMEDOUT) {
            LOGE(MODULE_SOCKET, "Failed to accept client, %d", rc);
            break;
        }
        if (*cancel) {
            LOGI(MODULE_SOCKET, "Cancelled to wait for connection");
            rc = USER_ABORTED;
            break;
        }
    } while (true);

    return rc;
}

int32_t poll_read_wait(int32_t clientfd,
    char *dat, int32_t max_len, int32_t *read_len, bool *cancel)
{
    int32_t rc = NO_ERROR;

    do {
        if (*cancel) {
            LOGI(MODULE_SOCKET, "Cancelled to wait for message");
            rc = USER_ABORTED;
            break;
        }
        rc = poll_read(clientfd, dat, max_len, read_len);
        if (SUCCEED(rc)) {
            break;
        } else if (rc != TIMEDOUT) {
            LOGE(MODULE_SOCKET,
                "Failed to poll data from client, %d", rc);
            break;
        }
    } while (true);

    return rc;
};

};

