#include <iostream> // instruction to compiler include the standard Stream I/O
#include "uv.h" // include the libuv library
#include <unistd.h> // include the unistd library

using namespace std; // makes names from std visible without std::

typedef struct {
    uv_fs_t req;
    uv_buf_t buf;
    uv_loop_t *loop;
    void (*callback)(const char*);
} file_t;

file_t* file;

#define BUF_SIZE 1024

void on_open(uv_fs_t* open_req);

void on_close(uv_fs_t* req);

void on_read(uv_fs_t* req);

void readfile(uv_loop_t* loop, const char* filename, void (*callback)(const char*)) {
    file = (file_t*) malloc(sizeof(file_t));
    file->callback = callback;
    file->loop = loop;
    file->buf = uv_buf_init((char*) malloc(BUF_SIZE), BUF_SIZE);
    uv_fs_open(loop, &file->req, filename, O_RDONLY,0, (uv_fs_cb) on_open);
}

void on_read(uv_fs_t* open_req) {
    printf("File readed successfully!\n");
    int result = open_req->result;
    if (result < 0) {
        fprintf(stderr, "Read error: %s\n", uv_strerror(result));
    } else {
        file->callback(file->buf.base);
    }
    uv_fs_close(file->loop, &file->req, result, on_close);
}

void on_open(uv_fs_t* open_req) {
    if (open_req->result >= 0) {
        printf("File opened successfully!\n");
        uv_fs_read(file->loop, &file->req, open_req->result, &file->buf, 1, -1, on_read);
    }
}

void on_close(uv_fs_t* req) {
    uv_fs_req_cleanup(req);
}

char* get_project_root() {
    char cwd[1024];
    char* project_root = NULL;

    if (getcwd(cwd, sizeof(cwd)) == NULL) {
        perror("getcwd() error");
        return NULL;
    }

    // go up to parent directory recursively until we find a file or directory
    // that is unique to our project
    while (1) {
        // check if we found the project root
        if (access("CMakeLists.txt", F_OK) == 0) {
            project_root = strdup(cwd);
            break;
        }
        if (access(".git", F_OK) == 0) {
            project_root = strdup(cwd);
            break;
        }
        // go up to parent directory
        if (chdir("..") == -1) {
            perror("chdir() error");
            break;
        }
        if (getcwd(cwd, sizeof(cwd)) == NULL) {
            perror("getcwd() error");
            break;
        }
    }

    return project_root;
}

// ReadFile implementation
int main() {
    uv_loop_t* loop = uv_default_loop();

    // Get the absolute path to the file
    char* cwd = get_project_root();
    const char* filename = "/readme.md";
    char* path = new char[strlen(cwd) + strlen(filename) + 1];
    strcpy(path, cwd);
    strcat(path, filename);

    // Print file path
    printf("File path: %s\n", path);

    readfile(loop, path, [](const char* data) {
        printf("File contents: %s\n", data);
    });

    // Run the event loop
    uv_run(loop, UV_RUN_DEFAULT);

    // Clean up
    free(path);
    uv_loop_close(loop);

    return 0;
}


// SetInterval implementation
//int counter = 0;
//void on_timeout(uv_timer_t* handle) {
//    printf("Timeout!\n");
//    if(++counter >= 10) {
//        uv_timer_stop(handle);
//    }
//}

//}
