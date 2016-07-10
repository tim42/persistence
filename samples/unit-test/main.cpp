
#include <map>
#include <iostream>

#include <persistence/persistence.hpp>
#include <persistence/stl.hpp> // I will test the whole STL thing, so yay, I can include this header

#include <persistence/tools/uninitialized.hpp>
#include <persistence/tools/logger/logger.hpp>


//
// NOTE: This is not really a sample, but a unit-test for persistence.
//       It does not really use any framework, but allow me to know when something
//       is broken and with the help of valgrind if some leaks are present.
//
//       (I chose not to use a framework 'cause I didn't want a dependency for persistence
//        and that way I can create backend agnostic & container agnostic tests)
//
//       Too bad I can't use reflective here :/
//

struct _end_test {};
#define _success(t, dt)       neam::cr::out.log() << LOGGER_INFO << #t " " #dt << " success !" << std::endl;
#define run_test(t, dt, i)    try { t<dt, i<dt> >(); _success(t, dt); } \
                              catch (_end_test &e) {} \
                              catch (std::exception &e) { neam::cr::out.error() << LOGGER_INFO << #t "" #dt << ": exception " << e.what() << std::endl; }

#define fail(msg)             do { neam::cr::out.error() << LOGGER_INFO  << __FUNCTION__ << ": " << msg << std::endl; throw _end_test(); } while(0)
#define fail_if(cond, msg)    do { if (cond) fail(msg); } while (0)

#define CRAP__VAR_TO_STRING(var)              static_cast<const std::ostringstream&>(std::ostringstream() << var).str()

/// \brief This will test the STL gen-specs with any backend that handle deserialization & serialization
template<typename Backend>
class stl_basic_test
{
  private:
    using payload_t = std::vector<std::pair<std::string, std::string>>;

  public:

    static void run()
    {
      neam::cr::out.log() << LOGGER_INFO << "running test 'stl_basic_test' with backend: " << neam::demangle<Backend>() << std::endl;

      // std test
      run_test(basic, std::vector<int>, init_for_int);
      run_test(basic, std::vector<std::string>, init_for_string);

      using int_array_5000 = std::array<int, 5000>;
      using string_array_5000 = std::array<std::string, 5000>;
      run_test(basic, int_array_5000, init_for_int_inplace);
      run_test(basic, string_array_5000, init_for_string_inplace);

      run_test(basic, std::list<int>, init_for_int);
      run_test(basic, std::list<std::string>, init_for_string);

      run_test(basic, std::deque<int>, init_for_int);
      run_test(basic, std::deque<std::string>, init_for_string);

      run_test(basic, std::forward_list<int>, init_for_int_front);
      run_test(basic, std::forward_list<std::string>, init_for_string_front);

      run_test(basic, std::set<int>, init_for_int_insert);
      run_test(basic, std::set<std::string>, init_for_string_insert);

      run_test(basic, std::unordered_set<int>, init_for_int_insert);
      run_test(basic, std::unordered_set<std::string>, init_for_string_insert);

      using int_int_map = std::map<int, int>;
      using string_string_map = std::map<std::string, std::string>;
      run_test(basic, int_int_map, init_for_int_int_insert);
      run_test(basic, string_string_map, init_for_string_string_insert);

      using int_int_unordered_map = std::unordered_map<int, int>;
      using string_string_unordered_map = std::unordered_map<std::string, std::string>;
      run_test(basic, int_int_unordered_map, init_for_int_int_insert);
      run_test(basic, string_string_unordered_map, init_for_string_string_insert);

      // payload test:
      run_test(basic, std::vector<payload_t>, p_init_pb);

      using p_array_200 = std::array<payload_t, 200>;
      run_test(basic, p_array_200, p_init_inplace);


      run_test(basic, std::list<payload_t>, p_init_pb);
      run_test(basic, std::deque<payload_t>, p_init_pb);
      run_test(basic, std::forward_list<payload_t>, p_init_pf);
      run_test(basic, std::set<payload_t>, p_init_ins);

      using string_p_map = std::map<std::string, payload_t>;
      using p_string_map = std::map<payload_t, std::string>;
      run_test(basic, string_p_map, p_init_sp_insert);
      run_test(basic, p_string_map, p_init_ps_insert);

      using string_p_unordered_map = std::unordered_map<std::string, payload_t>;
      run_test(basic, string_p_unordered_map, p_init_sp_insert);

      neam::cr::out.log() << std::endl;
    }

  private:
    template<typename Container, void (*InitFunc)(Container &)>
    static void basic()
    {
      Container vct;

      // initialize
      InitFunc(vct);

      // serialize
      neam::cr::raw_data rd = neam::cr::persistence::serialize<Backend>(vct);
      fail_if(!rd.size, "serialization failed");

      // deserialize
      neam::cr::uninitialized<Container> comp_vct;
      void *ret = neam::cr::persistence::deserialize<Backend>(rd, &comp_vct);
      fail_if(!ret, "deserialization failed");

      comp_vct.call_destructor(true);

      fail_if(vct != comp_vct.get(), "deserialization failed: results are differents");
    }

  private:
    // std
    template<typename Container>
    static void init_for_int(Container &c) { for (size_t i = 0; i < 10000; ++i) c.push_back(i - 5000); }
    template<typename Container>
    static void init_for_string(Container &c) { for (size_t i = 0; i < 10000; ++i) c.push_back(CRAP__VAR_TO_STRING(i - 5000)); }
    template<typename Container>
    static void init_for_int_front(Container &c) { for (size_t i = 0; i < 10000; ++i) c.push_front(i - 5000); }
    template<typename Container>
    static void init_for_string_front(Container &c) { for (size_t i = 0; i < 10000; ++i) c.push_front(CRAP__VAR_TO_STRING(i - 5000)); }
    template<typename Container>
    static void init_for_int_insert(Container &c) { for (size_t i = 0; i < 10000; ++i) c.insert(i - 5000); }
    template<typename Container>
    static void init_for_string_insert(Container &c) { for (size_t i = 0; i < 10000; ++i) c.insert(CRAP__VAR_TO_STRING(i - 5000)); }
    template<typename Container>
    static void init_for_int_int_insert(Container &c) { for (size_t i = 0; i < 10000; ++i) c.insert(std::make_pair(i - 5000, 5000 - i)); }
    template<typename Container>
    static void init_for_string_string_insert(Container &c) { for (size_t i = 0; i < 10000; ++i) c.insert(std::make_pair(CRAP__VAR_TO_STRING(i - 5000), CRAP__VAR_TO_STRING(5000 - i))); }
    template<typename Container>
    static void init_for_int_inplace(Container &c) { for (size_t i = 0; i < c.size(); ++i) c[i] = (i - 5000); }
    template<typename Container>
    static void init_for_string_inplace(Container &c) { for (size_t i = 0; i < c.size(); ++i) c[i] = (CRAP__VAR_TO_STRING(i - 5000)); }

    // payload
    static payload_t init_payload(size_t seed)
    {
      payload_t p;
      for (size_t i = 0; i < seed; ++i)
        p.push_back(std::make_pair(CRAP__VAR_TO_STRING((i * seed)), CRAP__VAR_TO_STRING((~i ^ seed))));
      return p;
    }
    template<typename Container>
    static void p_init_pb(Container &c) { for (size_t i = 0; i < 200; ++i) c.push_back(init_payload(i)); }
    template<typename Container>
    static void p_init_pf(Container &c) { for (size_t i = 0; i < 200; ++i) c.push_front(init_payload(i)); }
    template<typename Container>
    static void p_init_ins(Container &c) { for (size_t i = 0; i < 200; ++i) c.insert(init_payload(i)); }
    template<typename Container>
    static void p_init_inplace(Container &c) { for (size_t i = 0; i < c.size(); ++i) c[i] = (init_payload(i)); }
    template<typename Container>
    static void p_init_sp_insert(Container &c) { for (size_t i = 0; i < 200; ++i) c.insert(std::make_pair(CRAP__VAR_TO_STRING(100 - i), init_payload(i))); }
    template<typename Container>
    static void p_init_ps_insert(Container &c) { for (size_t i = 0; i < 200; ++i) c.insert(std::make_pair(init_payload(i), CRAP__VAR_TO_STRING(100 - i))); }};

int main()
{
  stl_basic_test<neam::cr::persistence_backend::neam>::run();

  stl_basic_test<neam::cr::persistence_backend::json>::run();
}
