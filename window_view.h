/*
 * window_view.h
 * Copyright (C) 2023 youfa <vsyfar@gmail.com>
 *
 * Distributed under terms of the GPLv2 license.
 */

#ifndef WINDOW_VIEW_H
#define WINDOW_VIEW_H

#include <string>

namespace webrtc_examples {

class WindowView {
 public:
  class Events {
   public:
    Events() = default;
    virtual ~Events() = default;
    virtual void OnEncoderUpdate(const std::string& encoder) = 0;
    virtual void OnDecodersUpdate(const std::vector<std::string>& decoders) = 0;
    virtual void OnSessionEvent(bool on,
                                const std::string& local_ip,
                                int local_port,
                                const std::string& remote_ip,
                                int remote_port,
                                const std::string& encoder,
                                const std::vector<std::string>& decoders) = 0;
    virtual void OnSendAudio(bool send) = 0;
    virtual void OnPlayoutAudio(bool playout) = 0;
  };

  virtual ~WindowView() = default;
  virtual void SetSupportCodecs(const std::vector<std::string>& codecs) = 0;
  virtual void SetLocalIpAddress(const std::string& ip) = 0;

  void RegisterEvents(Events* events) { events_ = events; }

 protected:
  Events* events() const { return events_; }

 private:
  Events* events_;
};

}  // namespace webrtc_examples

#endif /* !WINDOW_VIEW_H */
