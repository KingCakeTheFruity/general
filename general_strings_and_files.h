#ifndef GENERAL_STRINGS_AND_FILES
#define GENERAL_STRINGS_AND_FILES

#include "general_common.h"

#include <assert.h>
#include <ctype.h>
#include <sys/stat.h>

//=============================================================================
//<KCTF> Strings_and_files ====================================================

typedef struct Line_s {
    unsigned char *string;
    size_t len;
} Line;

/// Struct to store file's information into
typedef struct File_s {
    const char *name;
    FILE *ptr;
    struct stat info;
    unsigned char *text;
    size_t lines_cnt;
    Line **lines;
} File;

/**
    \brief Quadratic sort

    Sorts array on-place with given comparator

    \param[in] arr array which needs to be sorted
    \param[in] elem_cnt count of elements, [0, elem_cnt) will be sorted
    \param[in] elem_size size of each element in bytes
    \param[in] comp comparator returning an int <0 of elem1<elem2, 0 if elem1=elem2, >0 if elem1>elem2
*/
void qqh_sort(void *arr, const size_t elem_cnt, const size_t elem_size, int (*comp)(const void *elem1, const void *elem2));

/**
    \brief Comparator for two lines

    Ignores everything that is not a Russian or English letter

    \param[in] elem1,elem2 elements to compare
    \return an int <0 if elem1<elem2, 0 if elem1=elem2, >0 if elem1>elem2
*/
int compare_lines_letters(const void *elem1, const void *elem2);

/**
    \brief Reversed comparator for two lines

    Ignores everything that is not a Russian or English letter

    \param[in] elem1,elem2 elements to compare
    \return an int >0 if elem1<elem2, 0 if elem1=elem2, 0 if elem1>elem2
*/
int reverse_compare_lines_letters(const void **elem1, const void **elem2);

/**
    \brief Calls all necessary free's

    Kind of destructor for the File structure

    \param[in] file object to be destroyed
*/
void File_destruct(const File *file);

/**
    \brief Reads file

    Estimates file's size and allocates just enough memory + 1 byte for \0, then calls read_lines to fill buffer 'text' and index 'lines'

    \param[in] file object to be read to
    \param[in] name - filename to be read from
    \return 0 if file is read successfully, else error code <0
*/
int File_construct(File *file, const char *name);

/**
    \brief Reads lines from file

    Stores them into given File object

    \param[in] file an object to write into, contains input file name
    \return 0 if file is read successfully, else error code <0
*/
int read_lines(File *file);

/**
    \brief Prints file into given file



    \param[in] file file containing text to write
    \param[in] output file name
*/
void print_file(const File *file, const char *fout_name, const char *mode);


/**
    \brief Checks if c is a Russian or an English letter

    .

    \param[in] c char to check
    \return true if c is a Russian or an English letter, else false
*/
int is_countable(const unsigned char c);

/**
    \brief Swaps contains of two ptrs

    .

    \param[in] first,second prts to swap
    \return
*/
void swap_ptrs(void **first, void **second);

/**
    \brief Sets *c on next position of the same string, where is_countable returns true.

    If there are no such left, *c will be pointer to the end of the string ('\0')

    \param[in] first,second prts to swap
    \return
*/
void Char_get_next_symb(const unsigned char **c);

/**
    \brief Prints text error message to standard output

    .

    \param[in] error code of error to print
*/
void print_error(int error);


//=============================================================================
// IMPLEMENTATION
//=============================================================================

int is_countable(const unsigned char c) {
    return isalnum(c);
}

void swap_ptrs(void **first, void **second) {
    assert(first);
    assert(second);

    void *tmp = *second;
    *second = *first;
    *first = tmp;
}

void qqh_sort(void *input_arr, const size_t elem_cnt, const size_t elem_size, int (*comp)(const void *elem1, const void *elem2)) {
    assert(input_arr);
    assert(comp);

    char *arr = (char*) input_arr;

    for (size_t i = 0; i < elem_cnt; ++i) {
        for (size_t j = 0; j < elem_cnt - 1; ++j) {
            void *first = arr + j * elem_size;
            void *second = arr + (j + 1) * elem_size;
            if (comp(first, second) > 0) {
                swap_ptrs((void**) first, (void**) second);
            }
        }
    }
}

void Char_get_next_symb(const unsigned char **c) {
    assert(c);

    const unsigned char *cur_c = *c;
    while(isspace(*cur_c) && *cur_c) {
        ++cur_c;
    }
    *c = cur_c;
}

