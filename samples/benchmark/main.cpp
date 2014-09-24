
#include <unistd.h>
#include <cstddef>
#include <cmath>
#include <cstdlib>

#include <map>
#include <vector>
#include <iostream>
#include <chrono>

#include <persistence/persistence.hpp>

std::string rand_str()
{
  size_t len = 7 + rand() % 23; // well... modulo is not really good for randomness, but...

  std::string ret;
  for (size_t i = 0; i < len; ++i)
    ret += static_cast<char>(33 + rand() % 93);
  return ret;
}


struct titi
{
  titi(int = 0)
  {
    for (size_t i = 0; i < 1501; ++i)
      v.push_back(i * 5 - 2);
    i = 0;
    d = 42.00005;
    c = 'm';
  }
  ~titi() {}

  int i;
  double d;
  std::vector<uint64_t> v;
  private:
  char c;

  friend class neam::cr::persistence;
};

struct toto
{
  toto()
  {
    i = 42;
    d = -0.000000042;
    c = 'M';
    f = 33.33333333333333;

    for (size_t i = 0; i < 67252; ++i)
      m.emplace(rand_str(), 0);
  }
  ~toto() {}

  titi ti1;
  int i;
  double d;
  std::map<std::string, titi> m;
  titi ti2;
  char c;
  float f;
  titi ti3;
};

namespace neam
{
  namespace cr
  {
    template<typename Backend> class persistence::serializable<Backend, toto> : public serializable_object
    <
      Backend,

      NCRP_TYPED_OFFSET(toto, ti1),
      NCRP_TYPED_OFFSET(toto, i),
      NCRP_TYPED_OFFSET(toto, d),
      NCRP_TYPED_OFFSET(toto, m),
      NCRP_TYPED_OFFSET(toto, ti2),
      NCRP_TYPED_OFFSET(toto, c),
      NCRP_TYPED_OFFSET(toto, f),
      NCRP_TYPED_OFFSET(toto, ti3)
    > {};

    template<typename Backend> class persistence::serializable<Backend, titi> : public serializable_object
    <
      Backend,

      NCRP_TYPED_OFFSET(titi, i),
      NCRP_TYPED_OFFSET(titi, d),
      NCRP_TYPED_OFFSET(titi, c),
      NCRP_TYPED_OFFSET(titi, v)
    > {};
  } // namespace r
} // namespace neam


template<typename PreFunction, typename Function>
void run_test(size_t count, const std::string &name, PreFunction pre, Function fnc)
{
  double sum = 0;
  double t_count = 0;
  double data_count = 0;

  std::cout << " -- running test '" << name << "' " << count << " times..." << std::endl;
  for (;t_count < count; ++t_count)
  {
    std::cout << "iteration " << t_count + 1 << "...\r";
    std::cout.flush();

    pre();

    double start = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
    data_count += fnc();
    double end = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();

    sum += end - start;
  }

  // unit conversion
  double avg_time = (sum / t_count);
  const std::string time_unit_array[3] = {"micro", "milli", ""};
  size_t time_unit_idx = 0;
  for (; time_unit_idx < 3 && avg_time >= 500; ++time_unit_idx)
    avg_time /= 1000.;
  double single_data = data_count / t_count;
  const std::string data_unit_array[5] = {"bytes", "Kb", "Mb", "Gb", "Tb"};
  size_t data_unit_idx = 0;
  size_t single_data_unit_idx = 0;
  data_count = (data_count / (sum * 1e-6));
  for (; data_unit_idx < 5 && data_count >= 1000; ++data_unit_idx)
    data_count /= 1000.;
  for (; single_data_unit_idx < 5 && single_data >= 1000; ++single_data_unit_idx)
    single_data /= 1000.;

  std::cout << " -- data size: " << single_data << " " << data_unit_array[single_data_unit_idx] << std::endl;
  std::cout << " -- test '" << name << "' takes an average of " << avg_time << " " << time_unit_array[time_unit_idx] << "seconds";
  if (data_count)
  {
    std::cout << " and with an average of " << data_count << " " << data_unit_array[data_unit_idx] << " per seconds";
  }
  std::cout << "\n" << std::endl;
}

