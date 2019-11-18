/**
 * @file    client.cpp
 * @author  Unknown
 * @brief   Client main
 *
 * Generated by 'brix11 generate client' @ 2014-03-02 15:41:45 +0100
 *
 * @copyright Copyright (c) Remedy IT Expertise BV
 */

#include "tao/x11/idl_traits_t.h"

// X11_FUZZ: disable check_taox11_namespaces
namespace TAOX11_NAMESPACE
{
  namespace IDL
  {
    // specialization for sequence<long>
    // will be used by default
    template <typename OS>
    struct formatter<std::vector<int32_t>, OS>
    {
      formatter () {}
      explicit formatter (const char*) {}

      inline OS& operator ()(
          OS& os_,
          const std::vector<int32_t>& val_)
      {
        os_ << "Test::NumberList [" << std::endl;
        bool first_ = true;
        for (const int32_t& e_ : val_)
        {
          os_ << "  " << e_ << (first_ ? "" : ",") << std::endl;
          first_ = false;
        }
        return os_ << " ]" << std::endl;
      }
    };
  }
}
// X11_FUZZ: enable check_taox11_namespaces

#include "custom_formatC.h"
#include "ace/Get_Opt.h"
#include "testlib/taox11_testlog.h"
#include <iomanip>

std::string ior = "file://server.ior";

bool
parse_args (int argc, ACE_TCHAR *argv[])
{
  ACE_Get_Opt get_opts (argc, argv, ACE_TEXT("k:"));
  int c;

  while ((c = get_opts ()) != -1)
    switch (c)
      {
      case 'k':
        ior = get_opts.opt_arg ();
        break;

      case '?':
      default:
        TAOX11_TEST_ERROR << "usage: -k <ior>"
              << std::endl;
        return false;
      }
  // Indicates successful parsing of the command line
  return true;
}

// custom matrix formatter
// used when specified
struct my_matrix_formatter
{
  my_matrix_formatter () {}
  explicit my_matrix_formatter (const char*) {}

  inline void print_hline(std::ostream& os_, std::size_t len) {
    os_ << '+'; for (std::size_t i=0; i<len ;++i) os_ << "---"; os_ << '+' << std::endl;
  }

  inline std::ostream& operator ()(
      std::ostream& os_,
      const Test::Matrix& val_)
  {
    print_hline (os_, std::tuple_size<Test::Matrix::value_type> ());
    for (std::size_t i=0; i< std::tuple_size<Test::Matrix> (); ++i)
    {
      os_ << '|' << std::hex;
      for (std::size_t j=0; j< std::tuple_size<Test::Matrix::value_type> () ;++j)
      {
        os_ << std::setw(3) << static_cast<uint16_t> (val_[i][j]);
      }
      os_ << std::dec << '|' << std::endl;
    }
    print_hline (os_, std::tuple_size<Test::Matrix::value_type> ());
    return os_;
  }
};

template <typename OS>
struct my_format_string_fmt
{
  my_format_string_fmt () {}

  inline OS& operator ()(
      OS& os_,
      const std::string& val_)
  {
    // Use this structure for large maninipulations.
    os_ << "Test::String [" << std::endl;
    os_ << std::setw(15) << val_ << std::endl;
    return os_ << " ]" << std::endl;
  }
};

namespace format_test
{
  struct __Format_StringWriter
  {
    __Format_StringWriter (const std::string& s)
      : s_ (s) {}

    const std::string& s_;
  };

  __Format_StringWriter format_write (const std::string& s)
  {
    // This method can be used for simple string maninipulations.
    return __Format_StringWriter (s);
  }

  template <typename OS>
  OS&
  operator << (OS& os, __Format_StringWriter sw)
  {
    IDL::traits<std::string>::write_on<
        std::basic_ostream<typename OS::char_type, typename OS::traits_type>,
        my_format_string_fmt<std::basic_ostream<typename OS::char_type, typename OS::traits_type>>
        > (
      os,
      sw.s_);
    return os;
  }
}

