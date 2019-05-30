#pragma once

#include "envoy/config/subscription.h"

#include "common/config/grpc_delta_xds_context.h"
#include "common/config/utility.h"

namespace Envoy {
namespace Config {

/**
 * DeltaSubscription manages the logic of a delta xDS subscription for a particular resource
 * type. It uses a GrpcDeltaXdsContext to handle the actual gRPC communication with the xDS server.
 * DeltaSubscription and GrpcDeltaXdsContext are both used for both ADS and non-aggregated xDS;
 * the only difference is that ADS has multiple DeltaSubscriptions sharing a single
 * GrpcDeltaXdsContext.
 */
// TODO(fredlas) someday this class will be named GrpcSubscriptionImpl
class DeltaSubscriptionImpl : public Subscription {
public:
  DeltaSubscriptionImpl(std::shared_ptr<GrpcMux> context, absl::string_view type_url,
                        SubscriptionStats stats, std::chrono::milliseconds init_fetch_timeout);
  ~DeltaSubscriptionImpl();

  void pause();

  void resume();

  // Config::DeltaSubscription
  void start(const std::set<std::string>& resources, SubscriptionCallbacks& callbacks) override;
  void updateResources(const std::set<std::string>& update_to_these_names) override;

  std::shared_ptr<GrpcMux> getContextForTest() { return context_; }

private:
  std::shared_ptr<GrpcMux> context_;
  const std::string type_url_;
  SubscriptionStats stats_;
  WatchMap::Token watch_token_{WatchMap::InvalidToken};
};

} // namespace Config
} // namespace Envoy
