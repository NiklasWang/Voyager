#ifndef _ION_OPERATOR_H__
#define _ION_OPERATOR_H__

namespace voyager {

typedef int (*ion_open)();
typedef int (*ion_close)(int fd);
typedef int (*ion_alloc)(int fd, size_t len, size_t align, unsigned int heap_mask,
              unsigned int flags, ion_user_handle_t *handle);
typedef int (*ion_alloc_fd)(int fd, size_t len, size_t align, unsigned int heap_mask,
              unsigned int flags, int *handle_fd);
typedef int (*ion_sync_fd)(int fd, int handle_fd);
typedef int (*ion_free)(int fd, ion_user_handle_t handle);
typedef int (*ion_map)(int fd, ion_user_handle_t handle, size_t length, int prot,
            int flags, off_t offset, unsigned char **ptr, int *map_fd);
typedef int (*ion_share)(int fd, ion_user_handle_t handle, int *share_fd);
typedef int (*ion_import)(int fd, int share_fd, ion_user_handle_t *handle);


struct IonOperator :
    public Identifier {
public:

    ion_open     ion_open_func;
    ion_close    ion_close_func;
    ion_alloc    ion_alloc_func;
    ion_alloc_fd ion_alloc_fd_func;
    ion_sync_fd  ion_sync_fd_func;
    ion_free     ion_free_func;
    ion_map      ion_map_func;
    ion_share    ion_share_func;
    ion_import   ion_import_func;

public:
    int32_t init();
    bool    inited();
    int32_t deinit();

public:
    IonBufferMgr();
    virtual ~IonBufferMgr();

private:
    void *mDlHandler;
};

};

#endif

