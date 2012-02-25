#define _GNU_SOURCE
#include <support/scalar.h>
#include <stdlib.h>
#include <stdio.h>		/* for prinf() */
#include <string.h>		/* for memset() */
#include <assert.h>
#include <stdarg.h>
#include <errno.h>

#include <support/matrix.h>

#define return_if_fail(expr,retval) if(!(expr)) return retval;

void
_print_matrix(Matrix *m, char *label)
{
  unsigned int i, j;
  printf("%s = {\n", label);

  for ( i = 0; i < m->rows; i++ )
    {
      printf("  [ ");

      for(j=0; j < m->cols; j++)
	printf("%g%c%c", (double) matrix_get(m, i, j), (j == (m->cols-1)) ? 0 : ',', (j == (m->cols-1)) ? 0 : ' ');
    
      printf(" ]\n");
    }
  printf("};\n");
}


/** Allocates an uninitialized Matrix structure
 *
 * @return a pointer to the structure
 */
Matrix*
matrix_alloc()
{
  return ((Matrix *) malloc(sizeof(Matrix)));
}


/** Create a new matrix. Sets all values to zero.
 *
 * @param rows rows in the matrix
 * @param cols columns in the matrix
 *
 * @return a pointer to the newly-allocated matrix
 */
Matrix*
matrix_new(const unsigned int rows, const unsigned int cols)
{
  unsigned int i;

  Matrix *out = matrix_alloc();
  assert(out != NULL);

  out->flags = 0;
  out->rows = rows;
  out->cols = cols;

#ifdef MATRIX_BY_ROW
  out->data = (scalar_t **) malloc(sizeof(scalar_t *) * rows);

  for (i=0; i < rows; i++)
    {
      out->data[i] = (scalar_t *) malloc(sizeof(scalar_t) * cols);
      memset(out->data[i], 0, (sizeof(scalar_t) * cols));
    }
#else  /* MATRIX_BY_VALUE */
  out->data = (scalar_t **) malloc(sizeof(scalar_t *) * rows * cols );

  for (i=0; i < (rows * cols); i++)
    {
      out->data[i] = (scalar_t *) malloc(sizeof(scalar_t));
      *(out->data[i]) = 0.0;
    }
#endif

  return out;
}

/** Sets all values in a matrix to 0
 *
 * @param m a pointer to the matrix to clear
 *
 * @return m
 */
Matrix*
matrix_clear(Matrix *m)
{
  unsigned int i;

#ifdef MATRIX_BY_ROW
  for(i=0; i < m->rows; i++)
    {
      memset(m->data[i], 0, (sizeof(scalar_t) * m->cols));
    } 
#else  /* MATRIX_BY_VALUE */
  for (i=0; i < (m->rows * m->cols); i++)
    {
      *(m->data[i]) = 0.0;
    }
#endif

  return m;
}
  


/** Duplicates a matrix.
 *
 * @param m the matrix to be duplicated
 *
 * @return a pointer to a duplicate of m
 */
Matrix*
matrix_dupe(Matrix *m)
{
  unsigned int i, j;
  Matrix *out;
  out = matrix_new(m->rows, m->cols);

  for (i=0; i < out->rows; i++)
    {
      for (j=0; j < out->cols; j++)
	matrix_set(out, i, j, matrix_get(m, i, j));
    }

  return out;
}


/** Free a matrix and the data it holds.
 *  Freeing a minor matrix will not free its data.
 *
 * @param m the matrix to be freed
 */
void
matrix_free(Matrix *m)
{
  assert(m != NULL);
  matrix_free_data(m);
  free(m);
}

/** Free the data used by a matrix, leaving only the shell
 * intact.
 *
 * @param  m a pointer to a matrix
 *
 * @return the pointer to the matrix shell
 */
