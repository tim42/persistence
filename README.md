
# neam/persistence


**neam/persistence** is a small utility library that serialize and de-serialize C++ objects.
It provides multiple backends like a JSON serializer/deserializer, a verbose serializer or a binary serializer/deserializer.

## how to build

```bash
$ git submodule init
$ git submodule update
$ mkdir build && cd build && cmake ..
$ make
```

The library is in `libs/`, the samples are in the current folder.

## why it's different

neam/persistence is a non-intrusive, external serialization utility.
The traditional C++ approach is to inherit a given class from a serializer interface that provide at least two function `serialize` and `deserialize`, and its the duty of the class to serialize itself.
Their is also the dumb solution to dump the memory of the object directly (but this is not portable, even between version of the same compiler).

Both solutions difficultly handle pointers (and the latter doesn't even handle it), dynamic allocation, complex data types (like an object hierarchy, with arrays, dictionaries, ...).

neam/persistence is a _non-intrusive_ serializer. This mean it won't need you to write a `serialize` and a `deserialize` function in the class, and the class --mostly-- doesn't need to be changed.

neam/persistence is an _external_ serializer: it works outside of the class to serialize, you just have to provide the name of the properties you want to serialize.

neam/persistence provides multiple backends: with the same metadata, you can serialize and deserialize to multiple format (like JSON, binary, ...). Moreover, backends are easily extensible. You can either create your own backend or extend an existing backend.


## features

neam/persistence provides some "built-in" serializers for:
  - std::map
  - std::unordered_map
  - std::vector
  - std::basic_string (so `std::string` too, but not only)
  - C arrays (`int m_int_array[500];`, including multi-dimensional arrays)
  - pointers (using dynamic allocation), and null pointers
  - a generic object serializer (can serialize objects properties by properties)
  - neam::array_wrapper
  - neam::persistence::raw_data (almost like a `cr::array_wrapper`)

For objects requesting it, a post-serialization function could be called after the serialization of the object has been done (and has been successful).
As the constructor won't be called, this is particularly useful. (the constructor won't be called because the compiler will default initialize all the fields, possibly overwriting deserialized values)

neam/persistence also includes some _wrappers_: _(a code that wrap the generated data and perform some actions)_
  - checksum (a custom, handcrafted, non-secure but quite fast hashing function)
  - magic number (simply add a magic number)
  - a xor wrapper that xor the data to possibly obfuscate it a little bit (it uses a seedable PRNG to generate the sequence to xor the data with)

neam/persistence also provides a `storage` class that provide the ability to store and retrieve serialized objects to/from a file.

It supports different "backends", chosen at compile time
Current backends:
  - neam (binary)
  - JSON: A JSON serializer and deserializer for your C++ objects
  - verbose _(serialization only)_ see what is serialized in an human readable format.
    This backend could be usefull to print data easily (instead of manual `std::cout << ... << std::endl;`), to debug a possible problem with a serialized object,
    and to see how neam::persistence works with some C++ types.

## performances

Tests ran on an Intel i7, with g++ 4.8.3, in release mode with the binary backend
  - serialization:
    - **1.16 Gb/s** (size of the serialized data: 1.15 Gb)
    - **1.34 Gb/s** (size of the serialized data: 47.4 Ko)
  - deserialization:
    - **1.62 Gb/s** (size of the serialized data: 1.15 Gb)
    - **1.37 Gb/s** (size of the serialized data: 47.4 Ko)

**NOTE**: due to some recent changes in the architecture, neam::persistence has lost some speed (at most 1Gb/s on the deserialization), but it should be easily improved in the future.
Also, the JSON backend is not finished yet and is really **dumb** (it processes multiple time the same data, perform a lot of small operation instead of a big, faster one) 
so there's a LOT of room for improvement here.

Numbers taken from the **benchmark** sample.

## pitfalls

Thinking that neam/persistence is magic. C++ isn't a dynamic language, but instead works quite _near the metal_
(exemple: the size/format of some integers/floating point types depends of the underlying architecture).

neam/persistence doesn't handle loops, references (or non-dynamically allocated pointers). If you simply try to serialize this, you will either fall in one (or more) of those cases:
  - a stack overflow
  - running out of process memory
  - serialization will produce a data that doesn't represent the original object (and this will possibly includes some memory leaks as it will have dynamically allocated some memory that won't be freed by the destructor)

