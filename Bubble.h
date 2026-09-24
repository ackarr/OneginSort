#ifndef BUBBLE_H
#define BUBBLE_H

#include <stdbool.h>
#include <stddef.h>

int  Bubble(void* const data, const size_t  quant_num, const size_t size,
            int (*CompareFunc)(const void* value1, const void* value2));

int  Swap  (void* const adr1, void* const adr2, const size_t size);

#endif //BUBBLE_H
