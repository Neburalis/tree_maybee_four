#include <stdio.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>

#include "mytree.h"
#include "base.h"

namespace mytree {

global char   LOG_DIR[512]      = "";
global bool   LOG_DIR_INITED    = false;
global FILE  *LOG_FILE          = nullptr;
global char   LOG_FILEPATH[512] = "";
global size_t DUMP_COUNTER      = 0;

FILE *get_log_file() {
    return LOG_FILE;
}

int init_logger(const char *log_dirname) {
    if (log_dirname == nullptr || log_dirname[0] == '\0') return -1;

    /* Закрываем предыдущий лог, если был открыт */
    if (LOG_FILE != nullptr) {
        fclose(LOG_FILE);
        LOG_FILE = nullptr;
    }

    memset(LOG_DIR, 0, sizeof(LOG_DIR));
    memset(LOG_FILEPATH, 0, sizeof(LOG_FILEPATH));
    LOG_DIR_INITED = false;

    size_t len = strlen(log_dirname);
    if (len >= sizeof(LOG_DIR)) return -1;
    strcpy(LOG_DIR, log_dirname);

    int written = 0;
    if (log_dirname[len - 1] == '/')
        written = snprintf(LOG_FILEPATH, sizeof(LOG_FILEPATH), "%slog.html", log_dirname);
    else
        written = snprintf(LOG_FILEPATH, sizeof(LOG_FILEPATH), "%s/log.html", log_dirname);

    if (written <= 0 || (size_t) written >= sizeof(LOG_FILEPATH)) {
        LOG_DIR[0] = '\0';
        return -1;
    }

    LOG_FILE = fopen(LOG_FILEPATH, "w");
    if (LOG_FILE == nullptr) {
        LOG_DIR[0] = '\0';
        LOG_FILEPATH[0] = '\0';
        return -1;
    }

    fprintf(LOG_FILE,
        "<html>\n<head><meta charset=\"utf-8\"><title>Tree dumps</title></head>\n<body>\n<pre>\n");
    fflush(LOG_FILE);

    LOG_DIR_INITED = true;
    DUMP_COUNTER = 0;
    return 0;
}

void destruct_logger() {
    if (LOG_FILE != nullptr) {
        fprintf(LOG_FILE, "</pre>\n</body>\n</html>\n");
        fclose(LOG_FILE);
        LOG_FILE = nullptr;
    }
    LOG_DIR[0] = '\0';
    LOG_FILEPATH[0] = '\0';
    LOG_DIR_INITED = false;
    DUMP_COUNTER = 0;
}

static const char * _get_active_dir() {
    return LOG_DIR_INITED ? LOG_DIR : ".";
}

static int _write_node(NODE_T *subtree, FILE *fp, int *id_counter) {
    if (subtree == nullptr) return -1;

    int my_id = (*id_counter)++;

    fprintf(fp, "\tnode%d [label=\"{ <addr> addr=%p | <data> data=\\\"%d\\\" | { <left> left=%p | <right> right=%p } }\", shape=record, style=filled, fillcolor=\"#ffe5b7ff\"];\n",
            my_id, (void*)subtree, atoi(subtree->data), (void*)subtree->left, (void*)subtree->right);

    if (subtree->left != nullptr) {
        int left_id = _write_node(subtree->left, fp, id_counter);
        if (left_id >= 0)
            fprintf(fp, "\tnode%d -> node%d [color=\"#0c0ccc\", label=\"L\", constraint=true];\n", my_id, left_id);
    }

    if (subtree->right != nullptr) {
        int right_id = _write_node(subtree->right, fp, id_counter);
        if (right_id >= 0)
            fprintf(fp, "\tnode%d -> node%d [color=\"#3dad3d\", label=\"R\", constraint=true];\n", my_id, right_id);
    }

    return my_id;
}

static void _generate_dot_dump(MYTREE_T *tree, FILE *fp) {
    if (fp == nullptr) return;
    fprintf(fp,
        "digraph BinaryTree {\n"
        "\trankdir=TB;\n"
        "\tnode [fontname=\"Helvetica\", fontsize=10];\n"
        "\tedge [arrowsize=0.8];\n"
        "\tgraph [splines=true, concentrate=false];\n\n");

    if (tree == nullptr || tree->root == nullptr) {
        fprintf(fp, "\t// empty tree\n\n\tlabel = \"empty tree\";\n");
        fprintf(fp, "}\n");
        return;
    }

    int id_counter = 0;
    _write_node(tree->root, fp, &id_counter);

    fprintf(fp, "}\n");
}

function int _generate_files(MYTREE_T *tree, const char *dir, char *out_basename, size_t out_size) {
    verifier(tree) verified(return -1;);

    const char *outdir = (dir && dir[0] != '\0') ? dir : ".";

    time_t t = time(NULL);
    struct tm tm = *localtime(&t);

    char dot_path[512] = "";
    if (outdir[strlen(outdir) - 1] == '/')
        snprintf(dot_path, sizeof(dot_path), "%stree_dump.dot.tmp", outdir);
    else
        snprintf(dot_path, sizeof(dot_path), "%s/tree_dump.dot.tmp", outdir);

    FILE *fp = fopen(dot_path, "w");
    if (fp == nullptr) return -1;

    _generate_dot_dump(tree, fp);
    fclose(fp);

    char image_basename[256] = "";
    snprintf(image_basename, sizeof(image_basename), "tree_dump_%zu.svg",
             DUMP_COUNTER);

    char svg_path[512] = "";
    if (outdir[strlen(outdir) - 1] == '/')
        snprintf(svg_path, sizeof(svg_path), "%s%s", outdir, image_basename);
    else
        snprintf(svg_path, sizeof(svg_path), "%s/%s", outdir, image_basename);

    char command[512] = "";
    snprintf(command, sizeof(command), "dot -Tsvg %s -o %s", dot_path, svg_path);
    (void) system(command);

    if (out_basename && out_size > 0) {
        strncpy(out_basename, image_basename, out_size);
        out_basename[out_size - 1] = '\0';
    }

    ++DUMP_COUNTER;
    return 0;
}

void dump(MYTREE_T *tree, const char *reason) {
    const char *dir = _get_active_dir();
    char basename[256] = "";
    int rc = _generate_files(tree, dir, basename, sizeof(basename));
    /* Запись в уже открытый лог-файл */
    if (LOG_FILE == nullptr) return;

    char time_str[64] = "";
    time_t now = time(NULL);
    struct tm now_tm = *localtime(&now);
    strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", &now_tm);


    if (reason) fprintf(LOG_FILE, "<p>%s</p>\n", reason);
    if (rc == 0 && basename[0] != '\0') fprintf(LOG_FILE, "<img src=\"%s\">\n", basename);
    else fprintf(LOG_FILE, "<p>SVG not generated</p>\n");
    // fprintf(LOG_FILE, "<hr>\n");
    fflush(LOG_FILE);
}

void dump(MYTREE_T *tree) {
    dump(tree, nullptr);
}

}