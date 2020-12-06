#pragma once

#include "aidl/com/github/jonforshort/vpn/IVpnServiceListener.h"

#include <android/binder_ibinder.h>

namespace aidl {
namespace com {
namespace github {
namespace jonforshort {
namespace vpn {
class BpVpnServiceListener : public ::ndk::BpCInterface<IVpnServiceListener> {
public:
  BpVpnServiceListener(const ::ndk::SpAIBinder& binder);
  virtual ~BpVpnServiceListener();

  ::ndk::ScopedAStatus onSessionCreated(int32_t in_socket) override;
  ::ndk::ScopedAStatus onSessionDestroyed(int32_t in_socket) override;
};
}  // namespace vpn
}  // namespace jonforshort
}  // namespace github
}  // namespace com
}  // namespace aidl
