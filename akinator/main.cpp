#include <stdio.h>
#include <time.h>
#include <string.h>

#include "akinator.h"
#include "io_utils.h"

using namespace akinator;

int main() {
    TERMINAL_ENTER_ALT_SCREEN();
    TERMINAL_CLEAR_SCREEN();
    // init logger

    // Откуда вызвался && дата/время
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

    const char *db_path = "bd";

    MYTREE_T *akinator_knowledge_base = load_tree_from_file(db_path);
    if (akinator_knowledge_base == nullptr) {
        ERROR_MSG("Не смог загрузит бд из файла, создаю пустую");
        akinator_knowledge_base = rub_lamp();
    }

    if (akinator_knowledge_base == nullptr) {
        ERROR_MSG("Can't initialize knowledge base\n");
        destruct_logger();
        return 1;
    }

    dump(akinator_knowledge_base, "Dump after loading knowledge base");
    fprintf(get_log_file(), "<hr>");

    TERMINAL_CLEAR_SCREEN();

    do {
        guess(akinator_knowledge_base);
    } while(is_user_want_continue("\nНачать заново? (Y/n) ") == 1);

    while (is_user_want_continue("\nХотите получить определение персонажа? (Y/n) ") == 1) {
        printf("Кого определить? ");

        char target[256] = {};
        if (scanf("%255[^\n]", target) != 1) {
            clear_stdin_buffer();
            continue;
        }
        clear_stdin_buffer();

        definition(akinator_knowledge_base, target);
    }

    while (is_user_want_continue("\nХотите сравнить двух персонажей? (Y/n) ") == 1) {
        char first[256]  = {};
        char second[256] = {};

        printf("Первый персонаж: ");
        if (scanf("%255[^\n]", first) != 1) {
            clear_stdin_buffer();
            continue;
        }
        clear_stdin_buffer();

        printf("Второй персонаж: ");
        if (scanf("%255[^\n]", second) != 1) {
            clear_stdin_buffer();
            continue;
        }
        clear_stdin_buffer();

        diff(akinator_knowledge_base, first, second);
    }

    if (is_user_want_continue("\nСохранить базу данных? (Y/n) ") == 1) {
        FILE *save_file = fopen(db_path, "w");
        if (!save_file) {
            ERROR_MSG("Не получилось открыть базу данных для записи");
            destroy_genie_face(akinator_knowledge_base);
            destruct_logger();
            TERMINAL_EXIT_ALT_SCREEN();
            return 1;
        }
        save_to_file(save_file, akinator_knowledge_base);
        fclose(save_file);
    }

    destroy_genie_face(akinator_knowledge_base);
    destruct_logger();
    TERMINAL_EXIT_ALT_SCREEN();
}