neam/persistence is the perfect solution to serialize tree-like hierarchies (of all kind and complexity) but the worst solution (in the sense that it won't even work) to serialize graphs.


Using the neam binary backend, you can serialize on a machine an deserialize on another if and only if:
- floating points have the same size/format (32bit for floats, 64 for doubles), and share the same endianness as integers
- you used exactly the same serialization metadata on the two machines / programs

Endianness (floating points + integers) and size differences (integers) are handled transparently.

-------


# Examples

**NOTE**: you're advised to take a look at the samples in the `samples` folder.

## serialize

```c++
neam::cr::persistence::raw_data serialized;
serialized = neam::cr::persistence::serialize<neam::cr::persistence_backend::neam /*JSON, verbose */>(&my_object);
```

## deserialize

```c++
my_class *ptr = neam::cr::persistence::deserialize<neam::cr::persistence_backend::neam /*JSON, verbose */, my_class>(my_serialized_raw_data);

// ...

delete ptr;
```

## make a very simple class / struct compatible with neam/persistence

**NOTE:** If you want to serialize private members of a class, you have to add this statement `friend neam::cr::persistence;` in the class to serialize

_you should take a look at the `simple` or the `storage` samples ;)_


```C++

// this is the struct your want to make serializable:
struct my_struct
{
  double my_double;
  float my_float;
  std::map<std::string, std::vector<int>> my_map_of_vector;
};

// this is how you make it serializable:
// (this is what I called 'serialization metadata')
namespace neam
{
  namespace cr
  {
      template<typename Backend> class persistence::serializable<Backend, my_struct>
      : public serializable_object
      <
        Backend, // we want it on all the available backends

        // simply list here the members you want to serialize / deserialize
        NCRP_TYPED_OFFSET(my_struct, my_double),
        NCRP_TYPED_OFFSET(my_struct, my_float),
        NCRP_TYPED_OFFSET(my_struct, my_map_of_vector)
      > {};
  } // namespace cr
} // namespace neam


```

## make a class / struct compatible with neam/persistence (and call a post-deserialization function on deserialized objects)

**NOTE:** constructors aren't called because the compiler will want to initialize **all** the members of your class
and in the case of a post-deserialization, where members (possibly objects with dynamic allocation) have been already initialized,
this would result in a massive memory leak and revert the object in a default state.

**NOTE:** _[important note]_ : This function is mandatory if you have _complex_ (like `std::vector` or any other classes)
types in your class and those members aren't part of the serialization process. You have to class the placement new operator on those members
(example for a std::vector: `new(&m_int_vector) std::vector<int>();`. This won't allocate memory, simply call the constructor of the object).

```C++
// this is the struct your want to make serializable:
struct my_struct
{
  double my_double;
  float my_float;
  std::map<std::string, std::vector<int>> my_map_of_vector;

  int *non_serialized;

  // the constructor
  my_struct(size_t size) : non_serialized(new int[size]) {}

  // the post-deserialization function
  void post_deserialization(size_t size)
  {
     non_serialized = new int[size];
  }

  // and the destructor. (DO NOT FORGET IT IF YOU DO DYNAMIC ALLOCATION !!!)
  ~my_struct() {delete [] non_serialized;}
};

// this is how you make it serializable:
namespace neam
{
  namespace cr
  {
      template<typename Backend> class persistence::serializable<Backend, my_struct>
      : public constructible_serializable_object
      <
        Backend, // < we want it on all backends

        // Embed in the template a call to the post-deserialization function
        // This function will be called just after the object has been deserialized
        N_CALL_POST_FUNCTION(my_struct, N_EMBED(50)),

        // simply list here the members you want to serialize / deserialize
        NCRP_TYPED_OFFSET(my_struct, my_double),
        NCRP_TYPED_OFFSET(my_struct, my_float),
        NCRP_TYPED_OFFSET(my_struct, my_map_of_vector)
      > {};
  } // namespace r
} // namespace neam


```

## future / TODO

- Improve the JSON backend to avoid that dumb way of processing the size (pass a reference that is set by simple types (integers, strings, ...) and updated all along the hierarchy. It would be a **LOT** faster)
- Improve the neam (binray) backend to retrieve the same performances as before
- Add a SpiderMonkey backend

## author
Timothée Feuillet (_neam_ or _tim42_).