char Char_in_string(const unsigned char c, const unsigned char *str) {
    while (*str) {
        if (c == *str) {
            return 1;
        }
        ++str;
    }
    return 0;
}

int compare_lines_letters(const void *elem1, const void *elem2) {
    const unsigned char *first_c  = ((**(Line* const *)elem1).string);
    const unsigned char *second_c = ((**(Line* const *)elem2).string);

    while (*first_c && *second_c) {
        Char_get_next_symb(&first_c);
        Char_get_next_symb(&second_c);

        if (*first_c != *second_c || (*first_c) * (*second_c) == 0) {
            return (int) *first_c - (int) *second_c;
        }

        ++first_c;
        ++second_c;
    }

    Char_get_next_symb(&first_c);
    Char_get_next_symb(&second_c);
    return (int) *first_c - (int) *second_c;
}

int reverse_compare_lines_letters(const void **elem1, const void **elem2) {
    return -compare_lines_letters(elem1, elem2);
}

void File_destruct(const File *file) {
    assert(file);

    Line **lines_ptr = file->lines;
    for (size_t i = 0; i < file->lines_cnt; ++i) {
        free(*lines_ptr);
        ++lines_ptr;
    }
    free(file->lines);
    free(file->text);
}

int File_construct(File *file, const char *name) {
    assert(file);
    assert(name);

    file->name = name;

    const int ret = read_lines(file);
    file->lines_cnt = (size_t) ret;
    return ret;
}


int read_lines(File *file) {
    assert(file);

    file->ptr = fopen(file->name, "rb");
    if (!file->ptr) {
        return -1;
    }

    fstat(fileno(file->ptr), &(file->info));

    file->text = (unsigned char*) calloc((size_t) file->info.st_size + 1, sizeof(char));
    if (!file->text) {
        return ERROR_MALLOC_FAIL;
    }

    fread(file->text, (size_t) file->info.st_size, 1, file->ptr);
    
    int lines_cnt = 0;
    for (unsigned char *c = file->text; *c; ++c) {
        lines_cnt += *c == '\n';
    }
    file->lines_cnt = (size_t) lines_cnt + 1;

    file->lines = (Line**) calloc((size_t) lines_cnt + 1, sizeof(Line*));
    if (file->lines == NULL) {
        return ERROR_MALLOC_FAIL;
    }

    unsigned char *c = file->text;
    lines_cnt = -1;
    int line_len = 0;
    int itrs = 0;
    while (itrs < file->info.st_size && *c) {
        ++lines_cnt;

        file->lines[lines_cnt] = (Line*) calloc(1, sizeof(Line));
        Line *line_ptr = file->lines[lines_cnt];
        if (line_ptr == NULL) {
            return ERROR_MALLOC_FAIL;
        }
        line_ptr->string = c;

        while(itrs < file->info.st_size && *c != '\n') {
            ++line_len;
            ++c;
            ++itrs;
            if (*c == '\r') {
                *c = '\0';
            }
        }
        *c = '\0';
        ++c;
        ++itrs;

        line_ptr->len = (size_t) line_len;
        line_len = 0;
    }

    return lines_cnt + 1;
}

char String_starts_with(const unsigned char *line, const char *sample) {
    if (!*line || !*sample) {
        return 0;
    }

    while (*line && *sample) {
        if (*line != *sample) {
            return 0;
        }
        ++line;
        ++sample;
    }

    return 1;
}

void print_file(const File *file, const char *fout_name, const char *mode) {
    assert(file);
    assert(fout_name);

    FILE *fout = fopen(fout_name, mode);
    for (size_t i = 0; i < file->lines_cnt; ++i) {
        fprintf(fout, "%s\n", file->lines[i]->string);
    }

    fclose(fout);
}

void print_error(int error) {
    if (error == 0) {
        return;
    }

    if (error == ERROR_FILE_NOT_FOUND) {
        printf("[ERR] File not found!\n");
    } else if (error == ERROR_BIG_FILE) {
        printf("[ERR] Can't handle such a big file!\n");
    } else if (error == ERROR_MALLOC_FAIL) {
        printf("[ERR] Can't allocate memory\n");
    } else {
        printf("[ERR](~!~)WERRORHUTGEERRORF(~!~)[ERR]\n");
    }
}

#endif // GENERAL_STRINGS_AND_FILES
