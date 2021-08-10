#include <grpcpp/grpcpp.h>

#include <iostream>
#include <memory>
#include <string>

#include "appcallback.grpc.pb.h"
#include "common.pb.h"

class EchoAppServerImpl final
    : public dapr::proto::runtime::v1::AppCallback::Service {
 public:
  grpc::Status OnInvoke(
      grpc::ServerContext *context,
      const dapr::proto::common::v1::InvokeRequest *request,
      dapr::proto::common::v1::InvokeResponse *response) override;
  grpc::Status ListTopicSubscriptions(
      grpc::ServerContext *context, const google::protobuf::Empty *request,
      dapr::proto::runtime::v1::ListTopicSubscriptionsResponse *response)
      override;
  grpc::Status ListInputBindings(
      grpc::ServerContext *context, const google::protobuf::Empty *request,
      dapr::proto::runtime::v1::ListInputBindingsResponse *response) override;
  grpc::Status OnBindingEvent(
      grpc::ServerContext *context,
      const dapr::proto::runtime::v1::BindingEventRequest *request,
      dapr::proto::runtime::v1::BindingEventResponse *response) override;
  grpc::Status OnTopicEvent(
      grpc::ServerContext *context,
      const dapr::proto::runtime::v1::TopicEventRequest *request,
      dapr::proto::runtime::v1::TopicEventResponse *response) override;
};