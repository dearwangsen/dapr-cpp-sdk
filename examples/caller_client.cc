#include <grpcpp/ext/proto_server_reflection_plugin.h>
#include <grpcpp/grpcpp.h>
#include <grpcpp/health_check_service_interface.h>

#include <cstdlib>
#include <iostream>
#include <memory>
#include <string>
#include <thread>

#include "dapr.grpc.pb.h"
#include "dapr_server.h"

class EchoApp {
 public:
  EchoApp(std::string dapr_grpc_port, std::string app_port)
      : dapr_grpc_port_(dapr_grpc_port), app_port_(app_port) {}

  void ConnectToDapr() {
    // 连接到 dapr grpc 服务器
    std::cout << "Connecting to " << std::string("127.0.0.1:") + dapr_grpc_port_
              << std::endl;
    client_stub_ = dapr::proto::runtime::v1::Dapr::NewStub(
        grpc::CreateChannel(std::string("127.0.0.1:") + dapr_grpc_port_,
                            grpc::InsecureChannelCredentials()));
  }

  std::string CallMethod(const std::string app_id, const std::string method,
                         const std::string message) {
    grpc::ClientContext context;  // grpc上下文
    dapr::proto::runtime::v1::InvokeServiceRequest
        request;                                       // InvokeService 请求
    dapr::proto::common::v1::InvokeResponse response;  // InvokeService 响应
    // 构造服务调用请求
    request.set_id(app_id);                         // "callee"
    request.mutable_message()->set_method(method);  // "echo"
    request.mutable_message()->mutable_data()->set_value(
        message.c_str());  // "hello dapr"
    // 调用 InvokeService
    grpc::Status status =
        client_stub_->InvokeService(&context, request, &response);
    if (status.ok()) {
      return response.data().value();
    }
    return "RPC Error : " + status.error_message() + ", " +
           status.error_details();
  }

  void StartAppServer() {
    std::string endpoint = std::string("127.0.0.1:") + app_port_;
    service_ = std::shared_ptr<EchoAppServerImpl>(new EchoAppServerImpl());
    // 启用/禁用默认运行状况检查服务。这适用于所有C++之后创建的服务器。对于每台服务器，用户可以覆盖默认值带有HealthCheckServiceServerBuilder选项。不是线程安全的。
    grpc::EnableDefaultHealthCheckService(true);
    // 向服务器生成器添加原型反射插件。这个函数应该在静态初始化时调用。
    grpc::reflection::InitProtoReflectionServerBuilderPlugin();
    // 创建和启动服务器实例的生成器类。
    grpc::ServerBuilder builder;
    builder.AddListeningPort(endpoint, grpc::InsecureServerCredentials());
    // 注册服务。
    builder.RegisterService(service_.get());
    // 开始监听同步grpc服务，由dapr访问。
    app_server_ = builder.BuildAndStart();
    std::cout << "Server listening on " << endpoint << std::endl;
  }

 private:
  std::string app_port_;        // 6100端口，用于dapr客户端访问
                                // 需要与dapr启动时的端口保持一致
  std::string dapr_grpc_port_;  // dapr端口号
  std::unique_ptr<dapr::proto::runtime::v1::Dapr::Stub>
      client_stub_;                             // dapr 的存根
  std::unique_ptr<grpc::Server> app_server_;    // 6100 端口
  std::shared_ptr<EchoAppServerImpl> service_;  // EchoAppServerImpl
};

std::string GetEnvironmentVariable(const std::string &var) {
  const char *val = std::getenv(var.c_str());
  return (val == nullptr) ? "" : val;
}

int main(int argc, char **argv) {
  std::string grpc_port = GetEnvironmentVariable("DAPR_GRPC_PORT");
  std::string app_port = std::string("6100");
  if (grpc_port == "") grpc_port = "50001";
  std::cout << "grpc port:" << grpc_port << " app port:" << app_port
            << std::endl;

  std::unique_ptr<EchoApp> app(new EchoApp(grpc_port, app_port));

  app->StartAppServer();
  std::this_thread::sleep_for(std::chrono::milliseconds(1000));

  app->ConnectToDapr();
  std::cout << "Call echo method to "
            << "callee" << std::endl;
  while (true) {
    std::string response = app->CallMethod("callee", "echo", "hello dapr");
    std::cout << "Received [" << response << "] from "
              << "callee" << std::endl;
    // 暂停2s
    std::this_thread::sleep_for(std::chrono::milliseconds(2000));
  }
  return 0;
}
