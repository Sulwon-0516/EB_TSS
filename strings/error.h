#ifndef ERROR_H
#define ERROR_H

#include <stdio.h>
#include <stdlib.h>

void memory_free();
void log_skipped_process(int code);
void error_process(int error_code);

#endif