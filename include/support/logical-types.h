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

#ifndef __cplusplus
  /** Enable use of the bool type in plain C.
   *
   * @warning This is probably not portable.
   */
  typedef enum
    {
      false = 0,
      true = 1
    } bool;
#endif

#ifdef __cplusplus
}
#endif

#endif /* SUPPORT_LOGICAL_TYPES_H */
