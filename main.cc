/*
 * main.cc
 * Copyright (C) 2023 youfa <vsyfar@gmail.com>
 *
 * Distributed under terms of the GPLv2 license.
 */

#include <memory>

#include "examples/voipclient/gtk_window.h"
#include "examples/voipclient/voip_client.h"
#include "examples/voipclient/window_view.h"
#include "rtc_base/logging.h"

using namespace webrtc_examples;

class Conductor : public WindowView::Events {
 public:
  Conductor(VoipClient* voip_client)
      : WindowView::Events(), voip_client_(voip_client) {}

  virtual ~Conductor() {}

  void OnEncoderUpdate(const std::string& encoder) override {
    voip_client_->SetEncoder(encoder);
  }
  void OnDecodersUpdate(const std::vector<std::string>& decoders) override {
    voip_client_->SetDecoders(decoders);
  }
  void OnSessionEvent(bool on,
                      const std::string& local_ip,
                      int local_port,
                      const std::string& remote_ip,
                      int remote_port,
                      const std::string& encoder,
                      const std::vector<std::string>& decoders) override {
    RTC_LOG(LS_INFO) << "OnSessionEvent";
    if (on) {
      voip_client_->SetLocalAddress(local_ip, local_port);
      voip_client_->SetRemoteAddress(remote_ip, remote_port);
      voip_client_->StartSession();
      voip_client_->SetEncoder(encoder);
      voip_client_->SetDecoders(decoders);
    } else {
      voip_client_->StopSession();
    }
  }
  void OnSendAudio(bool send) override {
    if (send) {
      voip_client_->StartSend();
    } else {
      voip_client_->StopSend();
    }
  }
  void OnPlayoutAudio(bool playout) override {
    if (playout) {
      voip_client_->StartPlayout();
    } else {
      voip_client_->StopPlayout();
    }
  }

 private:
  VoipClient* voip_client_;
};

int main(int argc, char* argv[]) {
  std::unique_ptr<webrtc_examples::VoipClient> voip_client(
      webrtc_examples::VoipClient::Create());
  auto support_codecs = voip_client->GetSupportedCodecs();
  auto local_ip = voip_client->GetLocalIPAddress();

  std::unique_ptr<webrtc_examples::GTK_Window> gtk_window(
      new webrtc_examples::GTK_Window());
  gtk_window->SetLocalIpAddress(local_ip);

  std::shared_ptr<Conductor> window_events(new Conductor(voip_client.get()));
  gtk_window->SetSupportCodecs(support_codecs);
  gtk_window->RegisterEvents(window_events.get());

  gtk_window->Create(argc, argv);

  // sleep(10);

  return 0;
}
