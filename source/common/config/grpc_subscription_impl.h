#pragma once

#include "envoy/api/v2/core/base.pb.h"
#include "envoy/config/subscription.h"
#include "envoy/event/dispatcher.h"
#include "envoy/grpc/async_client.h"

#include "common/config/grpc_mux_impl.h"
#include "common/config/grpc_mux_subscription_impl.h"
#include "common/config/utility.h"

namespace Envoy {
namespace Config {

class GrpcSubscriptionImpl : public Config::Subscription {
public:
  GrpcSubscriptionImpl(const LocalInfo::LocalInfo& local_info, Grpc::AsyncClientPtr async_client,
                       Event::Dispatcher& dispatcher, Runtime::RandomGenerator& random,
                       const Protobuf::MethodDescriptor& service_method, absl::string_view type_url,
                       SubscriptionStats stats, Stats::Scope& scope,
                       const RateLimitSettings& rate_limit_settings,
                       std::chrono::milliseconds init_fetch_timeout)
      : grpc_mux_(std::make_shared<Config::GrpcMuxImpl>(local_info, std::move(async_client),
                                                        dispatcher, service_method, random, scope,
                                                        rate_limit_settings)),
        grpc_mux_subscription_(grpc_mux_, stats, type_url, dispatcher, init_fetch_timeout) {}

  // Config::Subscription
  void start(const std::set<std::string>& resources,
             Config::SubscriptionCallbacks& callbacks) override {
    // Subscribe first, so we get failure callbacks if grpc_mux_.start() fails.
    grpc_mux_subscription_.start(resources, callbacks);
  }

  void updateResources(const std::set<std::string>& update_to_these_names) override {
    grpc_mux_subscription_.updateResources(update_to_these_names);
  }

  std::shared_ptr<Config::XdsGrpcContext> grpcMux() { return grpc_mux_; }

private:
  std::shared_ptr<Config::GrpcMuxImpl> grpc_mux_;
  GrpcMuxSubscriptionImpl grpc_mux_subscription_;
};

} // namespace Config
} // namespace Envoy
