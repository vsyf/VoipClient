/*
 *  Copyright 2023 The WebRTC Project Authors. All rights reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef EXAMPLES_VOIP_CLIENT_VOIP_CLIENT_H_
#define EXAMPLES_VOIP_CLIENT_VOIP_CLIENT_H_

#include <memory>
#include <string>
#include <vector>

#include "api/audio_codecs/audio_format.h"
#include "api/call/transport.h"
#include "api/voip/voip_base.h"
#include "api/voip/voip_engine.h"
#include "rtc_base/async_packet_socket.h"
#include "rtc_base/async_udp_socket.h"
#include "rtc_base/socket_address.h"
#include "rtc_base/third_party/sigslot/sigslot.h"
#include "rtc_base/thread.h"

namespace webrtc_examples {

class VoipClient : public webrtc::Transport, public sigslot::has_slots<> {
 public:
  class Callback {
   public:
    virtual ~Callback() = default;
    virtual void OnStartSessionCompleted(bool success) = 0;
    virtual void OnStopSessionCompleted(bool success) = 0;
    virtual void OnStartSendCompleted(bool success) = 0;
    virtual void OnStopSendCompleted(bool success) = 0;
    virtual void OnStartPlayoutCompleted(bool success) = 0;
    virtual void OnStopPlayoutCompleted(bool success) = 0;
  };

  static VoipClient* Create();

  ~VoipClient() override;

  std::vector<std::string> GetSupportedCodecs();
  std::string GetLocalIPAddress();

  void SetEncoder(const std::string& encoder);
  void SetDecoders(const std::vector<std::string>& decoders);
  void SetLocalAddress(const std::string& ip_address, int port_number);
  void SetRemoteAddress(const std::string& ip_address, int port_number);

  void StartSession();

  void StopSession();

  void StartSend();

  void StopSend();

  void StartPlayout();

  void StopPlayout();

  // protcted:
  bool SendRtp(const uint8_t* packet,
               size_t length,
               const webrtc::PacketOptions& options) override;
  bool SendRtcp(const uint8_t* packet, size_t length) override;

  // Slots for sockets to connect to.
  void OnSignalReadRTPPacket(rtc::AsyncPacketSocket* socket,
                             const char* rtp_packet,
                             size_t size,
                             const rtc::SocketAddress& addr,
                             const int64_t& timestamp);
  void OnSignalReadRTCPPacket(rtc::AsyncPacketSocket* socket,
                              const char* rtcp_packet,
                              size_t size,
                              const rtc::SocketAddress& addr,
                              const int64_t& timestamp);

 private:
  VoipClient() : voip_thread_(rtc::Thread::CreateWithSocketServer()) {}

  void Init();

  // Methods to send and receive RTP/RTCP packets. Takes in a
  // copy of a packet as a vector to prolong the lifetime of
  // the packet as these methods will be called asynchronously.
  void SendRtpPacket(const std::vector<uint8_t>& packet_copy);
  void SendRtcpPacket(const std::vector<uint8_t>& packet_copy);
  void ReadRTPPacket(const std::vector<uint8_t>& packet_copy);
  void ReadRTCPPacket(const std::vector<uint8_t>& packet_copy);

  // Used to invoke operations and send/receive RTP/RTCP packets.
  std::unique_ptr<rtc::Thread> voip_thread_;

  std::weak_ptr<Callback> callback_;

  // A list of AudioCodecSpec supported by the built-in
  // encoder/decoder factories.
  std::vector<webrtc::AudioCodecSpec> supported_codecs_;
  // The entry point to all VoIP APIs.
  std::unique_ptr<webrtc::VoipEngine> voip_engine_ RTC_GUARDED_BY(voip_thread_);
  // Used by the VoIP API to facilitate a VoIP session.
  absl::optional<webrtc::ChannelId> channel_ RTC_GUARDED_BY(voip_thread_);
  // Members below are used for network related operations.
  std::unique_ptr<rtc::AsyncUDPSocket> rtp_socket_ RTC_GUARDED_BY(voip_thread_);
  std::unique_ptr<rtc::AsyncUDPSocket> rtcp_socket_
      RTC_GUARDED_BY(voip_thread_);
  rtc::SocketAddress rtp_local_address_ RTC_GUARDED_BY(voip_thread_);
  rtc::SocketAddress rtcp_local_address_ RTC_GUARDED_BY(voip_thread_);
  rtc::SocketAddress rtp_remote_address_ RTC_GUARDED_BY(voip_thread_);
  rtc::SocketAddress rtcp_remote_address_ RTC_GUARDED_BY(voip_thread_);
};

}  // namespace webrtc_examples

#endif  // EXAMPLES_VOIP_CLIENT_VOIP_CLIENT_H_
