
#include <unistd.h>
#include <cstddef>
#include <cmath>
#include <cstdlib>
#include <map>
#include <iostream>

#include <persistence/persistence.hpp>

/// \brief the class we want to be serialized / deserialized
class my_class
{
  public:
    /// \brief mandatory constructor
    my_class(int _i, double _s_double, float _s_float, int _s_int) : s_int(_s_int), s_double(_s_double), s_float(_s_float), i(_i) {}


    void print(const std::string &str) const
    {
      std::cout << str << " my_class: [s_int: " << s_int << ", s_double: " << s_double << ", s_float: " << s_float << ", i: " << i << "]" << std::endl;
    }

    void increment()
    {
      ++s_int;
    }

  private:
    /// \brief the function that will be called when deserializing the object
    void post_deserialization(int _i, neam::cr::from_serialization_t)
    {
      i = _i;
    }

  private: // serialized properties
    int s_int;
    double s_double;
    float s_float;

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
      Backend,

      // Embed in the template a call to the post-deserialization function
      // This function will be called just after the object has been deserialized
      N_CALL_POST_FUNCTION(my_class, N_EMBED(42), neam::cr::from_serialization),

      // simply list here the members you want to serialize / deserialize
      NCRP_TYPED_OFFSET(my_class, s_int),
      NCRP_TYPED_OFFSET(my_class, s_double),
      NCRP_TYPED_OFFSET(my_class, s_float)
    > {};
  } // namespace cr
} // namespace neam


void init_storage(neam::cr::storage &storage)
{
  my_class my_instance(13, 42.00000042, 4.2e-5, 0);
  my_instance.print("original:\n  ");

  storage.write_to_file("sample/storage/instance", my_instance);
}

int main()
{
  neam::cr::storage storage("samples.storage");

  if (!storage.is_valid() || !storage.contains("sample/storage/instance"))
    init_storage(storage);

  // deserialize stored data:
  my_class *ptr = storage.load_from_file<my_class>("sample/storage/instance");
  if (!ptr)
  {
    std::cerr << "Unable to de-serialize... :(" << std::endl;
    return 1;
  }

  ptr->print("deserialized:\n  ");
  ptr->increment();

  // update it:
  storage.write_to_file("sample/storage/instance", *ptr);

  delete ptr;

  return 0;
}
