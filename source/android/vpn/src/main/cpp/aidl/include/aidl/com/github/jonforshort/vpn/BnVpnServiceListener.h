#pragma once

#include "aidl/com/github/jonforshort/vpn/IVpnServiceListener.h"

#include <android/binder_ibinder.h>

namespace aidl {
namespace com {
namespace github {
namespace jonforshort {
namespace vpn {
class BnVpnServiceListener : public ::ndk::BnCInterface<IVpnServiceListener> {
public:
  BnVpnServiceListener();
  virtual ~BnVpnServiceListener();
protected:
  ::ndk::SpAIBinder createBinder() override;
private:
};
}  // namespace vpn
}  // namespace jonforshort
}  // namespace github
}  // namespace com
}  // namespace aidl
