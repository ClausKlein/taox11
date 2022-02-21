#include "HelloC.h"

#include "tao/x11/log.h"

// We start with a regular main that uses its commandline arguments and provides a try/catch block to make sure we catch
// any exception.
int main (int argc, char* argv[])
{
  // by default show all messages logged through global logger
  X11_LOGGER::priority_mask (x11_logger::X11_LogMask::LP_ALL);

  try
  {
    // We now first initialize the ORB and then do a string_to_object of the IOR file that server has written to disk.
    // After this we do a _narrow to the derived interface.
    auto orb = CORBA::ORB_init (argc, argv);
    auto tmp = orb->string_to_object ("file://test.ior");
    auto hello = IDL::traits<Test::Hello>::narrow (tmp);

    // We now have to check whether we have a valid object reference or not. If we invoke an operation on a nil object
    // reference we will cause an access violation.
    if (!hello)
    {
      taox11_error << "ERROR: IDL::traits<Test::Hello>::narrow (obj) "
                   << "returned null object." << std::endl;
      return 1;
    }

    // Now we are sure we have a valid object reference, so we invoke the get_string() operation on the server. We have at
    // this moment no clue how long this operation could take, it could return in micro seconds, it could take days, this
    // all depends on the server.

    std::string the_string = hello->get_string ();
    // And now we print the string to standard output.
    taox11_info << "hello->get_string () returned " << the_string << std::endl;

    // To let this example end itself gracefully we first shutdown the server and then destroy our own ORB.
    hello->shutdown ();
    orb->destroy ();

    // To make sure we see any exception we do have a catch statement catching these exceptions and printing the exception
    // information in a readable format.
  }
  catch (const std::exception& ex)
  {
    taox11_error << "exception caught: " << ex.what () << std::endl;
    return 1;
  }

  return 0;
}
