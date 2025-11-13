#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>
#include <time.h>
#include <ctype.h>

#include "akinator.h"
#include "base.h"
#include "io_utils.h"
#include "stringNthong.h"

namespace akinator {

function void skip_spaces(char *buffer, size_t *pos) {
    VERIFY(buffer != nullptr, ERROR_MSG("buffer is nullptr"); return;);
    VERIFY(pos != nullptr,    ERROR_MSG("pos is nullptr");    return;);

    char *cursor = buffer + (*pos);
    mystr::move_ptr_to_first_not_space_symbol(&cursor, 0);
    *pos = (size_t) (cursor - buffer);
}

function char *parse_quoted_value(char *buffer, size_t *pos, bool *error) {
    VERIFY(buffer != nullptr, return nullptr;);
    VERIFY(pos != nullptr,    return nullptr;);
    VERIFY(error != nullptr,  return nullptr;);

    skip_spaces(buffer, pos);

    char *cursor = buffer + (*pos);
//     int consumed = 0;
//     if (cursor[0] != '"' || sscanf(cursor, "\"%*[^\"]\"%n", &consumed) != 0 || consumed <= 0) {
//         *error = true;
//         ERROR_MSG("Failed to parse quoted value at position %zu\n", *pos);
//         return nullptr;
//     }
//     size_t payload_len = (size_t) (consumed >= 2 ? consumed - 2 : 0);
//
//     printf("%s", cursor);
//     MEOW();
//     printf("%d\n", consumed);
//
//     (*pos) += (size_t) consumed;

    cursor = strchr(cursor, '"') + 1;
    int len = strchr(cursor, '"') - cursor;

    char *title = TYPED_CALLOC(len+1, char);
    strncpy(title, cursor, len);
    title[len] = '\0';

    (*pos) += len + 2;

    // return result;
    return title;
}

function void debug_parse_print(char *buffer, size_t buffer_len, size_t pos, const char * reason) {
    printf("file loading dump %s\n", reason);
    // for (int i = 0; i < buffer_len; ++i) {
    //     if (i < pos) {
    //         printf(BRIGHT_BLACK("%c"), *(buffer + i));
    //     }
    //     else if (i == pos) {
    //         printf(GREEN("[%c]"), *(buffer + i));
    //     }
    //     else {
    //         printf("%c", *(buffer + i));
    //     }
    // }
    printf(BRIGHT_BLACK("%.*s"), pos, buffer);
    printf(GREEN("%c"), *(buffer + pos));
    if (pos + 1 < buffer_len) {
        printf("%s", buffer + pos + 1);
    }
    putchar('\n');
}

NODE_T *parse_node(char *buffer, size_t buffer_len, size_t *pos,
                            size_t *node_count, bool *error) {
    VERIFY(buffer != nullptr,    return nullptr;);
    VERIFY(pos != nullptr,       return nullptr;);
    VERIFY(node_count != nullptr,return nullptr;);
    VERIFY(error != nullptr,     return nullptr;);

    if (*error) {
        return nullptr;
    }

    debug_parse_print(buffer, buffer_len, *pos, "at new requrent start");
    skip_spaces(buffer, pos);
    debug_parse_print(buffer, buffer_len, *pos, "after skip spaces");

    if (buffer[*pos] == '\0') {
        *error = true;
        ERROR_MSG("Unexpected end of buffer while parsing node\n");
        return nullptr;
    }

    if (strncmp(buffer + *pos, "nil", 3) == 0) {
        (*pos) += 3;
        debug_parse_print(buffer, buffer_len, *pos, "after nil parsed");
        return nullptr;
    }

    // MEOW();
    // printf("[%s]\n", buffer);
    // printf("%zu\n", *pos);

    if (buffer[*pos] != '(') {
        *error = true;
        ERROR_MSG("Expected '(' at position %zu\n", *pos);
        return nullptr;
    }

    ++(*pos); // skip '('
    debug_parse_print(buffer, buffer_len, *pos, "after ( skiped");

    NODE_T *node = alloc_new_node();
    if (node == nullptr) {
        *error = true;
        ERROR_MSG("Can't allocate node while parsing\n");
        return nullptr;
    }

    node->data = parse_quoted_value(buffer, pos, error);
    debug_parse_print(buffer, buffer_len, *pos, "after name parsed");
    if (*error || node->data == nullptr) {
        destroy_genie_face(node);
        return nullptr;
    }

    skip_spaces(buffer, pos);
    debug_parse_print(buffer, buffer_len, *pos, "after spaces skiped");

    node->left = parse_node(buffer, buffer_len, pos, node_count, error);
    debug_parse_print(buffer, buffer_len, *pos, "after left parsed");
    if (*error) {
        destroy_genie_face(node);
        return nullptr;
    }
    if (node->left != nullptr) {
        node->left->parent = node;
    }

    skip_spaces(buffer, pos);
    debug_parse_print(buffer, buffer_len, *pos, "after spaces skiped");

    node->right = parse_node(buffer, buffer_len, pos, node_count, error);
    debug_parse_print(buffer, buffer_len, *pos, "after right parsed");
    if (*error) {
        destroy_genie_face(node);
        return nullptr;
    }
    if (node->right != nullptr) {
        node->right->parent = node;
    }

    skip_spaces(buffer, pos);
    debug_parse_print(buffer, buffer_len, *pos, "after spaces skiped");

    if (buffer[*pos] != ')') {
        *error = true;
        ERROR_MSG("Expected ')' at position %zu\n", *pos);
        destroy_genie_face(node);
        return nullptr;
    }

    ++(*pos); // skip ')'
    debug_parse_print(buffer, buffer_len, *pos, "after ) skiped");

    ++(*node_count);

    return node;
}

MYTREE_T *load_tree_from_file(const char *filename) {
    VERIFY(filename != nullptr, ERROR_MSG("filename is nullptr"); return nullptr;);

    size_t buffer_len = 0;
    char *buffer = read_file_to_buf(filename, &buffer_len);
    if (buffer == nullptr) {
        ERROR_MSG("Can't read knowledge base file '%s'\n", filename);
        return nullptr;
    }

    size_t pos = 0;
    size_t node_count = 0;
    bool error = false;

    NODE_T *root = parse_node(buffer, buffer_len, &pos, &node_count, &error);
    skip_spaces(buffer, &pos);

    if (error || root == nullptr) {
        ERROR_MSG("Failed to parse knowledge base from '%s'\n", filename);
        if (root != nullptr) {
            destroy_genie_face(root);
        }
        FREE(buffer);
        return nullptr;
    }

    skip_spaces(buffer, &pos);

    if (buffer[pos] != '\0') {
        ERROR_MSG("Unexpected data at the end of '%s'\n", filename);
        destroy_genie_face(root);
        FREE(buffer);
        return nullptr;
    }

    MYTREE_T *tree = TYPED_CALLOC(1, MYTREE_T);
    if (tree == nullptr) {
        ERROR_MSG("Can't allocate tree structure for '%s'\n", filename);
        destroy_genie_face(root);
        FREE(buffer);
        return nullptr;
    }

    tree->root = root;
    tree->size = node_count;

    if (!genie_health_condition(tree)) {
        ERROR_MSG("Parsed tree from '%s' is in invalid state\n", filename);
        destroy_genie_face(tree);
        FREE(buffer);
        return nullptr;
    }

    FREE(buffer);

    return tree;
}

// true => идти в левое поддерево, false => вправо
typedef bool PATH_STEP_T;

bool is_leaf(const NODE_T *node) {
    return node != nullptr && node->left == nullptr && node->right == nullptr;
}

enum PATH_RESULT {
    PATH_NOT_FOUND,
    PATH_FOUND_LEAF
};

// Рекурсивно ищет путь к целевому листу, заполняя шаги определения.
function PATH_RESULT collect_definition_path(NODE_T *subtree, CONTAIRING_T target,
                                           PATH_STEP_T *path, size_t depth,
                                           size_t capacity, size_t *out_length) {
    VERIFY(path != nullptr,        return PATH_NOT_FOUND;);
    VERIFY(out_length != nullptr,  return PATH_NOT_FOUND;);

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
        path[depth] = true;

        PATH_RESULT left_result = collect_definition_path(subtree->left, target,
                                                          path, depth + 1,
                                                          capacity, out_length);
        if (left_result == PATH_FOUND_LEAF) {
            return left_result;
        }
    }

