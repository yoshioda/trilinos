
#include <iostream>
#include <iomanip>

#include <Kokkos_HostMDArrayView.hpp>
#include <Kokkos_HostDeviceFor.hpp>
#include <Kokkos_HostDeviceReduce.hpp>

#include <Kokkos_test_hex_grad.hpp>
#include <Kokkos_test_reduce.hpp>

//------------------------------------------------------------------------
template <class T>
void run_test(int exp) {

  std::cout << std::right;
  std::cout << std::setw(20) << "Parallel work:"
            << std::setw(20) << "init time (sec):"
            << std::setw(20) << "reduce time (sec):"
            << std::setw(20) << "Total Time (sec):"
            << std::endl;
  std::cout << std::right << std::fixed;
  std::cout.precision(3);

  for (int i = 1; i < exp; ++i) {
    const int parallel_work_length = 1<<i;

    T test(parallel_work_length) ;

    std::cout << std::setw(20) << parallel_work_length
      << std::setw(20) << test.init_time
      << std::setw(20) << test.reduce_time
      << std::setw(20) << test.init_time + test.reduce_time
      << std::endl;
  }

  std::cout << std::left;
}

int main( int argc , char ** argv )
{
  const int exp = 25;

  std::cout << "\n\nHost Test Hex Grad:\n";
  run_test<TestHexGrad< float , Kokkos::HostMap > >(exp);

  std::cout << "\n\nHost Test Reduce:\n";
  run_test<TestMomKe< float , Kokkos::HostMap > >(exp);

  return 0 ;
}

