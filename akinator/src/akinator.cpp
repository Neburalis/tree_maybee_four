#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>
#include <time.h>

#include "akinator.h"
#include "base.h"
#include "io_utils.h"

namespace akinator {

/*function*/ NODE_T *alloc_new_node() {
    NODE_T *new_node = typed_calloc(1, NODE_T);
    if (new_node == nullptr) {
        return nullptr;
    }
    new_node->signature=signature;
    new_node->left   = nullptr;
    new_node->right  = nullptr;
    new_node->parent = nullptr;

    return new_node;
}

MYTREE_T *rub_lamp() {
    MYTREE_T *new_tree = typed_calloc(1, MYTREE_T);
    if (new_tree == nullptr) {
        return nullptr;
    }
    new_tree->size = 0;
    new_tree->root = nullptr;

    NODE_T *new_node = alloc_new_node();
    if (new_node == nullptr) {
        destroy_genie_face(new_tree);
        return nullptr;
    }

    new_node->data = strdup("Ничто");

    new_tree->root = new_node;
    ++(new_tree->size);

    return new_tree;
}

void destroy_genie_face(MYTREE_T *tree) {
    if (tree == nullptr) return;
    destroy_genie_face(tree->root);
    FREE(tree);
}

void destroy_genie_face(NODE_T *subtree) {
    if (subtree == nullptr) return;
    // printf("prev free %s\n", subtree->data);
    if (subtree->left != nullptr) {
        destroy_genie_face(subtree->left);
    }
    if (subtree->right != nullptr) {
        destroy_genie_face(subtree->right);
    }
    // printf("post free %s\n", subtree->data);
    FREE(subtree->data);
    // MEOW();
    FREE(subtree);
}

bool genie_health_condition(NODE_T *subtree) {
    if (subtree == nullptr) return true;
    if (subtree->signature != signature) return false;
    if (subtree->left != nullptr && subtree->left->parent != subtree) {
        //             ^ можем сходить в левый узел        ^ отец левого сына это я сам
        return false;
    }
    if (subtree->right != nullptr && subtree->right->parent != subtree) {
        return false;
    }
    return genie_health_condition(subtree->left) && genie_health_condition(subtree->right);
}

bool genie_health_condition(MYTREE_T *tree) {
    if (tree == nullptr) return false;
    return genie_health_condition(tree->root);
}

NODE_T *search(MYTREE_T *tree, CONTAIRING_T data) {
    genie_health_condition(tree) verified(return nullptr;);

    return search(tree->root, data);
}

NODE_T *search(NODE_T *subtree, CONTAIRING_T data) {
    genie_health_condition(subtree) verified(return nullptr;);

    if (subtree->data == data)              return subtree;
    if (strcmp(subtree->data, data) == 0)   return subtree;
    if (subtree->left  != nullptr)          return search(subtree->left,  data);
    if (subtree->right != nullptr)          return search(subtree->right, data);
    else                                    return nullptr;
}

void save_to_file(MYTREE_T *tree, FILE *file) {
    genie_health_condition(tree) verified(return;);

    return save_to_file(tree->root, file);
}

void save_to_file(NODE_T *subtree, FILE *file) {
    genie_health_condition(subtree) verified(return;);

    fprintf(file, "(");
    fprintf(file, "%s", subtree->data);
    if (subtree->left != nullptr) save_to_file(subtree->left, file);
    if (subtree->right != nullptr) save_to_file(subtree->right, file);
    fprintf(file, ")");
}

void guess(MYTREE_T *tree) {
    genie_health_condition(tree) verified(ERROR_MSG("DB in invalid state");return;);

    NODE_T *cur = tree->root;

    while(cur->left != nullptr) { // Пока не терминальный элемент - задаем вопросы
        if (is_user_want_continue("Ваш персонаж %s? (Y/n) ", cur->data)) {
            cur = cur->left;
        } else {
            cur = cur->right;
        }
    } // Дошли до терминального элемента => отгадали (или нет в базе)
    if (is_user_want_continue("Вы загадали \"%s\"? (Y/n) ", cur->data)) {
        printf("Я снова угадал! ");
    } else {
        add_new_field(tree, cur);
    }
}

void add_new_field(MYTREE_T *tree, NODE_T *cursor) {
    printf("Что вы загадали? ");
    char user_guess[256] = {};
    scanf("%[^\n]", &user_guess);
    clear_stdin_buffer();

    printf("Чем %s отличается от %s?\n Он ", user_guess, cursor->data);
    char new_question[256] = {};
    scanf("%[^\n]", &new_question);
    clear_stdin_buffer();

    dump(tree, "dump before adding", cursor);

    NODE_T *ans_yes = alloc_new_node(), *ans_no = alloc_new_node();
    tree->size += 2;

    ans_yes->data = strdup(user_guess);
    ans_no ->data = cursor->data;

    cursor->data  = strdup(new_question);
    cursor->left  = ans_yes;
    cursor->right = ans_no;

    ans_yes->parent = cursor;
    ans_no ->parent = cursor;

    dump(tree, "dump after adding new question", cursor);
    fprintf(get_log_file(), "<hr>");
}

}