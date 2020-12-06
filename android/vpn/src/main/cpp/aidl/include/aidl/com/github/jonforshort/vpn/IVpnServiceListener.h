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
class IVpnServiceListener : public ::ndk::ICInterface {
public:
  static const char* descriptor;
  IVpnServiceListener();
  virtual ~IVpnServiceListener();



  static std::shared_ptr<IVpnServiceListener> fromBinder(const ::ndk::SpAIBinder& binder);
  static binder_status_t writeToParcel(AParcel* parcel, const std::shared_ptr<IVpnServiceListener>& instance);
  static binder_status_t readFromParcel(const AParcel* parcel, std::shared_ptr<IVpnServiceListener>* instance);
  static bool setDefaultImpl(std::shared_ptr<IVpnServiceListener> impl);
  static const std::shared_ptr<IVpnServiceListener>& getDefaultImpl();
  virtual ::ndk::ScopedAStatus onSessionCreated(int32_t in_socket) = 0;
  virtual ::ndk::ScopedAStatus onSessionDestroyed(int32_t in_socket) = 0;
private:
  static std::shared_ptr<IVpnServiceListener> default_impl;
};
class IVpnServiceListenerDefault : public IVpnServiceListener {
public:
  ::ndk::ScopedAStatus onSessionCreated(int32_t in_socket) override;
  ::ndk::ScopedAStatus onSessionDestroyed(int32_t in_socket) override;
  ::ndk::SpAIBinder asBinder() override;
  bool isRemote() override;
};
}  // namespace vpn
}  // namespace jonforshort
}  // namespace github
}  // namespace com
}  // namespace aidl
