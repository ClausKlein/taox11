/**
 * @file    server.cpp
 * @author  Martin Corino
 *
 * @brief   CORBA C++11 server application
 *
 * @copyright Copyright (c) Remedy IT Expertise BV
 */


#include "client_i.h"

#include "testlib/taox11_testlog.h"
#include <fstream>

int
main(int argc, ACE_TCHAR *argv[])
{
  try
    {
      IDL::traits<CORBA::ORB>::ref_type _orb = CORBA::ORB_init (argc, argv);

      if (_orb == nullptr)
      {
        TAOX11_TEST_ERROR << "ERROR: CORBA::ORB_init (argc, argv) returned null ORB." << std::endl;
        return 1;
      }

      IDL::traits<CORBA::Object>::ref_type obj = _orb->resolve_initial_references ("RootPOA");

      if (!obj)
      {
        TAOX11_TEST_ERROR << "ERROR: resolve_initial_references (\"RootPOA\") returned null reference." << std::endl;
        return 1;
      }

      TAOX11_TEST_DEBUG << "retrieved RootPOA object reference" << std::endl;

      IDL::traits<PortableServer::POA>::ref_type root_poa = IDL::traits<PortableServer::POA>::narrow (obj);

      if (!root_poa)
      {
        TAOX11_TEST_ERROR << "ERROR: IDL::traits<PortableServer::POA>::narrow (obj) returned null object." << std::endl;
        return 1;
      }

      TAOX11_TEST_DEBUG << "narrowed POA interface" << std::endl;

      IDL::traits<PortableServer::POAManager>::ref_type poaman = root_poa->the_POAManager ();

      if (!poaman)
      {
        TAOX11_TEST_ERROR << "ERROR: root_poa->the_POAManager () returned null object." << std::endl;
        return 1;
      }

      CORBA::servant_traits<Test::Client>::ref_type client_impl =
        CORBA::make_reference<Client_i> (_orb);

      TAOX11_TEST_DEBUG << "created Client servant" << std::endl;

      PortableServer::ObjectId id = root_poa->activate_object (client_impl);

      TAOX11_TEST_DEBUG << "activated Client servant" << std::endl;

      IDL::traits<CORBA::Object>::ref_type client_obj = root_poa->id_to_reference (id);

      if (client_obj == nullptr)
      {
        TAOX11_TEST_ERROR << "ERROR: root_poa->id_to_reference (id) returned null reference." << std::endl;
        return 1;
      }

      IDL::traits<Test::Client>::ref_type client = IDL::traits<Test::Client>::narrow (client_obj);

      if (client == nullptr)
      {
        TAOX11_TEST_ERROR << "ERROR: IDL::traits<Test::Client>::narrow (client_obj) returned null reference." << std::endl;
        return 1;
      }

      std::string ior = _orb->object_to_string (client);

      // Output the IOR to the <ior_output_file>
      std::ofstream fos("test.ior");
      if (!fos)
      {
        TAOX11_TEST_ERROR << "ERROR: failed to open file 'test.ior'" << std::endl;
        return 1;
      }
      fos << ior;
      fos.close ();

      TAOX11_TEST_DEBUG << "IOR for Hello servant written to 'test.ior' : " << ior << std::endl;

      poaman->activate ();

      TAOX11_TEST_DEBUG << "starting event loop" << std::endl;

      _orb->run ();

      TAOX11_TEST_DEBUG << "event loop finished" << std::endl;

      root_poa->destroy (true, true);

      _orb->destroy ();
    }
  catch (const std::exception& e)
    {
      TAOX11_TEST_ERROR << "exception caught: " << e << std::endl;
      return 1;
    }

  return 0;
}
