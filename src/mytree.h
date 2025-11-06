#ifndef MYTREE_H
#define MYTREE_H

#include <stdint.h>

namespace mytree {

const int32_t signature = (int32_t) 0xF050C0C1;

typedef const char * CONTAIRING_T;

struct NODE_T {
    int32_t         signature;

    CONTAIRING_T    data;

    NODE_T         *left, *right/*,
                   *parent*/;
};

struct MYTREE_T {
    size_t  size;
    NODE_T *root;
};

enum ERRNO {
    TREE_NO_PROBLEM,
    TREE_CANT_ALLOC_MEMORY,
};

MYTREE_T *constructor();
// NODE_T *alloc_new_node(/*NODE_T *parent*/);

// Рекурсивно удаляет все дерево
void destructor(MYTREE_T *tree);
// Рекурсивно удаляет все поддерево
void destructor(NODE_T   *subtree);

// Возвращает true есть дерево в валидном состоянии, иначе - false
bool verifier(MYTREE_T *tree);
bool verifier(NODE_T   *subtree);

void print(MYTREE_T *tree);
void print(NODE_T   *subtree);

// Инициализирует папку для логов (должна вызываться один раз в main)
int init_logger(const char *log_dirname);

FILE *get_log_file();

// Закрывает лог-файл и сбрасывает состояние логгера
void destruct_logger();

// Делает дамп дерева и добавляет визуальный фрейм в log.html внутри инициализированной папки.
// Параметр reason может быть NULL.
void dump(MYTREE_T *tree, const char *reason);

void dump(MYTREE_T *tree, const char *reason, NODE_T *highlight);

// Удобная перегрузка без сообщения
void dump(MYTREE_T *tree);

NODE_T *insert(MYTREE_T *tree,    CONTAIRING_T value);
NODE_T *insert(NODE_T   *subtree, CONTAIRING_T value);

}

#endif // MYTREE_H