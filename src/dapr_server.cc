#include "dapr_server.h"

#include <grpcpp/grpcpp.h>

#include <iostream>
#include <memory>
#include <string>

#include "appcallback.grpc.pb.h"
#include "common.pb.h"

grpc::Status EchoAppServerImpl::OnInvoke(
    grpc::ServerContext *context,
    const dapr::proto::common::v1::InvokeRequest *request,
    dapr::proto::common::v1::InvokeResponse *response) {
  std::cout << "OnInvoke() is called" << std::endl;
  if (request->method() == "echo") {
    std::cout << "Got the message: " << request->data().value() << std::endl;
    std::string resp_str = "ack : " + request->data().value();
    response->mutable_data()->set_value(resp_str);
  }
  return grpc::Status::OK;
}

grpc::Status EchoAppServerImpl::ListTopicSubscriptions(
    grpc::ServerContext *context, const google::protobuf::Empty *request,
    dapr::proto::runtime::v1::ListTopicSubscriptionsResponse *response) {
  std::cout << "ListTopicSubscriptions() is called" << std::endl;
  return grpc::Status::OK;
}

grpc::Status EchoAppServerImpl::ListInputBindings(
    grpc::ServerContext *context, const google::protobuf::Empty *request,
    dapr::proto::runtime::v1::ListInputBindingsResponse *response) {
  std::cout << "ListInputBindings() is called" << std::endl;
  return grpc::Status::OK;
}

grpc::Status EchoAppServerImpl::OnBindingEvent(
    grpc::ServerContext *context,
    const dapr::proto::runtime::v1::BindingEventRequest *request,
    dapr::proto::runtime::v1::BindingEventResponse *response) {
  std::cout << "OnBindingEvent() is called" << std::endl;
  return grpc::Status::OK;
}

grpc::Status EchoAppServerImpl::OnTopicEvent(
    grpc::ServerContext *context,
    const dapr::proto::runtime::v1::TopicEventRequest *request,
    dapr::proto::runtime::v1::TopicEventResponse *response) {
  std::cout << "OnBindingEvent() is called" << std::endl;
  return grpc::Status::OK;
}