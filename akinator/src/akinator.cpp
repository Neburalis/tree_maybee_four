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

typedef struct PATH_STEP_T {
    const char *question;
    bool        is_positive; // true => идти в левое поддерево, false => вправо
} PATH_STEP_T;

bool is_leaf(const NODE_T *node) {
    return node != nullptr && node->left == nullptr && node->right == nullptr;
}

enum PATH_RESULT {
    PATH_NOT_FOUND,
    PATH_FOUND_LEAF
};

// Рекурсивно ищет путь к целевому листу, заполняя шаги определения.
static PATH_RESULT collect_definition_path(NODE_T *subtree, CONTAIRING_T target,
                                           PATH_STEP_T *path, size_t depth,
                                           size_t capacity, size_t *out_length) {
    if (subtree == nullptr) {
        return PATH_NOT_FOUND;
    }

    if (is_leaf(subtree)) {
        if (strcmp(subtree->data, target) == 0) {
            *out_length = depth;
            return PATH_FOUND_LEAF;
        }
        return PATH_NOT_FOUND;
    }

    if (depth >= capacity) {
        return PATH_NOT_FOUND;
    }

    if (subtree->left != nullptr) {
        path[depth].question    = subtree->data;
        path[depth].is_positive = true;

    PATH_RESULT left_result = collect_definition_path(subtree->left, target,
                              path, depth + 1,
                              capacity, out_length);
        if (left_result == PATH_FOUND_LEAF) {
            return left_result;
        }
    }

    if (subtree->right != nullptr) {
        path[depth].question    = subtree->data;
        path[depth].is_positive = false;

    PATH_RESULT right_result = collect_definition_path(subtree->right, target,
                               path, depth + 1,
                               capacity, out_length);
        if (right_result == PATH_FOUND_LEAF) {
            return right_result;
        }
    }

    return PATH_NOT_FOUND;
}

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

void save_to_file(FILE *file, MYTREE_T *tree) {
    genie_health_condition(tree) verified(return;);

    return save_to_file(file, tree->root);
}

void save_to_file(FILE *file, NODE_T *subtree) {
    genie_health_condition(subtree) verified(return;);

    fprintf(file, "(");
    fprintf(file, "\"%s\"", subtree->data);
    if (subtree->left  != nullptr) save_to_file(file, subtree->left );
    else fprintf(file, " nil");
    if (subtree->right != nullptr) save_to_file(file, subtree->right);
    else fprintf(file, " nil");
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
    scanf("%255[^\n]", &user_guess);
    clear_stdin_buffer();

    printf("Чем %s отличается от %s?\n Он ", user_guess, cursor->data);
    char new_question[256] = {};
    scanf("%255[^\n]", &new_question);
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

void definition(MYTREE_T *tree, CONTAIRING_T target) {
    genie_health_condition(tree) verified(ERROR_MSG("DB in invalid state"); return;);

    if (tree == nullptr || tree->root == nullptr || target == nullptr) {
        ERROR_MSG("Invalid arguments passed to definition()\n");
        return;
    }

    size_t capacity = tree->size > 0 ? tree->size : 1;
    PATH_STEP_T *path = typed_calloc(capacity, PATH_STEP_T);
    if (path == nullptr) {
        ERROR_MSG("Can't allocate memory for definition path\n");
        return;
    }

    size_t path_length = 0;

    PATH_RESULT result = collect_definition_path(tree->root, target, path, 0,
                                                 capacity, &path_length);
    if (result == PATH_NOT_FOUND) {
        printf("Персонаж \"%s\" не найден в базе.\n", target);
        FREE(path);
        return;
    }

    if (path_length == 0) {
        printf("%s пока не имеет характеристик.\n", target);
        FREE(path);
        return;
    }

    printf("%s ", target);
    for (size_t index = 0; index < path_length; ++index) {
        if (index > 0) {
            printf((index == path_length - 1) ? " и " : ", ");
        }

        if (!path[index].is_positive) {
            printf("не ");
        }

        printf("%s", path[index].question);
    }
    printf("\n");

    FREE(path);
}

}