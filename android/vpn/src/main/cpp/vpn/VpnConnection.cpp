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
#include <arpa/inet.h>
#include <cstddef>
#include <pcapplusplus/Packet.h>
#include <pcapplusplus/IPv4Layer.h>
#include <pcapplusplus/TcpLayer.h>
#include <pcapplusplus/UdpLayer.h>
#include <vector>
#include <unistd.h>

#include "utils/log.h"
#include "VpnConnection.h"

using namespace ai;

namespace {

    constexpr auto BUFFER_SIZE = 16 * 1024;

    auto getCurrentTime() {
        struct timeval tp{};
        gettimeofday(&tp, nullptr);
        return tp;
    }

    auto processDataBuffer(uint8_t const *dataBytes, size_t dataLength) {
        auto rawPacket = pcpp::RawPacket(dataBytes, dataLength, getCurrentTime(), false, pcpp::LINKTYPE_IPV4);
        auto packet = pcpp::Packet(&rawPacket);
        if (packet.isPacketOfType(pcpp::IPv4)) {
            auto const ipv4Packet = packet.getLayerOfType<pcpp::IPv4Layer>();
            auto const sourceIP = ipv4Packet->getSrcIpAddress();
            auto const destinationIP = ipv4Packet->getDstIpAddress();

            if (packet.isPacketOfType(pcpp::TCP)) {
                auto const tcpPacket = packet.getLayerOfType<pcpp::TcpLayer>();
                auto const sourcePort = static_cast<uint16_t>(ntohs(tcpPacket->getTcpHeader()->portSrc));
                auto const destinationPort = static_cast<uint16_t>(ntohs(tcpPacket->getTcpHeader()->portDst));

                LOGD("processDataBuffer processing tcp packet: sourceIP [%s], sourcePort [%hu], destinationIP [%s], destinationPort [%hu]",
                     sourceIP.toString().c_str(), sourcePort, destinationIP.toString().c_str(), destinationPort);

            } else if (packet.isPacketOfType(pcpp::UDP)) {
                auto const udpPacket = packet.getLayerOfType<pcpp::UdpLayer>();
                auto const sourcePort = static_cast<uint16_t>(ntohs(udpPacket->getUdpHeader()->portSrc));
                auto const destinationPort = static_cast<uint16_t>(ntohs(udpPacket->getUdpHeader()->portDst));

                LOGD("processDataBuffer processing udp packet: sourceIP [%s], sourcePort [%hu], destinationIP [%s], destinationPort [%hu]",
                     sourceIP.toString().c_str(), sourcePort, destinationIP.toString().c_str(), destinationPort);

            } else {
                LOGD("processDataBuffer processing unknown packet:  sourceIP [%s], destinationIP [%s]",
                     sourceIP.toString().c_str(), destinationIP.toString().c_str());
            }
        }
    }

    auto processFileDescriptor(int const fd, std::atomic_bool *stopThread_) -> void {
        LOGI("processFileDescriptor start");

        auto buffer = std::vector<std::uint8_t>(BUFFER_SIZE);
        int dataReadInBytes;
        while (true) {
            if (*stopThread_) {
                LOGI("processFileDescriptor stop thread requested");
                break;
            }
            dataReadInBytes = read(fd, buffer.data(), BUFFER_SIZE);
            if (dataReadInBytes < 1) {
                sleep(1);
                continue;
            }
            processDataBuffer(buffer.data(), dataReadInBytes);
        }

        LOGI("processFileDescriptor finished");
    }
}

vpn::VpnConnection::VpnConnection(const int fd) : fd_(fd), isDisconnectRequested_(false) {
}

vpn::VpnConnection::~VpnConnection() {
    close(fd_);
}

auto vpn::VpnConnection::connect() -> void {
    thread_ = std::thread(&processFileDescriptor, fd_, &isDisconnectRequested_);
}

auto vpn::VpnConnection::disconnect() -> void {
    isDisconnectRequested_ = true;
    thread_.join();
}