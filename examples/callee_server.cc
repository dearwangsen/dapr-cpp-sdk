#include <cstdlib>
#include <iostream>
#include <memory>
#include <string>
#include <thread>

#include <grpcpp/grpcpp.h>
#include <grpcpp/health_check_service_interface.h>
#include <grpcpp/ext/proto_server_reflection_plugin.h>

#include "dapr_server.h"

class EchoAppServer
{
  public:
    EchoAppServer(std::string app_port) : app_port_(app_port) {};

    void StartAppServer() 
    {
      std::string endpoint = std::string("127.0.0.1:") + app_port_;
      service_ = std::shared_ptr<EchoAppServerImpl>(new EchoAppServerImpl());

      grpc::EnableDefaultHealthCheckService(true);
      grpc::reflection::InitProtoReflectionServerBuilderPlugin();

      grpc::ServerBuilder builder;
      builder.AddListeningPort(endpoint, grpc::InsecureServerCredentials());
      builder.RegisterService(service_.get());
      
      // Start synchronous gRPC server.
      app_server_ = builder.BuildAndStart();
      std::cout << "Server listening on " << endpoint << std::endl;
    }

    void WaitUntilServerIsDown() 
    {
      std::cout << "Wait until server is down" << std::endl;
      app_server_->Wait();
    }

  private:
    std::string app_port_;
    std::unique_ptr<grpc::Server> app_server_;
    std::shared_ptr<EchoAppServerImpl> service_;
};

int main(int argc, char** argv)
{
  std::string app_port = std::string("6000");
  std::cout << " app port: " << app_port <<std::endl;
  std::unique_ptr<EchoAppServer> app(new EchoAppServer(app_port));
  app->StartAppServer();
  std::this_thread::sleep_for(std::chrono::milliseconds(1000));
  app->WaitUntilServerIsDown();
  return 0;
}
