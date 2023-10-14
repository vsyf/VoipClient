/*
 * gtk_window.cc
 * Copyright (C) 2023 youfa <vsyfar@gmail.com>
 *
 * Distributed under terms of the GPLv2 license.
 */

#include "gtk_window.h"

#include <gtk/gtk.h>

#include "rtc_base/logging.h"

namespace webrtc_examples {

namespace {

constexpr int kDefaultPort = 10000;

void OnEncoderComboBoxEdit(GtkWidget* widget, gpointer* data) {
  GtkTreeModel* model;
  GtkTreeIter iter;

  model = gtk_combo_box_get_model(GTK_COMBO_BOX(widget));
  if (gtk_combo_box_get_active_iter(GTK_COMBO_BOX(widget), &iter)) {
    gchar* value;
    gtk_tree_model_get(model, &iter, 0, &value, -1);
    RTC_LOG(LS_INFO) << "Selected encoder: " << value;
    ((GTK_Window*)data)->OnEncoderChanged(value);
    g_free(value);
  }
}

void OnDecodersButtonClicked(GtkWidget* widget, gpointer* data) {}

void OnSessionButtonClicked(GtkWidget* widget, gpointer* data) {
  ((GTK_Window*)data)->OnSessionStateChanged();
}

void OnSendButtonClicked(GtkWidget* widget, gboolean state, gpointer* data) {
  ((GTK_Window*)data)->OnSendStateChanged(state);
}
void OnPlayoutButtonClicked(GtkWidget* widget, gboolean state, gpointer* data) {
  ((GTK_Window*)data)->OnPlayoutStateChanged(state);
}

}  // namespace

GTK_Window::GTK_Window()
    : window_(nullptr),
      local_ip_edit_(nullptr),
      local_port_edit_(nullptr),
      remote_ip_edit_(nullptr),
      encoder_list_edit_(nullptr),
      decoder_select_button_(nullptr),
      decoders_text_(nullptr),
      send_playout_area_(nullptr),
      send_switch_(nullptr),
      playout_switch_(nullptr),
      session_button_(nullptr),
      session_on_(false)

{}

GTK_Window::~GTK_Window() {}

void GTK_Window::Create(int argc, char** argv) {
  gtk_init(&argc, &argv);
// g_type_init API is deprecated (and does nothing) since glib 2.35.0, see:
// https://mail.gnome.org/archives/commits-list/2012-November/msg07809.html
#if !GLIB_CHECK_VERSION(2, 35, 0)
  g_type_init();
#endif
// g_thread_init API is deprecated since glib 2.31.0, see release note:
// http://mail.gnome.org/archives/gnome-announce-list/2011-October/msg00041.html
#if !GLIB_CHECK_VERSION(2, 31, 0)
  g_thread_init(NULL);
#endif

  GtkBuilder* builder = gtk_builder_new();
  gtk_builder_add_from_file(builder, "examples/voipclient/voip_client.xml",
                            NULL);

  window_ = GTK_WIDGET(gtk_builder_get_object(builder, "window"));
  g_signal_connect(window_, "destroy", G_CALLBACK(gtk_main_quit), NULL);
  if (window_ == NULL) {
    g_warning("Can't find window in ui file");
    return;
  }
  // windows to center
  gtk_window_set_position(GTK_WINDOW(window_), GTK_WIN_POS_CENTER);

  local_ip_edit_ = GTK_WIDGET(gtk_builder_get_object(builder, "local_ip"));
  if (local_ip_.size() > 0) {
    gtk_entry_set_text(GTK_ENTRY(local_ip_edit_), local_ip_.c_str());
  }

  local_port_edit_ = GTK_WIDGET(gtk_builder_get_object(builder, "local_port"));
  gtk_entry_set_text(GTK_ENTRY(local_port_edit_),
                     std::to_string(kDefaultPort).c_str());

  remote_ip_edit_ = GTK_WIDGET(gtk_builder_get_object(builder, "remote_ip"));
  if (local_ip_.size() > 0) {
    gtk_entry_set_text(GTK_ENTRY(remote_ip_edit_), local_ip_.c_str());
  }
  remote_port_edit_ =
      GTK_WIDGET(gtk_builder_get_object(builder, "remote_port"));
  gtk_entry_set_text(GTK_ENTRY(remote_port_edit_),
                     std::to_string(kDefaultPort).c_str());

  encoder_list_edit_ =
      GTK_WIDGET(gtk_builder_get_object(builder, "encoder_list"));
  UpdateEncoderList(support_codecs_);
  g_signal_connect(encoder_list_edit_, "changed",
                   G_CALLBACK(OnEncoderComboBoxEdit), this);

  decoder_select_button_ =
      GTK_WIDGET(gtk_builder_get_object(builder, "select_decoder"));
  g_signal_connect(decoder_select_button_, "clicked",
                   G_CALLBACK(OnDecodersButtonClicked), this);

  decoders_text_ = GTK_WIDGET(gtk_builder_get_object(builder, "decoders_text"));
  UpdateDecodersInfo(support_codecs_);

  send_playout_area_ =
      GTK_WIDGET(gtk_builder_get_object(builder, "send_and_playout_area"));
  send_switch_ = GTK_WIDGET(gtk_builder_get_object(builder, "send_switch"));
  g_signal_connect(send_switch_, "state-set", G_CALLBACK(OnSendButtonClicked),
                   this);

  playout_switch_ =
      GTK_WIDGET(gtk_builder_get_object(builder, "playout_switch"));
  g_signal_connect(playout_switch_, "state-set",
                   G_CALLBACK(OnPlayoutButtonClicked), this);

  session_button_ =
      GTK_WIDGET(gtk_builder_get_object(builder, "session_button"));
  g_signal_connect(session_button_, "clicked",
                   G_CALLBACK(OnSessionButtonClicked), this);

  // gtk_widget_show_all(window_);
  gtk_main();
}

void GTK_Window::SetSupportCodecs(const std::vector<std::string>& codecs) {
  //
  support_codecs_ = codecs;
}

void GTK_Window::SetLocalIpAddress(const std::string& ip) {
  if (local_ip_edit_) {
    gtk_entry_set_text(GTK_ENTRY(local_ip_edit_), ip.c_str());
  }
  local_ip_ = ip;
}

void GTK_Window::OnSessionStateChanged() {
  RTC_LOG(LS_INFO) << "OnSessionStateChanged";
  session_on_ = !session_on_;
  if (session_on_) {
    // display send and playout area
    gtk_widget_show(send_playout_area_);
    // default set send_switch and playout switch off
    gtk_switch_set_active(GTK_SWITCH(send_switch_), FALSE);
    gtk_switch_set_active(GTK_SWITCH(playout_switch_), FALSE);

    gtk_button_set_label(GTK_BUTTON(session_button_), "Stop Session");
    std::string local_ip = gtk_entry_get_text(GTK_ENTRY(local_ip_edit_));
    std::string local_port = gtk_entry_get_text(GTK_ENTRY(local_port_edit_));
    int l_port = atoi(local_port.c_str());

    std::string remote_ip = gtk_entry_get_text(GTK_ENTRY(remote_ip_edit_));
    std::string remote_port = gtk_entry_get_text(GTK_ENTRY(remote_port_edit_));
    int r_port = atoi(remote_port.c_str());

    RTC_LOG(LS_INFO) << "local_ip:" << local_ip << ", local_port:" << l_port
                     << ", remote_ip:" << remote_ip
                     << ", remote_port:" << r_port;

    events()->OnSessionEvent(session_on_, local_ip, l_port, remote_ip, r_port,
                             enabled_encoder_, enabled_decoders_);
  } else {
    // hide send and playout area
    gtk_widget_hide(send_playout_area_);

    gtk_button_set_label(GTK_BUTTON(session_button_), "Start Session");
    events()->OnSessionEvent(false, "", 0, "", 0, enabled_encoder_,
                             enabled_decoders_);
  }
}

void GTK_Window::OnSendStateChanged(bool on) {
  RTC_LOG(LS_INFO) << "OnSendStateChanged, on:" << on;
  events()->OnSendAudio(on);
}

void GTK_Window::OnPlayoutStateChanged(bool on) {
  RTC_LOG(LS_INFO) << "OnPlayoutStateChanged, on:" << on;
  events()->OnPlayoutAudio(on);
}

void GTK_Window::UpdateEncoderList(const std::vector<std::string>& codecs) {
  // if codecs.size > 0, clear encoder_list_edit_
  if (codecs.size() > 0) {
    gtk_combo_box_text_remove_all(GTK_COMBO_BOX_TEXT(encoder_list_edit_));
  }

  // push support_codecs_ to encoder_list_edit_
  for (auto codec : support_codecs_) {
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(encoder_list_edit_),
                                   codec.c_str());
  }
  // set default encoder
  gtk_combo_box_set_active(GTK_COMBO_BOX(encoder_list_edit_), 0);
  enabled_encoder_ = support_codecs_[0];
}

void GTK_Window::OnEncoderChanged(const std::string& encoder) {
  RTC_LOG(LS_INFO) << "UpdateEncoder, encoder:" << encoder;
  events()->OnEncoderUpdate(encoder);
}

void GTK_Window::UpdateDecodersInfo(const std::vector<std::string>& decoders) {
  enabled_decoders_ = decoders;
  std::string decoders_text;
  for (size_t i = 0; i < (enabled_decoders_.size() - 1); i++) {
    decoders_text += enabled_decoders_[i] + ", ";
  }
  decoders_text.append(enabled_decoders_.back());
  gtk_label_set_text(GTK_LABEL(decoders_text_), decoders_text.c_str());
}

}  // namespace webrtc_examples