    if (subtree->right != nullptr) {
        path[depth] = false;

        PATH_RESULT right_result = collect_definition_path(subtree->right, target,
                                                           path, depth + 1,
                                                           capacity, out_length);
        if (right_result == PATH_FOUND_LEAF) {
            return right_result;
        }
    }

    return PATH_NOT_FOUND;
}

function const NODE_T *descend_by_steps(const NODE_T *node, const PATH_STEP_T *path,
                                      size_t steps) {
    VERIFY(path != nullptr, return nullptr;);

    for (size_t index = 0; index < steps && node != nullptr; ++index) {
        node = path[index] ? node->left : node->right;
    }
    return node;
}

function void print_feature_sequence(const NODE_T *root, const PATH_STEP_T *path,
                                   size_t start, size_t length) {
    VERIFY(genie_health_condition(root), ERROR_MSG("DB in invalid state"); return;);
    VERIFY(path   != nullptr,            ERROR_MSG("path is nullptr");     return;);
    VERIFY(length != 0,                  ERROR_MSG("length is zero");      return;);

    const NODE_T *question_node = descend_by_steps(root, path, start);
    // todo maybe make question node == null an error?
    for (size_t offset = 0; offset < length; ++offset) {
        if (offset > 0) {
            printf((offset == length - 1) ? " и " : ", ");
        }

        if (!path[start + offset]) {
            printf("не ");
        }

        if (question_node != nullptr) {
            printf("%s", question_node->data);
        } else {
            printf("(неизвестно)");
        }

        if (offset + 1 < length && question_node != nullptr) {
            question_node = path[start + offset]
                                ? question_node->left
                                : question_node->right;
        }
    }
}

