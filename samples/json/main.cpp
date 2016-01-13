
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
/// \brief create a random data
static inline void random_data(int8_t *data, size_t len)
{
  for (size_t i = 0; i < len; ++i)
    data[i] = static_cast<char>(rand() % 127);
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
      for (size_t i = 0; i < 3; ++i)
      {
        my_struct st;
        st.s_int = 5 - i;
        for (size_t j = 0; j < 3; ++j)
          st.s_vector.push_back(j);
        st.s_string = "this is a lovelely string ! [" + std::to_string(i) + "]";
        s_map[random_string()] = st;
      }
      s_data = new neam::cr::raw_data(50, new int8_t[50]);
      random_data(s_data->data, s_data->size);
    }

    ~my_class()
    {
      delete s_data;
    }

  private: // serialized properties
    int s_int;
    double s_double;
    float s_float;
    bool s_bool = true;

    neam::cr::raw_data *s_data;
    float my_array[3][2] = {{13.f, 55.550054f}, { -.00013f, 15.f}, {1e6f, 1.550e-10f}};

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
    NCRP_DECLARE_NAME(my_class, s_int);
    NCRP_DECLARE_NAME(my_class, s_double);
    NCRP_DECLARE_NAME(my_class, s_float);
    NCRP_DECLARE_NAME(my_class, s_bool);
    NCRP_DECLARE_NAME(my_class, s_data);
    NCRP_DECLARE_NAME(my_class, my_nullptr);
    NCRP_DECLARE_NAME(my_class, my_array);
    NCRP_DECLARE_NAME(my_class, s_map);
    template<typename Backend> class persistence::serializable<Backend, my_class> : public persistence::serializable_object
    <
      Backend, // < the backend (here: all backends)

      my_class, // < the class type to handle

      // simply list here the members you want to serialize / deserialize
      NCRP_NAMED_TYPED_OFFSET(my_class, s_int, names::my_class::s_int),
      NCRP_NAMED_TYPED_OFFSET(my_class, s_double, names::my_class::s_double),
      NCRP_NAMED_TYPED_OFFSET(my_class, s_float, names::my_class::s_float),
      NCRP_NAMED_TYPED_OFFSET(my_class, s_data, names::my_class::s_data),
      NCRP_NAMED_TYPED_OFFSET(my_class, s_bool, names::my_class::s_bool),
      NCRP_NAMED_TYPED_OFFSET(my_class, my_nullptr, names::my_class::my_nullptr),
      NCRP_NAMED_TYPED_OFFSET(my_class, my_array, names::my_class::my_array),
      NCRP_NAMED_TYPED_OFFSET(my_class, s_map, names::my_class::s_map)
    > {};

    NCRP_DECLARE_NAME(my_struct, s_int);
    NCRP_DECLARE_NAME(my_struct, s_string);
    NCRP_DECLARE_NAME(my_struct, s_vector);
    template<typename Backend> class persistence::serializable<Backend, my_struct> : public persistence::serializable_object
    <
      Backend, // < the backend (here: all backends)

      my_struct, // < the class type to handle

      // simply list here the members you want to serialize / deserialize
      NCRP_NAMED_TYPED_OFFSET(my_struct, s_int, names::my_struct::s_int),
      NCRP_NAMED_TYPED_OFFSET(my_struct, s_vector, names::my_struct::s_vector),
      NCRP_NAMED_TYPED_OFFSET(my_struct, s_string, names::my_struct::s_string)
    > {};
  } // namespace cr
} // namespace neam


int main()
{
  my_class my_instance(13, 42.00000042, 4.2e-5, 23);

  // serialize (a start indentation could also be provided as second parameter)
  neam::cr::raw_data serialized_data = neam::cr::persistence::serialize<neam::cr::persistence_backend::json>(my_instance);
  if (!serialized_data.data)
  {
    std::cerr << "Unable to serialize... :(" << std::endl;
    return 1;
  }

  size_t orig_size = serialized_data.size;
  serialized_data.size = strlen((const char *)serialized_data.data);
  std::cout << "the size of the serialized data is: " << serialized_data.size << " bytes [" << orig_size << "]" << std::endl;

  std::cout << "serialized data:\n<" << serialized_data.data << ">\n" << std::endl;

  neam::cr::raw_data serialized;

  serialized.data = (int8_t*)(R"({
  "s_int" : 23,
  "s_double" : 42.000000,
  "s_float" : 0.000042,
  "s_bool" : true
})");
  serialized.size = strlen((const char *)serialized.data);
  serialized.ownership = false;

  using res_type = my_class;
  res_type *d = neam::cr::persistence::deserialize<neam::cr::persistence_backend::json, res_type>(serialized_data);

  neam::cr::raw_data reserialized = neam::cr::persistence::serialize<neam::cr::persistence_backend::json>(d);
  std::cout << "serialized data:\n" << reserialized.data << "\n" << std::endl;

  std::cout << (strcmp((const char *)reserialized.data, (const char *)serialized_data.data) == 0 ? "success :)" : "fail :(") << std::endl;
  delete d;

  return 0;
}
