//
// MIT License
//
// Copyright 2020
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//
#ifndef ANDROID_INTROSPECTION_VPN_VPNSERVICE_H_
#define ANDROID_INTROSPECTION_VPN_VPNSERVICE_H_

#include "VpnConnection.h"

#include "aidl/com/github/jonforshort/vpn/BnVpnService.h"
#include "aidl/com/github/jonforshort/vpn/BnVpnServiceListener.h"

namespace ai::vpn {

    class VpnService : public aidl::com::github::jonforshort::vpn::BnVpnService {

    public:
        VpnService() = default;

        virtual ~VpnService() = default;

        virtual ::ndk::ScopedAStatus initialize(::ndk::SpAIBinder const&in_listener, const ::ndk::ScopedFileDescriptor const&in_vpnSocket);

        virtual ::ndk::ScopedAStatus start();

        virtual ::ndk::ScopedAStatus stop();

        virtual ::ndk::ScopedAStatus uninitialize();
    };
}

#endif /* ANDROID_INTROSPECTION_VPN_VPNSERVICE_H_ */
