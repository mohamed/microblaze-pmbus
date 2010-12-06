#ifndef UTIL_H_
#define UTIL_H_

#include <ctype.h>
#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include <xbasic_types.h>

typedef char* charptr;

void print_buffer(u8 *buf_ptr, u8 buf_len);
void xil_sprintf(char *output_buf, const charptr ctrl1, ...);

#endif
