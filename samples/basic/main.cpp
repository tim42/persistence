
#include <iostream>
#include <vector>
#include <fstream>

#include <persistence/persistence.hpp>
#include <persistence/tools/uninitialized.hpp>

/// \brief a class we want to be serialized / deserialized
struct my_struct
{
  int s_int;
  std::vector<int> s_vector;
};

// the serialization meta-data
namespace neam
{
  namespace cr
  {
    // This is a metadata for persistence. This construct will tell persistence what members
    // to serialize and deserialize.
    //
    // If some of those members are private, in your class put a
    // `friend neam::cr::persistence;`.
    //
    // If you don't want to serialize all of the members of the class, you'll have to use
    // a post deserialization callback that will call the constructors of the members
    // The "simple" sample shows how to deal with that.
    //
    // NOTE: You can totaly generate "generic" serialization metadata for template classes.
    // The "simple" sample shows how to deal with that.
    template<typename Backend>
    class persistence::serializable<Backend, my_struct> : public persistence::serializable_object
    <
      Backend, // < the backend (here: all backends)

      my_struct, // < the class type to handle

      // simply list here the members you want to serialize / deserialize
      //
      // Some backends (like json or verbose) will ask a name for the property.
      // The "simple" sample shows how to deal with that.
      NCRP_TYPED_OFFSET(my_struct, s_int),
      NCRP_TYPED_OFFSET(my_struct, s_vector)
    > {};
  } // namespace cr
} // namespace neam


int main()
{
  // This is the instance we want to serialize and deserialize
  my_struct my_instance;

  my_instance.s_int = 24;
  my_instance.s_vector.resize(10, 0x5A5A5A5A);

  // We serialize my_instance. This will create a neam::cr::raw_data,
  // that is the result of the serialization (it contains a data and a size fields).
  neam::cr::raw_data rd = neam::cr::persistence::serialize<neam::cr::persistence_backend::neam>(my_instance);

  {
    // We create a non-initialized instance of my_struct.
    // If we have it already initialized (if the constructor has been called),
    // we would have to call the destructor to avoid memory leaks.
    neam::cr::uninitialized<my_struct> my_copy_instance;

    // This will deserialize 'in place' the content of rd
    neam::cr::persistence::deserialize<neam::cr::persistence_backend::neam>(rd, &my_copy_instance);

    // As we have now an initialized instance, we have to tell the uninitialized<> object
    // to call the destructor when its life ends.
    my_copy_instance.call_destructor(true);


    // We can do whatever we want on the object now:
    my_copy_instance->s_int += 1;
    my_copy_instance->s_vector.reserve(10);
    // ...
  }

  {
    // This will not deserialize over an existing object, but instead will allocate a new one.
    // (notice the my_struct as template parameter)
    my_struct *my_copy_ptr = neam::cr::persistence::deserialize<neam::cr::persistence_backend::neam, my_struct>(rd);

    // We can do whatever we want on the object now:
    my_copy_ptr->s_int += 1;
    my_copy_ptr->s_vector.reserve(10);
    // ...

    // Don't forget to delete the object (or use a shared/unique ptr):
    delete my_copy_ptr;
  }


  {
    // If you plan to write the data to a file/stream/..., this is how it could be done:
    std::ofstream file("./temp-file.tmp", std::ios::binary);

    file.write(reinterpret_cast<char *>(rd.data), rd.size);
  }

  {
    // If you want to load the data from a file/stream/..., this is how it could be done:
    std::ifstream file("./temp-file.tmp", std::ios::binary | std::ios::ate);

    // get the size of the file & allocate the memory to hold the data
    size_t size = file.tellg();
    file.seekg(0, std::ios::beg);
    int8_t *data = reinterpret_cast<int8_t *>(operator new(size));

    // read the file
    file.read(reinterpret_cast<char *>(data), size);

    // create the raw_data object. Note the `neam::assume_ownership`
    // that tells the raw_data to free the memory at the end of its life
    neam::cr::raw_data rd_from_file(size, data, neam::assume_ownership);


    // We can now deserialize it:
    my_struct *my_copy_ptr = neam::cr::persistence::deserialize<neam::cr::persistence_backend::neam, my_struct>(rd_from_file);
    delete my_copy_ptr;
  }

  return 0;
}
