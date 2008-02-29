#ifndef SUPPORT_LOGICAL_TYPES_H
#define SUPPORT_LOGICAL_TYPES_H


#ifdef __cplusplus
extern "C"
{
#endif

  typedef long int integer_t;
  typedef unsigned long int uinteger_t;

  typedef unsigned long bitmask_t;
  typedef char* string_t;
  typedef void* ptr_t;

  typedef enum
    {
      FALSE = 0,
      TRUE = 1
    } bool_t;

#ifdef __cplusplus
}
#endif

#endif /* SUPPORT_LOGICAL_TYPES_H */