Matrix* matrix_free_data(Matrix* m)
{
  unsigned int i
#ifdef MATRIX_BY_VALUE
    ,j
#endif
    ;

  /* For the "matrix by value pointers" model only:
   *
   * If this isn't a minor matrix, we can free
   * the data inside. If it is, other matrices could be
   * using the data and we shouldn't free it.
   */
#ifdef MATRIX_BY_ROW
  for(i=0; i < m->rows; i++)
    free(m->data[i]);
#else  /* MATRIX_BY_VALUE */
  if(!(m->flags & IS_CHILD)) {
    for(i=0; i < m->rows; i++)
      for(j=0; j < m->cols; j++) {
	free(matrix_get_ptr(m, i, j));
      }
  }
#endif

  free(m->data);

  m->data = NULL;

  return m;
}
  



/** Set a matrix as an identity matrix
 *
 * @param m a pointer to a square matrix
 *
 * @return m
 */
Matrix *matrix_set_identity(Matrix *m) {
  unsigned int i;
  assert(MATRIX_IS_SQUARE(m));

  /* Clear all values */
  matrix_clear(m);

  /* Set diagonal values to 1 */
  for(i=0; i < m->rows; i++)
    matrix_set(m, i, i, 1.0);

  return m;
}


/** Sets the values of a specific row of a matrix
 *
 * @param m a Matrix pointer
 * @param row the row to set
 *
 * @return m
 */
Matrix *matrix_set_row(Matrix *m, const unsigned int row, ...) {
  unsigned int i;
  va_list vl;

  va_start(vl, row);
  assert(row < m->rows);

  va_start(vl, row);

  for(i=0; i < m->cols; i++)
    matrix_set(m, row, i, (scalar_t) va_arg(vl, double));

  va_end(vl);

  return m;
}

/** Sets the values of a specific column of a matrix
 *
 * @param m a Matrix pointer
 * @param col the column to set
 *
 * @return m
 */
Matrix *matrix_set_col(Matrix *m, const unsigned int col, ...) {
  unsigned int i;
  va_list vl;

  assert(col < m->cols);

  va_start(vl, col);

  for(i=0; i < m->rows; i++)
    matrix_set(m, i, col, (scalar_t) va_arg(vl, double));

  va_end(vl);

  return m;
}



/** Creates the minor of a matrix by dropping the ith row and the jth column.
 *
 * @param m a pointer to a matrix
 * @param row the row number to drop, from 0 to m->rows-1
 * @param col the column number to drop, from 0 to m->cols-1
 *
 * @return the resulting minor matrix
 */
Matrix *matrix_minor(Matrix *m, const unsigned int row, const unsigned int col) {
  Matrix *out;
  unsigned int io, im, jo, jm;
  assert(row < m->rows && col < m->cols);

#ifdef MATRIX_BY_ROW		/* We'll need to just copy everything */
  out = matrix_new(m->rows - 1, m->cols - 1);
#else  /* MATRIX_BY_VALUE */
  out = (Matrix *) malloc(sizeof(Matrix));
  out->flags = 0 | IS_CHILD;
  out->rows = m->rows - 1;
  out->cols = m->cols - 1;
  out->data = (scalar_t **) malloc(sizeof(scalar_t *) * out->rows * out->cols);
#endif

  for(io=im=0; (io < out->rows) && (im < m->rows); io++, im++) {
    /* Skip column 'col' */
    if(im == col)
      im++;

    for(jo=jm=0; (jo < out->cols) && (jm < m->cols); jo++, jm++) {
      /* Skip row 'row' */
      if(jm == row)
	jm++;

#ifdef MATRIX_BY_ROW
      matrix_set(out, io, jo, matrix_get(m, im, jm));
#else  /* MATRIX_BY_VALUE */
      matrix_set_ptr(out, io, jo, matrix_get_ptr(m, im, jm));
#endif
    }
  }

  return out;
}


/** Computes the determinant of a matrix
 *
 * @param m a Matrix pointer
 *
 * @return the determinant of m
 */
