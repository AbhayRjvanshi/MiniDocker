#include "utils.h"
#include <stdarg.h>
#include <time.h>

void log_message(log_level_t level, const char *format, ...) {
    if (!format) {
        return;
    }
    
    const char *level_str[] = {"DEBUG", "INFO", "WARN", "ERROR"};
    
    time_t now = time(NULL);
    struct tm *tm_info = localtime(&now);
    
    if (!tm_info) {
        fprintf(stderr, "Error: Failed to get local time\n");
        return;
    }
    
    if (level < 0 || level >= (int)(sizeof(level_str)/sizeof(level_str[0]))) {
        level = LOG_ERROR;
    }
    printf("[%02d:%02d:%02d] [%s] ", 
           tm_info->tm_hour, tm_info->tm_min, tm_info->tm_sec,
           level_str[level]);
    
    va_list args;
    va_start(args, format);
    vprintf(format, args);
    va_end(args);
    
    printf("\n");
}

void die(const char *msg) {
    if (msg) {
        log_message(LOG_ERROR, "Fatal error: %s", msg);
    }
    exit(EXIT_FAILURE);
}

int file_exists(const char *path) {
    if (!path) {
        return 0;
    }
    return access(path, F_OK) == 0;
}

char *read_file_content(const char *path) {
    if (!path) {
        return NULL;
    }
    
    FILE *file = fopen(path, "r");
    if (!file) {
        return NULL;
    }
    
    if (fseek(file, 0, SEEK_END) != 0) {
        fclose(file);
        return NULL;
    }
    
    long size = ftell(file);
    if (size < 0) {
        fclose(file);
        return NULL;
    }
    
    if (fseek(file, 0, SEEK_SET) != 0) {
        fclose(file);
        return NULL;
    }
    
    char *content = malloc(size + 1);
    if (!content) {
        fclose(file);
        return NULL;
    }
    
    size_t bytes_read = fread(content, 1, size, file);
    content[bytes_read] = '\0';
    
    fclose(file);
    return content;
}