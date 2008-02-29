#ifndef SUPPORT_MATRIX_HH
#define SUPPORT_MATRIX_HH

#include <support/support-config.h>
#include <support/scalar.h>

#include <stdexcept>

#ifndef __cplusplus
#error this is a c++ header file.
#else

#define MatrixMinor Matrix<_T_rows - 1, _T_columns - 1>

namespace spt
{
  /** Template-instantiated matrix class that uses immediate storage for member data storage.
   */
  template < unsigned int _T_rows, unsigned int _T_columns >
  class Matrix
  {
  public:
    Matrix()
    {}

    Matrix( const scalar_t values[_T_rows][_T_columns] )
    {
      reset(values);
    }

    virtual
    ~Matrix()
    {}

    void
    reset()
    {
      unsigned int i, j;
      for ( i = 0; i < _T_rows; i++ )
	for ( j = 0; j < _T_columns; j++ )
	  _M_data[i][j] = 0;
    }


    void
    reset( const scalar_t values[_T_rows][_T_columns])
    {
      unsigned int i, j;
      for ( i = 0; i < _T_rows; i++ )
	for ( j = 0; j < _T_columns; j++ )
	  _M_data[i][j] = values[i][j];
    }

    scalar_t
    trace()
    {
      return -1;
    }

    /** Creates the minor of a matrix by dropping the ith row and the jth
     * column.
     *
     * @param dest		Destination to store the result.
     * @param dropRow		The row number to drop.
     * @param dropColumn	The column number to drop.
     */

    virtual void
    setMinorMatrix(MatrixMinor& dest,
		   const unsigned int dropRow,
		   const unsigned int dropColumn)
    {
      unsigned int i_dest, i_src, j_dest, j_src;
      for ( i_dest = 0, i_src = 0;
	    i_dest < _T_rows - 1 && i_src < _T_rows;
	    i_dest++, i_src++ )
	{
	  if ( i_src == dropColumn )
	    i_src++;

	  for ( j_dest = 0, j_src = 0;
		j_dest < _T_rows - 1 && j_src < _T_rows;
		j_dest++, j_src++ )
	    {	  
	      if ( j_src == dropRow )
		j_src++;
	      
	      dest.set(i_dest, j_dest, _M_data[i_src][j_src]);
	    }
	}
    }

    scalar_t
    get(unsigned int row, unsigned int column)
    {
      return _M_data[row][column];
    }

    void
    set(unsigned int row, unsigned int column, scalar_t value)
    {
      _M_data[row][column] = value;
    }

    scalar_t
    determinant()
    {
      scalar_t out(0);

      if ( _T_rows != _T_columns )
	throw std::logic_error("The determinant is not defined for non-square matrices.");
      
      else
	{
	  int j;
	  MatrixMinor minor;
	  for ( j = 0; j < _T_columns; j++ )
	    {
	      setMinorMatrix(minor, 0, j);
	      out +=
		((j % 2) ? 1 : -1)
		* _M_data[0][j]
		* minor.determinant();
	    }
	}

      return out;
    }

    /** Transpose a square matrix.
     *
     * @returns	The transposed matrix.
     */
    Matrix<_T_rows, _T_columns>
    transpose()
    {
      if ( _T_rows != _T_columns )
	throw std::logic_error("Only square matrices can be transposed.");

      Matrix<_T_rows, _T_columns> out;
      unsigned int i, j;

      for ( i = 0; i < _T_rows; i++ )
	for ( j = i; j < _T_columns; j++ )
	  {
	    out._M_data[i][j] =  _M_data[j][i];
	    out._M_data[j][i] =  _M_data[i][j];
	  }

      return out;
    }

    void
    print()
    {
      int i, j;

      /* printf("{\n"); */
      for ( i = 0; i < _T_rows; i++ )
	{
	  if ( i == 0 )
	    printf("  ⎡ ");	/* <space><space><U+23A1><space> */
	  else if ( i == _T_rows - 1 )
	    printf("  ⎣ ");	/* <space><space><U+23A3><space> */
	  else
	    printf("  ⎢ ");	/* <space><space><U+23A2><space> */	    
	    

	  for(j=0; j < _T_columns; j++)
	    printf("%.02g%s", _M_data[i][j],
		   (j == (_T_columns - 1) ) ? " " : ", ");
    
	  if ( i == 0 )
	    puts("⎤");		/* U+23A4 */
	  else if ( i == _T_rows - 1 )
	    puts("⎦");		/* U+23A6 */
	  else
	    puts("⎥");		/* U+23A5 */
	}
      /* printf("};\n"); */
    }

  protected:
    scalar_t _M_data[_T_rows][_T_columns];
  };

  
  scalar_t
  Matrix<2,2>::determinant()
  {
    return ( _M_data[0][0] * _M_data[1][1] ) - ( _M_data[0][1] * _M_data[1][0] );
  }

  virtual void
  Matrix<1,1>::setMinorMatrix()
  {
    return;
  }

}

#endif	/* C++ only! */

#endif	/* SUPPORT_MATRIX_HH */