int main()
{
  srand(reinterpret_cast<size_t>(malloc(0)) ^ time(nullptr));

  std::cout << " -- initializing data..." << std::endl;
  titi ti;
  toto to;
  neam::cr::memory_allocator mem_ti;
  neam::cr::memory_allocator checksum_mem_ti;
  neam::cr::memory_allocator mem_to;
  neam::cr::memory_allocator checksum_mem_to;

  std::cout << " ----------------\n" << std::endl;

  run_test(100000, "serialization of a small object", [&]() {mem_ti.clear();}, [&]() -> double
  {
    size_t ret = 0;
    neam::cr::persistence::serializable<neam::cr::persistence_backend::neam, titi>::to_memory(mem_ti, ret, &ti);
    return ret;
  });
  run_test(100000, "serialization of a small object (with a checksum)", [&]() {checksum_mem_ti.clear();}, [&]() -> double
  {
    size_t ret = 0;
    neam::cr::persistence::serializable<neam::cr::persistence_backend::neam, neam::cr::checksum<titi>>::to_memory(checksum_mem_ti, ret, &ti);
    return ret;
  });

  run_test(40, "serialization of a BIG object", [&]() {mem_to.clear();}, [&]() -> double
  {
    size_t ret = 0;
    mem_to.clear();
    neam::cr::persistence::serializable<neam::cr::persistence_backend::neam, toto>::to_memory(mem_to, ret, &to);
    return ret;
  });
  run_test(15, "serialization of a BIG object (with a checksum)", [&]() {checksum_mem_to.clear();}, [&]() -> double
  {
    size_t ret = 0;
    mem_to.clear();
    neam::cr::persistence::serializable<neam::cr::persistence_backend::neam, neam::cr::checksum<toto>>::to_memory(checksum_mem_to, ret, &to);
    return ret;
  });

  std::cout << " ----------------\n" << std::endl;

  char *data_ti = reinterpret_cast<char *>(mem_ti.get_contiguous_data());

  run_test(100000, "deserialization of a small object", [&]{ti.~titi();}, [&]() -> double
  {
    neam::cr::persistence::serializable<neam::cr::persistence_backend::neam, titi>::from_memory(data_ti, mem_ti.size(), &ti);
    return mem_ti.size();
  });
  data_ti = reinterpret_cast<char *>(checksum_mem_ti.get_contiguous_data());
  run_test(100000, "deserialization of a small object (with a checksum)", [&]{ti.~titi();}, [&]() -> double
  {
    neam::cr::persistence::serializable<neam::cr::persistence_backend::neam, neam::cr::checksum<titi>>::from_memory(data_ti, checksum_mem_ti.size(), &ti);
    return checksum_mem_ti.size();
  });

  char *data_to = reinterpret_cast<char *>(mem_to.get_contiguous_data());
  run_test(40, "deserialization of a BIG object", [&]{to.~toto();}, [&]() -> double
  {
    neam::cr::persistence::serializable<neam::cr::persistence_backend::neam, toto>::from_memory(data_to, mem_to.size(), &to);
    return mem_to.size();
  });
  data_to = reinterpret_cast<char *>(checksum_mem_to.get_contiguous_data());
  run_test(15, "deserialization of a BIG object (with a checksum)", [&]{to.~toto();}, [&]() -> double
  {
    neam::cr::persistence::serializable<neam::cr::persistence_backend::neam, neam::cr::checksum<toto>>::from_memory(data_to, checksum_mem_to.size(), &to);
    return checksum_mem_to.size();
  });
  std::cout << " ----------------\n -- all tests done." << std::endl;

  return 0;
}
