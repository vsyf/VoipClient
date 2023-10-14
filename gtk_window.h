/*
 * gtk_window.h
 * Copyright (C) 2023 youfa <vsyfar@gmail.com>
 *
 * Distributed under terms of the GPLv2 license.
 */

#ifndef GTK_WINDOW_H
#define GTK_WINDOW_H

#include <gtk/gtk.h>

#include <string>
#include <vector>

#include "window_view.h"

namespace webrtc_examples {
class GTK_Window : public WindowView {
 public:
  GTK_Window();
  virtual ~GTK_Window() override;

  void Create(int argc, char** argv);

  // WindowView interface
  void SetSupportCodecs(const std::vector<std::string>& codecs) override;
  void SetLocalIpAddress(const std::string& ip) override;

  void OnEncoderChanged(const std::string& encoder);
  void OnSessionStateChanged();
  void OnSendStateChanged(bool on);
  void OnPlayoutStateChanged(bool on);

 private:
  void UpdateEncoderList(const std::vector<std::string>& codecs);
  void UpdateDecodersInfo(const std::vector<std::string>& decoders);
  GtkWidget* window_;
  GtkWidget* local_ip_edit_;
  GtkWidget* local_port_edit_;
  GtkWidget* remote_ip_edit_;
  GtkWidget* remote_port_edit_;

  GtkWidget* encoder_list_edit_;

  GtkWidget* decoder_select_button_;
  GtkWidget* decoders_text_;

  GtkWidget* send_playout_area_;
  GtkWidget* send_switch_;
  GtkWidget* playout_switch_;

  GtkWidget* session_button_;

  std::string local_ip_;
  bool session_on_;
  std::vector<std::string> support_codecs_;
  std::string enabled_encoder_;
  std::vector<std::string> enabled_decoders_;
};
}  // namespace webrtc_examples

#endif /* !GTK_WINDOW_H */
