#ifndef __WPA_SUPPLICANT_CLIENT_SOCKET_H__
#define __WPA_SUPPLICANT_CLIENT_SOCKET_H__

#include <node.h>
#include <nan.h>
#include "wpa_ctrl.h"

extern "C" {
  struct wpa_ctrl * wpa_ctrl_open(const char *ctrl_path);
  struct wpa_ctrl * wpa_ctrl_open2(const char *ctrl_path, const char *cli_path);
  void wpa_ctrl_close(struct wpa_ctrl *ctrl);
  int wpa_ctrl_request(struct wpa_ctrl *ctrl, const char *cmd, size_t cmd_len,
  char *reply, size_t *reply_len, void (*msg_cb)(char *msg, size_t len));
  int wpa_ctrl_attach(struct wpa_ctrl *ctrl);
  int wpa_ctrl_detach(struct wpa_ctrl *ctrl);
  int wpa_ctrl_recv(struct wpa_ctrl *ctrl, char *reply, size_t *reply_len);
  int wpa_ctrl_pending(struct wpa_ctrl *ctrl);
  int wpa_ctrl_get_fd(struct wpa_ctrl *ctrl);
}

class WpaClientSocket : public node::ObjectWrap {

public:
  static NAN_MODULE_INIT(Init);

  static NAN_METHOD(New);
  static NAN_METHOD(Start);
  static NAN_METHOD(Bind);
  static NAN_METHOD(Write);

private:
  WpaClientSocket();
  ~WpaClientSocket();

  int start();
  int bind(const char *path);
  int write(const char *command, char *reply, size_t *reply_len);
  void poll();
  static void PollCloseCallback(uv_poll_t *handle);
  static void PollCallback(uv_poll_t *handle, int status, int events);

private:
  Nan::Persistent<v8::Object> This;
  struct wpa_ctrl *ctrl_conn;
  struct wpa_ctrl *monitor_conn;
  uv_poll_t poll_handle;
  bool needBind;

  static Nan::Persistent<v8::FunctionTemplate> constructor_template;
};

#endif
