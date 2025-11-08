#include <stdio.h>
#include <time.h>
#include <string.h>

#include "akinator.h"
#include "io_utils.h"

using namespace akinator;

int main() {
    // init logger
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

    MYTREE_T *akinator_knowledge_base = rub_lamp();

//     akinator_knowledge_base->root->data = strdup("животное");
//
//     NODE_T *new_node = alloc_new_node();
//     new_node->data = strdup("Полторашка");
//     akinator_knowledge_base->root->left = new_node;
//     ++(akinator_knowledge_base->size);
//
//     new_node = alloc_new_node();
//     new_node->data = strdup("препает матан");
//     akinator_knowledge_base->root->right = new_node;
//     ++(akinator_knowledge_base->size);
//
//     new_node = alloc_new_node();
//     new_node->data = strdup("Петрович");
//     akinator_knowledge_base->root->right->left = new_node;
//     ++(akinator_knowledge_base->size);
//
//     new_node = alloc_new_node();
//     new_node->data = strdup("Паша Т");
//     akinator_knowledge_base->root->right->right= new_node;
//     ++(akinator_knowledge_base->size);
//
//     dump(akinator_knowledge_base, "Dump after generate predefined tree");

    do {
        guess(akinator_knowledge_base);
    } while(is_user_want_continue("\nНачать заново? (Y/n) ") == 1);

    destroy_genie_face(akinator_knowledge_base);
    destruct_logger();
}