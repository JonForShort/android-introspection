#pragma once

#include "aidl/com/github/jonforshort/vpn/IVpnService.h"

#include <android/binder_ibinder.h>

namespace aidl {
namespace com {
namespace github {
namespace jonforshort {
namespace vpn {
class BnVpnService : public ::ndk::BnCInterface<IVpnService> {
public:
  BnVpnService();
  virtual ~BnVpnService();
protected:
  ::ndk::SpAIBinder createBinder() override;
private:
};
}  // namespace vpn
}  // namespace jonforshort
}  // namespace github
}  // namespace com
}  // namespace aidl
