#include <iostream>
#include <support/matrix.hh>

const scalar_t values1[3][3] =
  {
    { 1, 2, 3 },
    { 4, 5, 6 },
    { 7, 8, 9 }
  };


int
main ( int argc, char** argv )
{
  spt::Matrix<3,3> m(values1);

  std::cout << "M:" << std::endl;
  m.print();
  printf("det(M) = %.02f\n", m.determinant());


  spt::Matrix<3,3> n(m.transpose());

  std::cout << "N:" << std::endl;
  n.print();
  printf("det(N) = %.02f\n", n.determinant());

  return 0;
}
