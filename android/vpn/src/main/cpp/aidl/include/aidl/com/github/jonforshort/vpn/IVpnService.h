#pragma once

#include <android/binder_interface_utils.h>

#include <cstdint>
#include <memory>
#include <optional>
#include <string>
#include <vector>
#ifdef BINDER_STABILITY_SUPPORT
#include <android/binder_stability.h>
#endif  // BINDER_STABILITY_SUPPORT

namespace aidl {
namespace com {
namespace github {
namespace jonforshort {
namespace vpn {
class IVpnService : public ::ndk::ICInterface {
public:
  static const char* descriptor;
  IVpnService();
  virtual ~IVpnService();



  static std::shared_ptr<IVpnService> fromBinder(const ::ndk::SpAIBinder& binder);
  static binder_status_t writeToParcel(AParcel* parcel, const std::shared_ptr<IVpnService>& instance);
  static binder_status_t readFromParcel(const AParcel* parcel, std::shared_ptr<IVpnService>* instance);
  static bool setDefaultImpl(std::shared_ptr<IVpnService> impl);
  static const std::shared_ptr<IVpnService>& getDefaultImpl();
  virtual ::ndk::ScopedAStatus start(const ::ndk::SpAIBinder& in_listener, const ::ndk::ScopedFileDescriptor& in_fd) = 0;
  virtual ::ndk::ScopedAStatus stop() = 0;
private:
  static std::shared_ptr<IVpnService> default_impl;
};
class IVpnServiceDefault : public IVpnService {
public:
  ::ndk::ScopedAStatus start(const ::ndk::SpAIBinder& in_listener, const ::ndk::ScopedFileDescriptor& in_fd) override;
  ::ndk::ScopedAStatus stop() override;
  ::ndk::SpAIBinder asBinder() override;
  bool isRemote() override;
};
}  // namespace vpn
}  // namespace jonforshort
}  // namespace github
}  // namespace com
}  // namespace aidl
