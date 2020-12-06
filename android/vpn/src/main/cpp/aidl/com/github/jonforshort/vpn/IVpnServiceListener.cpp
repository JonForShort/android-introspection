#include <android/binder_parcel_utils.h>
#include <aidl/com/github/jonforshort/vpn/BpVpnServiceListener.h>
#include <aidl/com/github/jonforshort/vpn/BnVpnServiceListener.h>
#include <aidl/com/github/jonforshort/vpn/IVpnServiceListener.h>

namespace aidl {
namespace com {
namespace github {
namespace jonforshort {
namespace vpn {
static binder_status_t _aidl_onTransact(AIBinder* _aidl_binder, transaction_code_t _aidl_code, const AParcel* _aidl_in, AParcel* _aidl_out) {
  (void)_aidl_in;
  (void)_aidl_out;
  binder_status_t _aidl_ret_status = STATUS_UNKNOWN_TRANSACTION;
  std::shared_ptr<BnVpnServiceListener> _aidl_impl = std::static_pointer_cast<BnVpnServiceListener>(::ndk::ICInterface::asInterface(_aidl_binder));
  switch (_aidl_code) {
    case (FIRST_CALL_TRANSACTION + 0 /*onSessionCreated*/): {
      int32_t in_socket;

      _aidl_ret_status = AParcel_readInt32(_aidl_in, &in_socket);
      if (_aidl_ret_status != STATUS_OK) break;

      ::ndk::ScopedAStatus _aidl_status = _aidl_impl->onSessionCreated(in_socket);
      _aidl_ret_status = AParcel_writeStatusHeader(_aidl_out, _aidl_status.get());
      if (_aidl_ret_status != STATUS_OK) break;

      if (!AStatus_isOk(_aidl_status.get())) break;

      break;
    }
    case (FIRST_CALL_TRANSACTION + 1 /*onSessionDestroyed*/): {
      int32_t in_socket;

      _aidl_ret_status = AParcel_readInt32(_aidl_in, &in_socket);
      if (_aidl_ret_status != STATUS_OK) break;

      ::ndk::ScopedAStatus _aidl_status = _aidl_impl->onSessionDestroyed(in_socket);
      _aidl_ret_status = AParcel_writeStatusHeader(_aidl_out, _aidl_status.get());
      if (_aidl_ret_status != STATUS_OK) break;

      if (!AStatus_isOk(_aidl_status.get())) break;

      break;
    }
  }
  return _aidl_ret_status;
}

static AIBinder_Class* _g_aidl_clazz = ::ndk::ICInterface::defineClass(IVpnServiceListener::descriptor, _aidl_onTransact);

BpVpnServiceListener::BpVpnServiceListener(const ::ndk::SpAIBinder& binder) : BpCInterface(binder) {}
BpVpnServiceListener::~BpVpnServiceListener() {}

::ndk::ScopedAStatus BpVpnServiceListener::onSessionCreated(int32_t in_socket) {
  binder_status_t _aidl_ret_status = STATUS_OK;
  ::ndk::ScopedAStatus _aidl_status;
  ::ndk::ScopedAParcel _aidl_in;
  ::ndk::ScopedAParcel _aidl_out;

  _aidl_ret_status = AIBinder_prepareTransaction(asBinder().get(), _aidl_in.getR());
  if (_aidl_ret_status != STATUS_OK) goto _aidl_error;

  _aidl_ret_status = AParcel_writeInt32(_aidl_in.get(), in_socket);
  if (_aidl_ret_status != STATUS_OK) goto _aidl_error;

  _aidl_ret_status = AIBinder_transact(
    asBinder().get(),
    (FIRST_CALL_TRANSACTION + 0 /*onSessionCreated*/),
    _aidl_in.getR(),
    _aidl_out.getR(),
    0
    #ifdef BINDER_STABILITY_SUPPORT
    | FLAG_PRIVATE_LOCAL
    #endif  // BINDER_STABILITY_SUPPORT
    );
  if (_aidl_ret_status == STATUS_UNKNOWN_TRANSACTION && IVpnServiceListener::getDefaultImpl()) {
    return IVpnServiceListener::getDefaultImpl()->onSessionCreated(in_socket);
  }
  if (_aidl_ret_status != STATUS_OK) goto _aidl_error;

  _aidl_ret_status = AParcel_readStatusHeader(_aidl_out.get(), _aidl_status.getR());
  if (_aidl_ret_status != STATUS_OK) goto _aidl_error;

  if (!AStatus_isOk(_aidl_status.get())) return _aidl_status;

  _aidl_error:
  _aidl_status.set(AStatus_fromStatus(_aidl_ret_status));
  return _aidl_status;
}
::ndk::ScopedAStatus BpVpnServiceListener::onSessionDestroyed(int32_t in_socket) {
  binder_status_t _aidl_ret_status = STATUS_OK;
  ::ndk::ScopedAStatus _aidl_status;
  ::ndk::ScopedAParcel _aidl_in;
  ::ndk::ScopedAParcel _aidl_out;

  _aidl_ret_status = AIBinder_prepareTransaction(asBinder().get(), _aidl_in.getR());
  if (_aidl_ret_status != STATUS_OK) goto _aidl_error;

  _aidl_ret_status = AParcel_writeInt32(_aidl_in.get(), in_socket);
  if (_aidl_ret_status != STATUS_OK) goto _aidl_error;

  _aidl_ret_status = AIBinder_transact(
    asBinder().get(),
    (FIRST_CALL_TRANSACTION + 1 /*onSessionDestroyed*/),
    _aidl_in.getR(),
    _aidl_out.getR(),
    0
    #ifdef BINDER_STABILITY_SUPPORT
    | FLAG_PRIVATE_LOCAL
    #endif  // BINDER_STABILITY_SUPPORT
    );
  if (_aidl_ret_status == STATUS_UNKNOWN_TRANSACTION && IVpnServiceListener::getDefaultImpl()) {
    return IVpnServiceListener::getDefaultImpl()->onSessionDestroyed(in_socket);
  }
  if (_aidl_ret_status != STATUS_OK) goto _aidl_error;

  _aidl_ret_status = AParcel_readStatusHeader(_aidl_out.get(), _aidl_status.getR());
  if (_aidl_ret_status != STATUS_OK) goto _aidl_error;

  if (!AStatus_isOk(_aidl_status.get())) return _aidl_status;

  _aidl_error:
  _aidl_status.set(AStatus_fromStatus(_aidl_ret_status));
  return _aidl_status;
}
// Source for BnVpnServiceListener
BnVpnServiceListener::BnVpnServiceListener() {}
BnVpnServiceListener::~BnVpnServiceListener() {}
::ndk::SpAIBinder BnVpnServiceListener::createBinder() {
  AIBinder* binder = AIBinder_new(_g_aidl_clazz, static_cast<void*>(this));
  #ifdef BINDER_STABILITY_SUPPORT
  AIBinder_markCompilationUnitStability(binder);
  #endif  // BINDER_STABILITY_SUPPORT
  return ::ndk::SpAIBinder(binder);
}
// Source for IVpnServiceListener
const char* IVpnServiceListener::descriptor = "com.github.jonforshort.vpn.IVpnServiceListener";
IVpnServiceListener::IVpnServiceListener() {}
IVpnServiceListener::~IVpnServiceListener() {}


std::shared_ptr<IVpnServiceListener> IVpnServiceListener::fromBinder(const ::ndk::SpAIBinder& binder) {
  if (!AIBinder_associateClass(binder.get(), _g_aidl_clazz)) { return nullptr; }
  std::shared_ptr<::ndk::ICInterface> interface = ::ndk::ICInterface::asInterface(binder.get());
  if (interface) {
    return std::static_pointer_cast<IVpnServiceListener>(interface);
  }
  return ::ndk::SharedRefBase::make<BpVpnServiceListener>(binder);
}

binder_status_t IVpnServiceListener::writeToParcel(AParcel* parcel, const std::shared_ptr<IVpnServiceListener>& instance) {
  return AParcel_writeStrongBinder(parcel, instance ? instance->asBinder().get() : nullptr);
}
binder_status_t IVpnServiceListener::readFromParcel(const AParcel* parcel, std::shared_ptr<IVpnServiceListener>* instance) {
  ::ndk::SpAIBinder binder;
  binder_status_t status = AParcel_readStrongBinder(parcel, binder.getR());
  if (status != STATUS_OK) return status;
  *instance = IVpnServiceListener::fromBinder(binder);
  return STATUS_OK;
}
bool IVpnServiceListener::setDefaultImpl(std::shared_ptr<IVpnServiceListener> impl) {
  // Only one user of this interface can use this function
  // at a time. This is a heuristic to detect if two different
  // users in the same process use this function.
  assert(!IVpnServiceListener::default_impl);
  if (impl) {
    IVpnServiceListener::default_impl = impl;
    return true;
  }
  return false;
}
const std::shared_ptr<IVpnServiceListener>& IVpnServiceListener::getDefaultImpl() {
  return IVpnServiceListener::default_impl;
}
std::shared_ptr<IVpnServiceListener> IVpnServiceListener::default_impl = nullptr;
::ndk::ScopedAStatus IVpnServiceListenerDefault::onSessionCreated(int32_t /*in_socket*/) {
  ::ndk::ScopedAStatus _aidl_status;
  _aidl_status.set(AStatus_fromStatus(STATUS_UNKNOWN_TRANSACTION));
  return _aidl_status;
}
::ndk::ScopedAStatus IVpnServiceListenerDefault::onSessionDestroyed(int32_t /*in_socket*/) {
  ::ndk::ScopedAStatus _aidl_status;
  _aidl_status.set(AStatus_fromStatus(STATUS_UNKNOWN_TRANSACTION));
  return _aidl_status;
}
::ndk::SpAIBinder IVpnServiceListenerDefault::asBinder() {
  return ::ndk::SpAIBinder();
}
bool IVpnServiceListenerDefault::isRemote() {
  return false;
}
}  // namespace vpn
}  // namespace jonforshort
}  // namespace github
}  // namespace com
}  // namespace aidl
