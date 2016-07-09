
#include <map>
#include <iostream>

#include <persistence/persistence.hpp>

//
// NOTE: You should look to the basic and simple samples before !!
//
// You may not need what that sample will show you, but you never know !!
// It's here, just in case.
// I've made big projects that uses persistence without using any of this,
// but I recon that in some cases it could be helpful.
//

int print_counter = 0;

struct my_struct
{
  int s_int;
  std::vector<int> s_vector;
};

class my_class
{
  public:
    double s_double;
    std::vector<my_struct> s_vector;

    std::vector<double> s_dbl_vector; // only "serialized" with the verbose backend

  private:
    // The post-deserialization function that will be called after deserializing the object
    // This sample (also) shows how to pass instances as parameters
    void post_deserialization(const my_struct &ms)
    {
      new (&s_dbl_vector) std::vector<double>();

      // print the received structure:
      if (print_counter == 0)
      {
        ++print_counter;
        neam::cr::raw_data rd = neam::cr::persistence::serialize<neam::cr::persistence_backend::verbose>(ms);
        std::cout << "my_struct ms:\n" << rd.data << std::endl;
      }
    }

    friend neam::cr::persistence;
};

// the serialization meta-data
namespace neam
{
  namespace cr
  {
    template<typename Backend>
    class persistence::serializable<Backend, my_struct> : public persistence::serializable_object
    <
      Backend, // < the backend (here: all backends)

      my_struct, // < the class type to handle

      NCRP_TYPED_OFFSET(my_struct, s_int),
      NCRP_TYPED_OFFSET(my_struct, s_vector)
    > {};

    // the generic metadata, used across some backends (even those that doesn not exist yet)
    template<typename Backend>
    class persistence::serializable<Backend, my_class> : public persistence::constructible_serializable_object
    <
      Backend, // < the backend (here: all backends)

      my_class, // < the class type to handle

      // If your object doesn't have any parameter to its constructor: use N_CR_EMBED_OBJECT_0(my_class)
      N_CALL_POST_FUNCTION(my_class, N_CR_EMBED_OBJECT(my_struct, N_EMBED(42), N_CR_EMBED_OBJECT(std::vector<int>, N_EMBED(-42), N_EMBED(111)))),


      NCRP_TYPED_OFFSET(my_class, s_double),
      NCRP_TYPED_OFFSET(my_class, s_vector)
    > {};

    // Here we create a special metadata for the verbose backend that will serialize
    // some data that isn't serialized in the generic metadata. (useful to print some info about the class
    // without cluttering your serialized data)
    template<>
    class persistence::serializable<persistence_backend::verbose, my_class> : public persistence::serializable_object
    <
      persistence_backend::verbose, // < the backend (here: JUST the verbose backend)

      my_class, // < the class type to handle

      NCRP_TYPED_OFFSET(my_class, s_double),
      NCRP_TYPED_OFFSET(my_class, s_vector),
      NCRP_TYPED_OFFSET(my_class, s_dbl_vector)
    > {};
  } // namespace cr
} // namespace neam

int main()
{
  my_class my_instance;

  my_instance.s_dbl_vector.resize(5, 0.1);
  my_instance.s_double = -18.2;
  my_instance.s_vector.resize(2, my_struct {30, {1, -2, 3}});

  neam::cr::raw_data rd = neam::cr::persistence::serialize<neam::cr::persistence_backend::neam>(my_instance);
  {

    // Will print the struct that are passed as argument to the post_deserialization callback
    my_class *my_ptr;
    if ((my_ptr = neam::cr::persistence::deserialize<neam::cr::persistence_backend::neam, my_class>(rd)))
      delete my_ptr;
  }

  // And what if I want the control over the deserialized object and its memory ?
  {
    // The allocation_transaction object will hold every allocation made by the deserializer,
    // that way if an error occurs, it can rollback and free all the memory it has allocated.
    // That can be very useful if the deserializer fails while an object is half deserialized !
    neam::cr::allocation_transaction transaction;
    my_class *ptr = reinterpret_cast<my_class *>(transaction.allocate_raw(sizeof(my_class)));

    size_t size = rd.size; // here because of the JSON serializer: it needs a non-const reference !.
    bool result = neam::cr::persistence::serializable<neam::cr::persistence_backend::neam, my_class>::from_memory(transaction, reinterpret_cast<const char *>(rd.data), size, ptr);
    if (!result)
    {
      transaction.rollback();
      std::cout << "deserialization failed !" << std::endl;
      return 0;
    }

    // we have the object in ptr:
    std::cout << "ptr->s_double: " << ptr->s_double << std::endl;

    // A rollback here will free the object and all the memory allocated
    transaction.rollback();


    // This scheme can be very useful if you have simple structures (with no constructors / destructors)
    // that hold a lot of dynamically allocated objects (like a tree or vector of pointers, ...).
    // You can free everything at once, without having to walk the tree, ...
    // Moreover this also works with partially constructed objects whereas destructors will most likely fail in that case.
  }
}
