#ifndef SYS_H
#define SYS_H

#include <stdint.h>  // 添加标准整数类型
#include "main.h"

// Cortex-M4 内联汇编函数声明
static inline void WFI_SET(void);
static inline void INTX_DISABLE(void);
static inline void INTX_ENABLE(void);
static inline void MSR_MSP(uint32_t addr);

#endif
