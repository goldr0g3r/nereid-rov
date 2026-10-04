/**
 * @file    Std_Types.h
 * @brief   Project-wide standard types (AUTOSAR-inspired, see docs/software/coding_standard.md).
 *
 * Every layer may include this header. It deliberately maps onto <stdint.h>/<stdbool.h>
 * instead of redefining AUTOSAR platform types, so plain C tooling and tests stay simple.
 */
#ifndef STD_TYPES_H
#define STD_TYPES_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

typedef uint8_t Std_ReturnType;

#define E_OK     ((Std_ReturnType)0x00u)
#define E_NOT_OK ((Std_ReturnType)0x01u)

#define STD_LOW  0x00u
#define STD_HIGH 0x01u

#define STD_OFF 0u
#define STD_ON  1u

/** Number of elements of a statically sized array. */
#define STD_ARRAY_SIZE(a) (sizeof(a) / sizeof((a)[0]))

/** Mark a parameter as intentionally unused. */
#define STD_UNUSED(x) ((void)(x))

/** Compile-time assertion usable at file scope. */
#define STD_STATIC_ASSERT(cond, msg) _Static_assert((cond), msg)

#endif /* STD_TYPES_H */
