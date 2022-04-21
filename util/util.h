#pragma once

#include <iostream>

uint32_t currentMs();

uint32_t iclock();

void itimeofday(long *sec, long *usec);

uint64_t iclock64(void);
