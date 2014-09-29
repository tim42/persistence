
#include <unistd.h>
#include <cstddef>
#include <cmath>
#include <cstdlib>
#include <map>
#include <iostream>

#include <persistence/persistence.hpp>

/// \brief create a random string
static inline std::string random_string(size_t min_len = 5)
{
  const size_t rand_len = min_len + rand() % 15;

  std::string ret;
  ret.reserve(rand_len);
  for (size_t i = 0; i < rand_len; ++i)
    ret += static_cast<char>(rand() % 93 + 32);
  return ret;
}

/// \brief a class we want to be serialized / deserialized
struct my_struct
{
  int s_int;
  std::string s_string;
  std::vector<int> s_vector;
};

/// \brief another class we want to be serialized / deserialized
class my_class
{
  public:
    /// \brief mandatory constructor
    my_class(int _i, double _s_double, float _s_float, int _s_int) : s_int(_s_int), s_double(_s_double), s_float(_s_float), i(_i)
    {
      for (size_t i = 0; i < 5; ++i)
      {
        my_struct st;
        st.s_int = 5 - i;
        for (size_t j = 0; j < 5; ++j)
          st.s_vector.push_back(j);
        st.s_string = "this is a lovelely string ! [" + std::to_string(i) + "]";
        s_map[random_string()] = st;
      }
      s_data = new neam::cr::raw_data(sizeof(my_class), reinterpret_cast<int8_t *>(this));
    }

    ~my_class()
    {
      delete s_data;
    }

  private: // serialized properties
    int s_int;
    double s_double;
    float s_float;

    neam::cr::raw_data *s_data;
    float my_array[3][2] = {{13.f, 55.550054f}, {-.00013f, 15.f}, {1e6f, 1.550e-10f}};

    std::map<std::string, my_struct> s_map;

    int *my_nullptr = nullptr;

  private: // non-serialized properties
    int i;

    friend neam::cr::persistence;
};

// the serialization meta-data
namespace neam
{
  namespace cr
  {
    template<typename Backend> class persistence::serializable<Backend, my_class> : public persistence::serializable_object
    <
      Backend, // < the backend (here: all backends)

      // simply list here the members you want to serialize / deserialize
      NCRP_TYPED_OFFSET(my_class, s_int),
      NCRP_TYPED_OFFSET(my_class, s_double),
      NCRP_TYPED_OFFSET(my_class, s_float),
      NCRP_TYPED_OFFSET(my_class, s_data),
      NCRP_TYPED_OFFSET(my_class, my_nullptr), // won't appear in the serialization output ;)
      NCRP_TYPED_OFFSET(my_class, my_array),
      NCRP_TYPED_OFFSET(my_class, s_map)
    > {};

    template<typename Backend> class persistence::serializable<Backend, my_struct> : public persistence::serializable_object
    <
      Backend, // < the backend (here: all backends)

      // simply list here the members you want to serialize / deserialize
      NCRP_TYPED_OFFSET(my_struct, s_int),
      NCRP_TYPED_OFFSET(my_struct, s_string),
      NCRP_TYPED_OFFSET(my_struct, s_vector)
    > {};
  } // namespace cr
} // namespace neam


int main()
{
  my_class my_instance(13, 42.00000042, 4.2e-5, 23);

  // serialize (a start indentation could also be provided as second parameter)
  neam::cr::raw_data serialized_data = neam::cr::persistence::serialize<neam::cr::persistence_backend::verbose>(my_instance);
  if (!serialized_data.data)
  {
    std::cerr << "Unable to serialize... :(" << std::endl;
    return 1;
  }

  std::cout << "the size of the serialized data is: " << serialized_data.size << " bytes" << std::endl;

  std::cout << "serialized data:\n" << serialized_data.data << "\n" << std::endl;

  return 0;
}
