#pragma once

#include "aidl/com/github/jonforshort/vpn/IVpnService.h"

#include <android/binder_ibinder.h>

namespace aidl {
namespace com {
namespace github {
namespace jonforshort {
namespace vpn {
class BpVpnService : public ::ndk::BpCInterface<IVpnService> {
public:
  BpVpnService(const ::ndk::SpAIBinder& binder);
  virtual ~BpVpnService();

  ::ndk::ScopedAStatus initialize() override;
  ::ndk::ScopedAStatus start() override;
  ::ndk::ScopedAStatus stop() override;
  ::ndk::ScopedAStatus uninitialize() override;
};
}  // namespace vpn
}  // namespace jonforshort
}  // namespace github
}  // namespace com
}  // namespace aidl
