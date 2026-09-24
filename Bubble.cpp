#include "Bubble.h"

int Bubble(void* const data, const size_t  n, const size_t size,
           int (*CompareFunc)(const void* value1, const void* value2))
{
    for(size_t i = 0; i < n; i++)
    {
        bool IsChange = false;

        char* data_c = (char*) data;

        for(size_t j = 0; j < n - i - 1; j++)
        {
            if((*CompareFunc)(data_c + j * size, data_c + (j + 1) * size) > 0)
            {
                Swap(data_c + j * size, data_c + (j + 1) * size, size);
                IsChange = true;
            }
        }

        if(IsChange == false)
            break;
    }

    return 0;
}

int Swap (void* const adr1, void* const adr2, const size_t size)
{
    char* adr1_p = (char*) adr1;
    char* adr2_p = (char*) adr2;

    for(size_t i = 0; i < size; i++)
    {
        char temp = adr1_p[i];
        adr1_p[i] = adr2_p[i];
        adr2_p[i] = temp;
    }

    return 0;
}
