/**
 * @file    server.cpp
 * @author  Mark Drijver
 *
 * @brief   CORBA C++11 server application
 *
 * @copyright Copyright (c) Remedy IT Expertise BV
 */

#include "hello.h"

#include "tao/x11/log.h"
#include <fstream>

int main (int argc, char* argv[])
{
  // by default show all messages logged through global logger
  X11_LOGGER::priority_mask (x11_logger::X11_LogMask::LP_ALL);

  try
  {
    auto orb = CORBA::ORB_init (argc, argv);
    if (orb == nullptr)
    {
      taox11_error << "ERROR: CORBA::ORB_init (argc, argv) returned null ORB." << std::endl;
      return 1;
    }

    auto obj = orb->resolve_initial_references ("RootPOA");
    if (!obj)
    {
      taox11_error << "ERROR: resolve_initial_references (\"RootPOA\") returned null reference." << std::endl;
      return 1;
    }
    taox11_info << "retrieved RootPOA object reference" << std::endl;

    auto root_poa = IDL::traits<PortableServer::POA>::narrow (obj);
    if (!root_poa)
    {
      taox11_error << "ERROR: IDL::traits<PortableServer::POA>::narrow (obj) returned null object." << std::endl;
      return 1;
    }
    taox11_info << "narrowed POA interface" << std::endl;

    auto poa_manager = root_poa->the_POAManager ();
    if (!poa_manager)
    {
      taox11_error << "ERROR: root_poa->the_POAManager () returned null object." << std::endl;
      return 1;
    }

    // Now we create our servant and activate it.
    CORBA::servant_traits<Test::Hello>::ref_type hello_impl = CORBA::make_reference<Hello> (orb);

    taox11_info << "created Hello servant" << std::endl;

    PortableServer::ObjectId id = root_poa->activate_object (hello_impl);

    taox11_info << "activated Hello servant" << std::endl;

    auto object = root_poa->id_to_reference (id);
    if (object == nullptr)
    {
      taox11_error << "ERROR: root_poa->id_to_reference (id) returned null reference." << std::endl;
      return 1;
    }

    auto hello = IDL::traits<Test::Hello>::narrow (object);
    if (hello == nullptr)
    {
      taox11_error << "ERROR: IDL::traits<Test::Hello>::narrow (hello_obj) returned null reference." << std::endl;
      return 1;
    }

    // We now write our IOR to a file on disk so that the client can find the server.
    std::string ior = orb->object_to_string (hello);

    // Output the IOR to the ior_output_file
    std::ofstream fos ("test.ior");
    if (!fos)
    {
      taox11_error << "ERROR: failed to open file 'test.ior'" << std ::endl;
      return 1;
    }
    fos << ior;
    fos.close ();

    // Now we activate our POA Manager, at that moment the server accepts incoming requests and then run our ORB.
    poa_manager->activate ();
    orb->run ();

    // When the run method returns we print a message that we are ready and then destroy the RootPOA and the ORB.
    root_poa->destroy (true, true);
    orb->destroy ();
  }
  // And we have a catch block to catch all exceptions and we use the ostream insertion support to print the exception
  // information to the output.
  catch (const std::exception& ex)
  {
    taox11_error << "ERROR: exception caught: " << ex.what () << std::endl;
    return 1;
  }

  return 0;
}
