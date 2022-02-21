#include "HelloS.h"

/// Implement the Test::Hello interface
class Hello final : public virtual CORBA::servant_traits<Test::Hello>::base_type
{
public:
  /// Constructor
  Hello (IDL::traits<CORBA::ORB>::ref_type orb);
  std::string get_string () override;
  void shutdown () override;

private:
  /// Use an ORB reference to shutdown the application.
  IDL::traits<CORBA::ORB>::ref_type orb_;
};