scalar_t matrix_det(Matrix *m) {
  Matrix *minor;
  unsigned int j;
  scalar_t rdet, sign;

  assert(MATRIX_IS_SQUARE(m));


  if(m->cols == 2) {
    return ((matrix_get(m,0,0) * matrix_get(m,1,1))
	    - (matrix_get(m,0,1) * matrix_get(m,1,0)));
  }
  else {
    rdet = 0;
    for(j=0; j < m->cols; j++) {
      minor = matrix_minor(m, 0, j);

      sign = (j%2) ? S_LITERAL(1.0) : S_LITERAL(-1.0);
      rdet += sign * matrix_get(m, 0, j) * matrix_det(minor);

      matrix_free(minor);
    }

    return rdet;
  }

  return 0;
}


/** Multiplies two matrices. Stores the result in dest.
 *
 * @param a a Matrix pointer
 * @param b a Matrix pointer
 * @param dest pointer to a matrix in which to store the result, or NULL to allocate a new one
 *
 * @return dest, or a pointer to a new Matrix if dest is NULL
 */
Matrix*
matrix_mult(Matrix *a, Matrix *b, Matrix *dest)
{
  unsigned int i, j, k;
  scalar_t s;

  assert(a->cols == b->rows);

  if(!dest)
    dest = matrix_new(a->rows, b->cols);
  else
    assert(dest->rows == a->rows && dest->cols == b->cols) ;

  for(i=0; i < dest->rows; i++) {
    for(j=0; j < dest->cols; j++) {
      s = 0;
      for(k=0; k < a->cols; k++)
	s += (matrix_get(a,i,k) * matrix_get(b,k,j));

      matrix_set(dest, i, j, s);
    }
  }

  return dest;
}


/** Adds two matrices. Stores the result in dest.
 *
 * @param a a Matrix pointer
 * @param b a Matrix pointer
 * @param dest pointer to a matrix in which to store the result, or NULL to allocate a new one
 *
 * @return dest, or a pointer to a new Matrix if dest is NULL
 */
Matrix*
matrix_add(Matrix *a, Matrix *b, Matrix *dest)
{
  unsigned int i, j;

  assert(MATRIX_CONGRUENT(a, b));

  if(!dest)
    dest = matrix_new(a->rows, a->cols);

  for(i=0; i < dest->rows; i++) {
    for(j=0; j < dest->cols; j++) {
      matrix_set(dest, i, j, matrix_get(a,i,j) + matrix_get(b,i,j));
    }
  }

  return dest;
}


/** Transposes a matrix.
 *
 * @param m a pointer to the matrix to be transposed
 * @param dest a pointer to the location to store the result, or NULL
 *
 * @return dest, or a pointer to a new Matrix containing the result\
 *         if dest is NULL
 */
Matrix* matrix_transpose(Matrix *m, Matrix *dest)
{
  unsigned int i, j;
#ifdef MATRIX_BY_VALUE
  scalar_t *a, *b;
#endif

  if(dest)
    assert(dest->rows == m->cols && dest->cols == m->rows);

#ifdef MATRIX_BY_ROW
  if(!dest)
    dest = matrix_new(m->cols, m->rows);

#else  /* MATRIX_BY_VALUE */

  if(!dest) {
    dest = (Matrix *) malloc(sizeof(Matrix));

    dest->rows = m->cols;
    dest->cols = m->rows;

    dest->flags = 0 | IS_CHILD;
    dest->data = (scalar_t **) malloc(sizeof(scalar_t *) * (dest->rows) * (dest->cols));
  }
#endif

  for(i=0; i < m->rows; i++) {
    for(j=0; j < m->cols; j++) {
#ifdef MATRIX_BY_ROW
      matrix_set(dest, j, i, matrix_get(m, i, j));
#else  /* MATRIX_BY_VALUE */
      a = matrix_get_ptr(m, i, j);
      b = matrix_get_ptr(m, j, i);

      matrix_set_ptr(dest, i, j, b);
      matrix_set_ptr(dest, j, i, a);
#endif
    }
  }

  return dest;
}

int
matrix_compare(Matrix *a, Matrix *b)
{
  unsigned int i, j;

  if(!MATRIX_CONGRUENT(a,b))
    return -1;

  for(i=0; i < a->rows; i++)
    for(j=0; j < a->cols; j++)
      if ( S_NE(matrix_get(a, i, j), matrix_get(b, i, j)))
	return 1;

  return 0;
}
