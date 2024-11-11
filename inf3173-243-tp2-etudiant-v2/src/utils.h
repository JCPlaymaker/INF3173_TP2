#ifndef INF3173_TP1_UTILS_H
#define INF3173_TP1_UTILS_H

#include <time.h>

#include "list.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Compare deux estampille de temps.
 *
 * Si t1 > t2, retourne 1
 * Si t1 == t2, retourne 0
 * Si t1 < t2, retourne -1
 */
int timespec_cmp(const struct timespec* t1, const struct timespec* t2);

/*
 * Diviser une chaine selon le séparateur et ajoutant chaque élément dans la
 * liste. La fonction associée split_string_from_file() charge la chaine
 * depuis le fichier. L'option skip permet d'ignorer les n premiers éléments.
 *
 * Par exemple: "1 2 3" -> ["1", "2", "3"]
 */
int split_string(struct list* lst, const char* str, const char* delim, int skip);
int split_string_from_file(struct list* lst, const char* path, const char* delim, int skip);

#ifdef __cplusplus
}
#endif

#endif  // INF3173_TP1_UTILS_H
