#include "Common.h"

namespace voyager {

class OverallControl :
    public Identifier {
public:

    int32_t addServer(const char *path, const char *name, int32_t maxConnection = 1);
    int32_t removeServer(const char *path, const char *name);
    int32_t addClient(const char *path, const char *name);
    int32_t removeClient(const char *path, const char *name);
    int32_t addServer(const char *ip, int32_t port, int32_t maxConnection = 1);
    int32_t removeServer(const char *ip, int32_t port);
    int32_t addClient(const char *ip, int32_t port);
    int32_t removeClient(const char *ip, int32_t port);
    void    setLayout(const OverallControlLayout *layout);
    int32_t initLayout();
    void    dump(const char *prefix = "");

public:
    OverallControl();
    virtual ~OverallControl();

private:
    int32_t searchServer(const char *path, const char *name, int32_t &index);
    int32_t searchClient(const char *path, const char *name, int32_t &index);
    int32_t searchServer(const char *ip, int32_t port, int32_t &index);
    int32_t searchClient(const char *ip, int32_t port, int32_t &index);

private:
    OverallControlLayout *mLayout;
};

};