struct my_person_formatter
{

  inline std::ostream& operator ()(
      std::ostream& os_,
      const Test::Person& val_)
  {
    return os_ << "Person :" << std::endl
        << "--" << std::setw(15) << "name = " << val_.name () << std::endl
        << "--" << std::setw(15) << "address = "<< val_.address () << std::endl
        << "--" << std::setw(15) << "age = "<< val_.age () << std::endl
        << "--" << std::setw(15) << "sex = "<< (val_.sex () == Test::Person::TSex::male ? "male" : "female") << std::endl;
  }
};

int main(int argc, char* argv[])
{
  try
    {
      IDL::traits<CORBA::ORB>::ref_type _orb = CORBA::ORB_init (argc, argv);

      if (_orb == nullptr)
        {
          TAOX11_TEST_ERROR
            << "ERROR: CORBA::ORB_init (argc, argv) returned null ORB."
            << std::endl;
          return 1;
        }

      if (parse_args (argc, argv) == false)
        return 1;

      IDL::traits<CORBA::Object>::ref_type obj = _orb->string_to_object (ior);

      if (!obj)
        {
        TAOX11_TEST_ERROR
            << "ERROR: string_to_object(<ior>) returned null reference."
            << std::endl;
          return 1;
        }

      TAOX11_TEST_INFO << "retrieved object reference" << std::endl;

      IDL::traits<Test::Foo>::ref_type foo_obj = IDL::traits<Test::Foo>::narrow (obj);

      if (!foo_obj)
        {
        TAOX11_TEST_ERROR
            << "ERROR: IDL::traits<Test::Foo>::narrow (obj) returned null object."
            << std::endl;
          return 1;
        }

      // Stream a vector of int32_t straight into a stream. The (overloaded)
      // formatter for 'std::vector<int32_t>' will be invoked.
      std::vector<int32_t> vc;
      for (uint16_t it=0; it < 8; ++it)
        vc.push_back(it);

      TAOX11_TEST_INFO << vc << std::endl;

      std::string const tmp ("Hello World !");

      TAOX11_TEST_INFO << format_test::format_write(tmp) << std::endl;


      IDL::traits<Test::Foo>::write_on (TAOX11_TEST_INFO << "narrowed Test::Foo interface = ", foo_obj) << std::endl;

      // Initialize data
      Test::NumberList nl = { 0, 2, 4, 8, 16, 32, 64, 128, 256, 512};
      Test::Matrix m;
      for (uint8_t b1=0; b1 < 16 ;++b1)
        for (uint8_t b2=0; b2 < 16 ;++b2)
          m[b1][b2] = (b1*16)+b2;
      Test::Person p ("Thomas Crown", "Unknown Street", 43, Test::Person::TSex::male);

      // write data
      IDL::traits<Test::Matrix>::write_on (TAOX11_TEST_INFO << "Default dump Matrix m = ", m) << std::endl;
      TAOX11_TEST_INFO << "Custom format Matrix m:" << std::endl << IDL::traits<Test::Matrix>::write<my_matrix_formatter> (m) << std::endl;
      IDL::traits<Test::NumberList>::write_on (TAOX11_TEST_INFO << "Default dump NumberList m = ", nl) << std::endl;

      TAOX11_TEST_INFO << "Default dump Person on server"<< std::endl;
      foo_obj->write_on_servant (p);
      TAOX11_TEST_INFO << "Custom format Person:" << std::endl << IDL::traits<Test::Person>::write<my_person_formatter> (p) << std::endl;

      TAOX11_TEST_INFO << "shutting down...";

      foo_obj->shutdown ();

      TAOX11_TEST_INFO << std::endl;

      _orb->destroy ();
    }
  catch (const std::exception& e)
    {
    TAOX11_TEST_ERROR << "exception caught: " << e.what () << std::endl;
      return 1;
    }
  return 0;
}
