
#include <unistd.h>
#include <cstddef>
#include <cmath>
#include <cstdlib>
#include <map>
#include <iostream>

#include <persistence/persistence.hpp>

/// \brief a class we want to be serialized / deserialized
struct my_struct
{
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
      for (size_t i = 0; i < 2121; ++i)
      {
        my_struct st;
        for (size_t j = 0; j < 100; ++j)
          st.s_vector.push_back(j);
        s_map[i] = st;
      }
    }

    void print(const std::string &str) const
    {
      std::cout << str << " my_class: [s_int: " << s_int << ", s_double: " << s_double << ", s_float: " << s_float << ", i: " << i << "]" << std::endl;
    }

  private:
    /// \brief the function that will be called when deserializing the object
    void post_deserialization(int _i)
    {
      i = _i;
    }

  private: // serialized properties
    int s_int;
    double s_double;
    float s_float;

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
    template<typename Backend> class persistence::serializable<Backend, my_class> : public persistence::constructible_serializable_object
    <
      Backend, // < the backend (here: all backends)

      my_class, // < the class type to handle

      // Embed in the template a call to the post-deserialization function
      // This function will be called just after the object has been deserialized
      N_CALL_POST_FUNCTION(my_class, N_EMBED(42)),

      // simply list here the members you want to serialize / deserialize
      NCRP_TYPED_OFFSET(my_class, s_int),
      NCRP_TYPED_OFFSET(my_class, s_double),
      NCRP_TYPED_OFFSET(my_class, s_float)
    > {};

    template<typename Backend> class persistence::serializable<Backend, my_struct> : public persistence::constructible_serializable_object
    <
      Backend, // < the backend (here: all backends)

      my_struct, // < the class type to handle

      // simply list here the members you want to serialize / deserialize
      NCRP_TYPED_OFFSET(my_struct, s_int),
      NCRP_TYPED_OFFSET(my_struct, s_vector)
    > {};
  } // namespace cr
} // namespace neam


int main()
{
  my_class my_instance(13, 42.00000042, 4.2e-5, 23);
  my_instance.print("original:\n  ");

  // serialize
  neam::cr::raw_data serialized_data = neam::cr::persistence::serialize<neam::cr::persistence_backend::neam>(my_instance);
  if (!serialized_data.data)
  {
    std::cerr << "Unable to serialize... :(" << std::endl;
    return 1;
  }

  std::cout << "the size of the serialized instance is: " << serialized_data.size << " bytes" << std::endl;

  // deserialize
  my_class *ptr = neam::cr::persistence::deserialize<neam::cr::persistence_backend::neam, my_class>(serialized_data);
  if (!ptr)
  {
    std::cerr << "Unable to de-serialize... :(" << std::endl;
    return 1;
  }

  ptr->print("deserialized:\n  ");
  delete ptr;

  return 0;
}
