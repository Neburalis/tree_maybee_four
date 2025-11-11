#ifndef MYTREE_H
#define MYTREE_H

#include <stdint.h>

namespace akinator {

const int32_t signature = (int32_t) 0xF050C0C1;

typedef char * CONTAIRING_T;

struct NODE_T {
    int32_t         signature;

    CONTAIRING_T    data;

    NODE_T         *left, *right,
                   *parent;
};

struct MYTREE_T {
    size_t  size;
    NODE_T *root;
};

enum ERRNO {
    TREE_NO_PROBLEM,
    TREE_CANT_ALLOC_MEMORY,
};

NODE_T *alloc_new_node(/*NODE_T *parent*/);

MYTREE_T *rub_lamp();
// NODE_T *alloc_new_node(/*NODE_T *parent*/);

// Рекурсивно удаляет все дерево
void destroy_genie_face(MYTREE_T *tree);
// Рекурсивно удаляет все поддерево
void destroy_genie_face(NODE_T   *subtree);

// Возвращает true есть дерево в валидном состоянии, иначе - false
bool genie_health_condition(MYTREE_T *tree);
bool genie_health_condition(NODE_T   *subtree);

// Поиск узла по data
NODE_T *search(MYTREE_T *tree,    CONTAIRING_T data);
NODE_T *search(NODE_T   *subtree, CONTAIRING_T data);

void save_to_file(MYTREE_T *tree, FILE *file);
void save_to_file(NODE_T   *subtree, FILE *file);

// Инициализирует папку для логов (должна вызываться один раз в main)
int init_logger(const char *log_dirname);

FILE *get_log_file();

// Закрывает лог-файл и сбрасывает состояние логгера
void destruct_logger();

// Делает дамп дерева и добавляет визуальный фрейм в log.html внутри инициализированной папки.
// Параметр reason может быть NULL.
void dump(MYTREE_T *tree);
void dump(MYTREE_T *tree, const char *reason);
void dump(MYTREE_T *tree, const char *reason, NODE_T *highlight);

void guess(MYTREE_T *tree);

void add_new_field(MYTREE_T *tree, NODE_T *cursor);

void definition(MYTREE_T *tree, CONTAIRING_T data);
}

#endif // MYTREE_H