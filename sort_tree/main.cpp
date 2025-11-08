#include <stdio.h>
#include <time.h>
#include <string.h>

#include "io_utils.h"
#include "mytree.h"

using namespace mytree;

#define DO(code) do {                                                           \
        dump(tree, "Dump before <font color=\"blue\">" #code "</font>");        \
        NODE_T *node = code;                                                    \
        dump(tree, "Dump after <font color=\"blue\">" #code "</font>", node);   \
        fprintf(get_log_file(), "<hr>");                                        \
    } while (0)

int main() {
    // init_logger

    create_folder_if_not_exists("logs");
    time_t t0 = time(NULL);
    struct tm tminfo = *localtime(&t0);
    char log_dir[256] = "";
    snprintf(log_dir, sizeof(log_dir), "logs/log-%04d%02d%02d-%02d%02d%02d",
             tminfo.tm_year + 1900, tminfo.tm_mon + 1, tminfo.tm_mday,
             tminfo.tm_hour, tminfo.tm_min, tminfo.tm_sec);
    if (create_folder_if_not_exists(log_dir) != 0) {
        init_logger("logs");
    } else {
        init_logger(log_dir);
    }

    MYTREE_T *tree = constructor();

    DO(insert(tree, "10"));
    DO(insert(tree, "5"));
    DO(insert(tree, "7"));
    DO(insert(tree, "1"));
    DO(insert(tree, "3"));
    DO(insert(tree, "2"));
    DO(insert(tree, "20"));
    DO(insert(tree, "15"));
    DO(insert(tree, "16"));

    print(tree);

    destructor(tree);
    destruct_logger();
}
