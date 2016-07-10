
#include <iostream>
#include <vector>
#include <map>

#include <persistence/persistence.hpp>
#include <persistence/stl/vector.hpp>

#include <persistence/tools/uninitialized.hpp>

//
// NOTE: You should look the 'basic' sample before this one
//


/// \brief a class we want to be serialized / deserialized
template<typename First, typename Second>
class my_class
{
  public:
    int s_int;
    std::vector<std::pair<First, Second>> s_pair_vector;

    std::vector<First> vector; // This will NOT be de/serialized

  private:
    /// \brief the post-deserialization function that will be called after deserializing the object
    void post_deserialization(unsigned int vector_size)
    {
      // call the constructor of `vector`
      new (&vector) std::vector<First>();

      // do some things:
      vector.resize(vector_size, 0x5A);

      std::cout << " -- Called !!" << std::endl;
    }

    friend neam::cr::persistence;
};

// the serialization meta-data
namespace neam
{
  namespace cr
  {
    // This is how you declare names for members.
    // NOTE: If your class is in a namespace, a good practice is to prefix the name
    //       with the namespaces like that:
    //       NCRP_DECLARE_NAME(my_namespace__my_class, s_int);
    NCRP_DECLARE_NAME(my_class, s_int);
    NCRP_DECLARE_NAME(my_class, s_pair_vector);

    // As my_class is a template class and has more than one parameter, the coma
    // will mess with the macros, so to work around that, there's the NCRP_TEMPLATE_CLASS() marco.
    //
    // NOTE: In this sample we use a std::pair<..., ...>, but persistence already comes with
    //       a metadata for std::pair. If you plan to indirectly serialize / deserialize classes,
    //       you'll have to provide metadata for those too.
    //
    // NOTE: notice the `peristence::constructible_serializable_object`, and not the `peristence::serializable_object`
    //       as we want to call the post_deserialization callback.
    template<typename Backend, typename First, typename Second>
    class persistence::serializable<Backend, my_class<First, Second>> : public persistence::constructible_serializable_object
    <
      Backend, // < the backend (here: all backends)

      my_class<First, Second>, // < the class type to handle

      // This is how you call the post deserialization callback.
      // The callback can have the signature you want, throw exception, ...
      // The only thing is you have to provide **values** for its arguments.
      // (can be objects, with the constructor call mechanism).
      // Just keep in mind that this is a template thing, so floating points may not be
      // as easy as you want to use in here. (same goes for instances, ...).
      //
      // To deal with that, there's EMBED and CONSTRUCTOR_CALL (in the persistence/tools folder).
      // The advanced sample show that.
      //
      // The line below is equivalent to calling `my_class<First, Second>::post_deserialization(42)`
      N_CALL_POST_FUNCTION(NCRP_TEMPLATE_CLASS(my_class<First, Second>), N_EMBED(42)),

      // simply list here the members you want to serialize / deserialize
      // Notice the NCRP_NAMED_TYPED_OFFSET(), as we also want to provide names for the members
      NCRP_NAMED_TYPED_OFFSET(NCRP_TEMPLATE_CLASS(my_class<First, Second>), s_int, names::my_class::s_int),
      NCRP_NAMED_TYPED_OFFSET(NCRP_TEMPLATE_CLASS(my_class<First, Second>), s_pair_vector, names::my_class::s_pair_vector)
    > {};
  } // namespace cr
} // namespace neam


int main()
{
  // This is totaly legit, and persistence will work fine with this:
  my_class<int, my_class<int, double>> my_instance;

  // Initialize that instance:
  my_instance.s_int = 88;
  my_instance.s_pair_vector.resize(3);
  my_instance.s_pair_vector[0].first = 7;
  my_instance.s_pair_vector[0].second.s_int = 5;
  my_instance.s_pair_vector[0].second.s_pair_vector.emplace_back(1, 0.5);
  my_instance.s_pair_vector[1].first = 6;
  my_instance.s_pair_vector[1].second.s_int = 6;
  my_instance.s_pair_vector[1].second.s_pair_vector.emplace_back(-1, 1.5);
  my_instance.s_pair_vector[2].first = 5;
  my_instance.s_pair_vector[2].second.s_int = 7;
  my_instance.s_pair_vector[2].second.s_pair_vector.emplace_back(1, -0.5);

  // There's multiple backends in persistence, and you can totaly write your own backend.
  // The default (faster, better, stronger) backend is the `neam` one, but a `verbose` (that can only serialize)
  // and a JSON (slow, not so well written, but that works) backend also exists:
  neam::cr::raw_data rd = neam::cr::persistence::serialize<neam::cr::persistence_backend::verbose>(my_instance);

  // The verbose backend is made to be printed (is null terminated), so here we go !
  std::cout << "my_instance (VERBOSE):\n" << rd.data << std::endl;

  // Now the json backend (that can also be printed / is null terminated):
  rd = neam::cr::persistence::serialize<neam::cr::persistence_backend::json>(my_instance);

  std::cout << "my_instance (JSON):\n" << rd.data << std::endl;

  // As JSON can also be deserialized:
  {
    neam::cr::uninitialized<my_class<int, my_class<int, double>>> my_copy_instance;
    neam::cr::persistence::deserialize<neam::cr::persistence_backend::json>(rd, &my_copy_instance);

    // As the object is fully constructed (thanks to the post_deserialization callback)
    // We can allow the uninitialized<> object to destruct the instance it keeps
    my_copy_instance.call_destructor(true);

    std::cout << " -- size of my_copy_instance.vector: " << my_copy_instance->vector.size() << std::endl;
  }

  // The `neam` backend can't be printed (is binary), but is significantly faster than
  // the other backends:
  rd = neam::cr::persistence::serialize<neam::cr::persistence_backend::neam>(my_instance);

  std::cout << "\nmy_instance (NEAM): binary data of " << rd.size << " bytes" << std::endl;

  {
    neam::cr::uninitialized<my_class<int, my_class<int, double>>> my_copy_instance;
    neam::cr::persistence::deserialize<neam::cr::persistence_backend::neam>(rd, &my_copy_instance);
    my_copy_instance.call_destructor(true);

    std::cout << " -- size of my_copy_instance.vector: " << my_copy_instance->vector.size() << std::endl;
  }

  return 0;
}