/*function*/ NODE_T *alloc_new_node() {
    NODE_T *new_node = TYPED_CALLOC(1, NODE_T);
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
    MYTREE_T *new_tree = TYPED_CALLOC(1, MYTREE_T);
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

bool genie_health_condition(const NODE_T *subtree) {
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

bool genie_health_condition(const MYTREE_T *tree) {
    if (tree == nullptr) return false;
    return genie_health_condition(tree->root);
}

NODE_T *search(MYTREE_T *tree, CONTAIRING_T data) {
    genie_health_condition(tree) VERIFIED(return nullptr;);

    return search(tree->root, data);
}

NODE_T *search(NODE_T *subtree, CONTAIRING_T data) {
    genie_health_condition(subtree) VERIFIED(return nullptr;);

    if (subtree->data == data)              return subtree;
    if (strcmp(subtree->data, data) == 0)   return subtree;
    if (subtree->left  != nullptr)          return search(subtree->left,  data);
    if (subtree->right != nullptr)          return search(subtree->right, data);
    else                                    return nullptr;
}

void save_to_file(FILE *file, MYTREE_T *tree) {
    genie_health_condition(tree) VERIFIED(return;);

    return save_to_file(file, tree->root);
}

void save_to_file(FILE *file, NODE_T *subtree) {
    genie_health_condition(subtree) VERIFIED(return;);

    fprintf(file, "(");
    fprintf(file, "\"%s\"", subtree->data);
    if (subtree->left  != nullptr) save_to_file(file, subtree->left );
    else fprintf(file, " nil");
    if (subtree->right != nullptr) save_to_file(file, subtree->right);
    else fprintf(file, " nil");
    fprintf(file, ")");
}

void guess(MYTREE_T *tree) {
    genie_health_condition(tree) VERIFIED(ERROR_MSG("DB in invalid state");return;);

    NODE_T *cur = tree->root;

    while(!is_leaf(cur)) { // Пока не терминальный элемент - задаем вопросы
        if (is_user_want_continue("Ваш персонаж %s? (Y/n) ", cur->data)) {
            cur = cur->left;
        } else {
            cur = cur->right;
        }
    } // Дошли до терминального элемента => отгадали (или нет в базе)
    if (is_user_want_continue("Вы загадали %s? (Y/n) ", cur->data)) {
        printf("Я снова угадал!\n");
    } else {
        add_new_object(tree, cur);
    }
}

void add_new_object(MYTREE_T *tree, NODE_T *cursor) {
    genie_health_condition(tree) VERIFIED(ERROR_MSG("DB in invalid state");return;);
    VERIFY(cursor != nullptr, ERROR_MSG("cursor is nullptr"); return;);

    printf("Что вы загадали? ");
    char user_guess[256] = {};
    scanf("%255[^\n]", user_guess);
    clear_stdin_buffer();

    printf("Чем %s отличается от %s?\n Он ", user_guess, cursor->data);
    char new_question[256] = {};
    scanf("%255[^\n]", new_question);
    clear_stdin_buffer();

    dump(tree, "dump before adding", cursor);

    NODE_T *ans_yes = alloc_new_node();
    NODE_T *ans_no  = alloc_new_node();

    tree->size += 2;

    bool has_negation = false;
    char *neg_pos = strstr(new_question, "не");
    if (neg_pos != nullptr) {
        has_negation = true;
        size_t removal_len = strlen("не");
        memmove(neg_pos, neg_pos + removal_len, strlen(neg_pos + removal_len) + 1);
        size_t len = strlen(new_question);
        while (len > 0 && (new_question[len - 1] == ' ' || new_question[len - 1] == '\t')) {
            new_question[len - 1] = '\0';
            --len;
        }
        char *start = new_question;
        while (*start == ' ' || *start == '\t') {
            ++start;
        }
        if (start != new_question) {
            memmove(new_question, start, strlen(start) + 1);
        }
    }

    VERIFY(cursor->data != nullptr, {
        ERROR_MSG("cursor->data is nullptr");
        FREE(ans_yes);
        FREE(ans_no);
        tree->size -= 2;
        return;
    });

    if (has_negation) {
        ans_yes->data = cursor->data;
        ans_no ->data = strdup(user_guess);
    } else {
        ans_yes->data = strdup(user_guess);
        ans_no ->data = cursor->data;
    }

    cursor->data  = strdup(new_question);
    cursor->left  = ans_yes;
    cursor->right = ans_no;

    ans_yes->parent = cursor;
    ans_no ->parent = cursor;

    dump(tree, "dump after adding new question", cursor);
    fprintf(get_log_file(), "<hr>");
}

void definition(MYTREE_T *tree, CONTAIRING_T target) {
    genie_health_condition(tree) VERIFIED(ERROR_MSG("DB in invalid state"); return;);

    if (tree->root == nullptr || target == nullptr) {
        ERROR_MSG("Invalid arguments passed to definition()\n");
        return;
    }

    size_t capacity = tree->size > 0 ? tree->size : 1;
    PATH_STEP_T *path = TYPED_CALLOC(capacity, PATH_STEP_T);
    if (path == nullptr) {
        ERROR_MSG("Can't allocate memory for definition path\n");
        return;
    }

    size_t path_length = 0;
    PATH_RESULT result = collect_definition_path(tree->root, target, path, 0,
                                                 capacity, &path_length);
    if (result == PATH_NOT_FOUND) {
        printf("Персонаж %s не найден в базе.\n", target);
        FREE(path);
        return;
    }

    if (path_length == 0) {
        printf("%s пока не имеет характеристик.\n", target);
        FREE(path);
        return;
    }

    printf("%s ", target);
    print_feature_sequence(tree->root, path, 0, path_length);
    printf("\n");

    FREE(path);
}

void diff(MYTREE_T *tree, CONTAIRING_T target1, CONTAIRING_T target2) {
    genie_health_condition(tree) VERIFIED(ERROR_MSG("DB in invalid state"); return;);

    if (tree == nullptr || tree->root == nullptr || target1 == nullptr || target2 == nullptr) {
        ERROR_MSG("Invalid arguments passed to diff()\n");
        return;
    }

    size_t capacity = tree->size > 0 ? tree->size : 1;
    PATH_STEP_T *path1 = TYPED_CALLOC(capacity, PATH_STEP_T);
    PATH_STEP_T *path2 = TYPED_CALLOC(capacity, PATH_STEP_T);
    if (path1 == nullptr || path2 == nullptr) {
        ERROR_MSG("Can't allocate memory for diff paths\n");
        FREE(path1);
        FREE(path2);
        return;
    }

    size_t len1 = 0, len2 = 0;
    PATH_RESULT res1 = collect_definition_path(tree->root, target1, path1, 0,
                                               capacity, &len1);
    PATH_RESULT res2 = collect_definition_path(tree->root, target2, path2, 0,
                                               capacity, &len2);

    if (res1 == PATH_NOT_FOUND) { // TODO put before res2 = ...
        printf("Персонаж %s не найден в базе.\n", target1);
        FREE(path1);
        FREE(path2);
        return;
    }

    if (res2 == PATH_NOT_FOUND) {
        printf("Персонаж %s не найден в базе.\n", target2);
        FREE(path1);
        FREE(path2);
        return;
    }

    size_t common = 0;
    size_t min_len = (len1 < len2) ? len1 : len2;
    while (common < min_len &&
        path1[common] == path2[common]) {
        ++common;
    }

    if (common == len1 && common == len2) {
        printf("%s и %s — это один и тот же персонаж.\n", target1, target2);
        FREE(path1);
        FREE(path2);
        return;
    }

    printf("%s и %s ", target1, target2);

    if (common > 0) {
        printf("оба ");
        print_feature_sequence(tree->root, path1, 0, common);
        printf(", но ");
    } else {
        printf("не имеют общих характеристик; ");
    }

    printf("%s ", target1);
    if (len1 > common) {
        print_feature_sequence(tree->root, path1, common, len1 - common);
    } else {
        printf("по этим характеристикам совпадает");
    }

    printf(", а %s ", target2);
    if (len2 > common) {
        print_feature_sequence(tree->root, path2, common, len2 - common);
    } else {
        printf("по этим характеристикам совпадает");
    }
    printf(".\n");

    FREE(path1);
    FREE(path2);
}

}