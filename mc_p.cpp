#include <CL/sycl.hpp>
#include <iostream>
#include <cmath>
#include <cstdlib>
#include <vector>
#include <array>
#include <chrono>
#include "oneapi/mkl/rng/device.hpp"

using namespace cl::sycl;
using std::strtol;

#define NBIN 900000  // # of bins for quadrature
#define NTRD 1024  // # of bins for quadrature
#define U_i 0.75f //  total interaction coefficient
#define U_a 0.3f // absorption coefficient
#define ROULETTE_C 3.0f // roulette constant
#define W_THRESHOLD 0.2f // threshold for mc termination

int main(int argc, char* argv[])
{
  auto t1 = std::chrono::high_resolution_clock::now();

  const int seed = 777;
  std::array<float, NTRD> sum;
  for (int i=0; i<NTRD; ++i) sum[i] = 0.0f;

  queue q(gpu_selector{});

  std::cout << "Running on: " <<
  q.get_device().get_info<info::device::name>() << std::endl;

  range<1> sizeBuf{NTRD};

  {
    buffer<float, 1> sumBuf(sum.data(), sizeBuf);
    q.submit([&](handler &h){
      auto sumAccessor =
      sumBuf.template get_access<access::mode::read_write>(h);
      h.parallel_for(sizeBuf, [=](id<1> tid) {
        oneapi::mkl::rng::device::philox4x32x10<> engine(seed, tid);
        oneapi::mkl::rng::device::uniform<> distr;
        float pos[3] = {0.0f,0.0f,0.0f}, vec[3] = {1.0f,0.0f,0.0f}, weight = 1,free_path,random_num;
        bool flag_terminate = false;
        while (!flag_terminate){
        	random_num = oneapi::mkl::rng::device::generate(distr, engine);
        	free_path = (-1.0f) * log(random_num) / U_i;
        	//update location
        	for (int i=0;i<3;i++){
        		pos[i] += free_path * vec[i];
        	}
        	if (pos[0] < 0.0f){
        		flag_terminate = true;
        	}
        	//absorption 
        	weight *= U_a/U_i;
        	//scattering
        	random_num = oneapi::mkl::rng::device::generate(distr, engine);
        	if (random_num < 0.5f){
        		for (int i=0;i<3;i++){
        			vec[i] *= -1.0f;
        		}
        	}
        	//termination
        	if (weight<W_THRESHOLD){
        		random_num = oneapi::mkl::rng::device::generate(distr, engine);
        		if (random_num>1/ROULETTE_C){
        			flag_terminate = true;
        		}
        		else{
        			weight *= ROULETTE_C;
        		}
        	}
        }
        sumAccessor[tid] += vec[0];
      });  // End parallel for
    });  // End queue submit
  }

  float avg=0.0f;
  for (int i=0; i<NTRD; i++)  // Inter-thread reduction
    avg += sum[i];
  std::cout << "Average termnation position = " << avg << std::endl;

  auto t2 = std::chrono::high_resolution_clock::now();
  auto duration = std::chrono::duration_cast<std::chrono::microseconds>( t2 - t1 ).count();
  std::cout << "using time = "<<duration<< std::endl;

  return 0;
}

