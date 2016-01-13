
#include <unistd.h>
#include <cstddef>
#include <cmath>
#include <cstdlib>

#include <map>
#include <vector>
#include <iostream>
#include <chrono>

#include <persistence/persistence.hpp>

/// \brief a class we want to be serialized / deserialized
struct my_struct
{
  my_struct() {}
  my_struct(my_struct &&o)
    : s_int(o.s_int), s_vector(std::move(o.s_vector))
  {
  }

  my_struct &operator = (my_struct &&o)
  {
    s_int = (o.s_int);
    s_vector = (std::move(o.s_vector));
    return *this;
  }
  my_struct &operator = (my_struct &o)
  {
    s_int = (o.s_int);
    s_vector = ((o.s_vector));
    return *this;
  }

  int s_int;
  std::vector<int> s_vector;
};

/// \brief another class we want to be serialized / deserialized
class my_class
{
  public:
    /// \brief mandatory constructor
    my_class(int _i, double _s_double, float _s_float, int _s_int) : s_int(_s_int), s_double(_s_double), s_float(_s_float), i(_i)
    {
      for (int i = 0; i < _i; ++i)
      {
        my_struct st;
        for (int j = 0; j < _i; ++j)
          st.s_vector.push_back(j);
        s_map[i] = st;
      }
    }

    ~my_class()
    {
    }

  private:
    /// \brief the post-deserialization function that will be called after deserializing the object
    void post_deserialization(int _i)
    {
      i = _i;
    }

  private: // serialized properties
    int s_int;
    double s_double;
    float s_float;

  public:
    std::map<int, my_struct> s_map;

  private: // non-serialized properties
    int i;

    friend neam::cr::persistence;
};

// the serialization meta-data
namespace neam
{
  namespace cr
  {
    NCRP_DECLARE_NAME(my_class, s_int);
    NCRP_DECLARE_NAME(my_class, s_double);
    NCRP_DECLARE_NAME(my_class, s_float);
    NCRP_DECLARE_NAME(my_class, s_map);
    template<typename Backend> class persistence::serializable<Backend, my_class> : public persistence::constructible_serializable_object
    <
      Backend, // < the backend (here: all backends)

      my_class, // < the class type to handle

      // Embed in the template a call to the post-deserialization function
      // This function will be called just after the object has been deserialized
      N_CALL_POST_FUNCTION(my_class, N_EMBED(42)),

      // simply list here the members you want to serialize / deserialize
      NCRP_NAMED_TYPED_OFFSET(my_class, s_int, names::my_class::s_int),
      NCRP_NAMED_TYPED_OFFSET(my_class, s_double, names::my_class::s_double),
      NCRP_NAMED_TYPED_OFFSET(my_class, s_float, names::my_class::s_float),
      NCRP_NAMED_TYPED_OFFSET(my_class, s_map, names::my_class::s_map)
    > {};

    NCRP_DECLARE_NAME(my_struct, s_int);
    NCRP_DECLARE_NAME(my_struct, s_vector);
    template<typename Backend> class persistence::serializable<Backend, my_struct> : public persistence::serializable_object
    <
      Backend, // < the backend (here: all backends)

      my_struct, // < the class type to handle

      // simply list here the members you want to serialize / deserialize
      NCRP_NAMED_TYPED_OFFSET(my_struct, s_int, names::my_struct::s_int),
      NCRP_NAMED_TYPED_OFFSET(my_struct, s_vector, names::my_struct::s_vector)
    > {};
  } // namespace cr
} // namespace neam


// ///////////////////////////////////////////////
// ///////////////////////////////////////////////
// ///////////////////////////////////////////////

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

using backend = neam::cr::persistence_backend::neam;
constexpr float multiplier = 1;
// #define skip_big

int main()
{
  std::cout << " -- benchmark for backend " << neam::demangle<backend>() << std::endl;
  std::cout << " -- initializing data..." << std::endl;

  my_class my_instance(75, 42.00000042, 4.2e-5, 23);
#ifndef skip_big
  my_class my_big_instance(12000, 42.00000042, 4.2e-5, 23);
#endif
  std::cout << " ----------------\n" << std::endl;


  neam::cr::raw_data rd_small;
  run_test(100000 * multiplier, "serialization of a small object", [&] {rd_small._clean();}, [&]() -> double
  {
    rd_small = neam::cr::persistence::serialize<backend>(my_instance);
    rd_small.assume_ownership();
    return rd_small.size;
  });

#ifndef skip_big
  neam::cr::raw_data rd_big;
  run_test(20 * multiplier, "serialization of a BIG object", [&] {rd_big._clean();}, [&]() -> double
  {
    rd_big = neam::cr::persistence::serialize<backend>(my_big_instance);
    rd_big.assume_ownership();
    return rd_big.size;
  });
#endif

  std::cout << " ----------------\n" << std::endl;

  my_class *ptr = reinterpret_cast<my_class *>(operator new(sizeof(my_class)));
  ptr = neam::cr::persistence::deserialize<backend>(rd_small, ptr);
  if (!ptr)
  {
    std::cerr << "Well, neam::persistence is broken :(" << std::endl;
    return 1;
  }

  run_test(100000 * multiplier, "deserialization of a small object", [&] {ptr->~my_class();}, [&]() -> double
  {
    ptr = neam::cr::persistence::deserialize<backend>(rd_small, ptr);
    return rd_small.size;
  });
  rd_small._clean();

#ifndef skip_big
  run_test(20 * multiplier, "deserialization of a BIG object", [&] {ptr->~my_class();}, [&]() -> double
  {
    ptr = neam::cr::persistence::deserialize<backend>(rd_big, ptr);
    return rd_big.size;
  });
  rd_big._clean();
#endif

  delete ptr;

  return 0;
}
