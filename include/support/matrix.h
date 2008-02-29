#ifndef SUPPORT_MATRIX_H
#define SUPPORT_MATRIX_H

#include <support/scalar.h>


/* Which model do we use?
 *
 * By allocating by row, we can access the data more easily
 * with matrix->data[i][j]. Simple and elegant, except for
 * the fact that it's nearly impossible to share data
 * between matrices.
 *
 * By allocating an array of pointers to the values, we can
 * share data between matrices, and thus easily do things
 * like creating minors or transposes without allocating much more
 * memory. Unfortunately, it also makes accessing the actual
 * data a whole lot more ugly - and SLOW.
 */

/* Less complicated, but also less flexible */
#define  MATRIX_BY_ROW

/* More complicated, but also more flexible */
/* #define MATRIX_BY_VALUE */


#ifdef MATRIX_BY_ROW
#define matrix_get(m,r,c) (m->data[r][c])
#define matrix_set(m,r,c,v) (m->data[r][c] = v)
#elif defined MATRIX_BY_VALUE
#define matrix_get(m,r,c) ((m->data[(r*m->cols)+c]) ? *(m->data[(r*m->cols)+c]) : 0.0)
#define matrix_set(m,r,c,v) (*(m->data[(r*m->cols)+c]) = v)

#define matrix_get_ptr(m,r,c) (m->data[(r*m->cols)+c])
#define matrix_set_ptr(m,r,c,p) (m->data[(r*m->cols)+c] = p)
#else
#error No matrix model defined.
#endif


#define MATRIX_IS_SQUARE(m) (m->cols == m->rows)
#define MATRIX_CONGRUENT(a, b) (a->rows == b->rows && a->cols == b->cols)

typedef struct _matrix Matrix;

enum _matrix_flags {
  IS_CHILD = 1 << 0		/* uses values from another matrix */
};


struct _matrix {
  unsigned int flags;
  unsigned int rows, cols;

  scalar_t **data;
};

Matrix *matrix_alloc();		/* allocate a new, uninitialized Matrix struct. */

Matrix *matrix_new(const unsigned int rows, const unsigned int cols);

Matrix *matrix_clear(Matrix *m);

Matrix *matrix_copy(Matrix *m);

void matrix_free(Matrix *m);

Matrix *matrix_free_data(Matrix *m);


Matrix *matrix_set_identity(Matrix *m);


Matrix *matrix_set_row(Matrix *m, const unsigned int row, ...);

Matrix *matrix_set_col(Matrix *m, const unsigned int col, ...);


Matrix *matrix_minor(Matrix *m, const unsigned int i, const unsigned int j);

scalar_t matrix_det(Matrix *m);

Matrix *matrix_mult(Matrix *a, Matrix *b, Matrix *dest);

Matrix *matrix_add(Matrix *a, Matrix *b, Matrix *dest);

Matrix *matrix_transpose(Matrix *m, Matrix *dest);


int matrix_compare(Matrix *a, Matrix *b);



#define print_matrix(m) _print_matrix(m, #m)

void _print_matrix(Matrix *m, char *label);




#endif /* SUPPORT_MATRIX_H */
