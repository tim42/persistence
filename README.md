
# neam/persistence


**neam/persistence** is a small utility library that serialize and de-serialize C++ objects.
It provides multiple backends like a JSON serializer, a verbose serializer or a binary serializer/deserializer.

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

neam/persistence also provides a `storage` class that provide the ability to store and retrieve serialized objects to/from a file.

It supports different "backends", chosen at compile time
Current backends:
  - neam (binary)
  - verbose _(serialization only)_ see what is serialized in an human readable format.
    This backend could be usefull to print data easily (instead of manual `std::cout << ... << std::endl;`), to debug a possible problem with a serialized object,
    and to see how neam::persistence works with some C++ types.
  - JSON _(currently: serialization only)_ A JSON serializer for your C++ objects

## performances

Tests ran on an Intel i7, with g++ 4.8.3, in release mode:
  - serialization:
    - **1.11 Gb/s** (size of the serialized data: 1.15 Gb)
    - **1.77 Gb/s** (size of the serialized data: 47.4 Ko)
  - deserialization:
    - **2.30 Gb/s** (size of the serialized data: 1.15 Gb)
    - **1.82 Gb/s** (size of the serialized data: 47.4 Ko)

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

**NOTE**: you're advised to take a look at the samples in the... `samples` folder.

## serialize

```c++
neam::cr::persistence::raw_data serialized;
serialized = neam::cr::persistence::serialize<neam::cr::persistence_backend::neam>(my_object);
```

## deserialize

```c++
my_class *ptr = neam::cr::persistence::deserialize<neam::cr::persistence_backend::neam, my_class>(my_serialized_raw_data);

// ...

delete ptr;
```

## make a very simple class / struct compatible with neam/persistence

**NOTE:** If you want to serialize private members of a class, you have to add this statement `friend neam::cr::persistence;` in the class to serialize

_you should take a look at the `simple` sample ;)_


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
  } // namespace r
} // namespace neam


```

## make a class / struct compatible with neam/persistence (and call a post-deserialization function on deserialized objects)

**NOTE:** constructors aren't called because the compiler will want to initialize **all** the members of your class
and in the case of a post-deserialization, where members (possibly objects with dynamic allocation) have been already initialized,
this would result in a massive memory leak and revert the object in a default state.

**NOTE:** _[important note]_ : This function is mandatory if you have _complex_ (like `std::vector` or any other classes)
types in your class and those members aren't part of the serialization process. You have to class the placement new operator on those members
(example for a std::vector: `operator new(&m_int_vector) std::vector();`. This won't allocate memory, simply call the constructor of the object).

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

## using _neam::cr::storage_

```c++
neam::cr::storage storage("path/to/my-storage-file.ndb");

// check if the file contains valid data or if it contains a specific object
if (!storage.is_valid() || !storage.contains("exemple/object"))
{
	// no: initialize the file with a default object:
	storage.write_to_file("exemple/object", my_class());
}

// the file here is initialized and contains an object named "exemple/object".
// retrieve it:
my_class *ptr = storage.load_from_file<my_class>("exemple/object");

// test if the pointer is valid:
if (!ptr)
{
	// re-initialize the object (overwrite its previous invalid content)
	storage.write_to_file("exemple/object", my_class());
	// reload it:
	ptr = storage.load_from_file<my_class>("exemple/object");
	if (!ptr)
		abort(); // do whatever you want: the problem here shouldn't happen
}

// update things in the object
ptr->update_object();

// update the content of the file
storage.write_to_file("exemple/object", *ptr);

// free the memory
delete ptr;
```

## future / TODO

- Add a SpiderMonkey backend
- Add deserialization to the JSON backend
- Improve the way std::map/std::unordered_map are handled in the verbose / neam backends
- remove the extra-copy (create the element in-place). (could be a lot faster on the deserialization benchmark).


## author
Timothée Feuillet (_neam_ or _tim42_).
