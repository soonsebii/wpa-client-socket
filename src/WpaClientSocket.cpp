#include <errno.h>
#include <sys/socket.h>
#include <sys/types.h>

#include <nan.h>

#include "WpaClientSocket.h"

using namespace v8;

Nan::Persistent<FunctionTemplate> WpaClientSocket::constructor_template;

NAN_MODULE_INIT(WpaClientSocket::Init) {
  Nan::HandleScope scope;

  Local<FunctionTemplate> tmpl = Nan::New<FunctionTemplate>(New);
  constructor_template.Reset(tmpl);

  tmpl->InstanceTemplate()->SetInternalFieldCount(1);
  tmpl->SetClassName(Nan::New("WpaClientSocket").ToLocalChecked());

  Nan::SetPrototypeMethod(tmpl, "start", Start);
  Nan::SetPrototypeMethod(tmpl, "bind", Bind);
  Nan::SetPrototypeMethod(tmpl, "write", Write);

  target->Set(Nan::New("WpaClientSocket").ToLocalChecked(), tmpl->GetFunction());
}

WpaClientSocket::WpaClientSocket() : node::ObjectWrap() {
  this->needBind = true;
}

WpaClientSocket::~WpaClientSocket() {
  uv_close((uv_handle_t*)&this->poll_handle, (uv_close_cb)WpaClientSocket::PollCloseCallback);
 
  if (this->ctrl_conn != NULL)
    wpa_ctrl_close(this->ctrl_conn);

  if (this->monitor_conn != NULL) {
    wpa_ctrl_detach(this->monitor_conn);
    wpa_ctrl_close(this->monitor_conn);
  }
}

int WpaClientSocket::start() {
  int ret = uv_poll_start(&this->poll_handle, UV_READABLE, WpaClientSocket::PollCallback);
  return ret;
}

int WpaClientSocket::bind(const char *path) {
  this->ctrl_conn = wpa_ctrl_open(path);
  if (this->ctrl_conn == NULL) {
    return -1;
  }

  this->monitor_conn = wpa_ctrl_open(path);
  if (this->monitor_conn == NULL) {
    return -1;
  }

  if (wpa_ctrl_attach(this->monitor_conn) < 0) {
    return -1;
  }

  int fd = wpa_ctrl_get_fd(this->monitor_conn); 
  uv_poll_init(uv_default_loop(), &this->poll_handle, fd);

  this->poll_handle.data = this;

  return 0;
}

int WpaClientSocket::write(const char *command, char *reply, size_t *reply_len) {
  int ret;

  ret = wpa_ctrl_request(this->ctrl_conn, command, strlen(command), reply, reply_len, NULL);
  if (ret < 0) {
    return -1;
  }

  return 0;
}

void WpaClientSocket::poll() {
  Nan::HandleScope scope;

  char buf[2048];
  size_t len = sizeof(buf) - 1;

  if (wpa_ctrl_recv(this->monitor_conn, buf, &len) == 0) {
    Local<Value> argv[2] = {
      Nan::New("data").ToLocalChecked(),
      Nan::CopyBuffer(buf, len).ToLocalChecked()
    };

    Nan::MakeCallback(Nan::New<Object>(this->This), Nan::New("emit").ToLocalChecked(), 2, argv);
  }
}

void WpaClientSocket::PollCallback(uv_poll_t * handle, int status, int events) {
  WpaClientSocket *p = (WpaClientSocket *)handle->data;
  p->poll();
}

void WpaClientSocket::PollCloseCallback(uv_poll_t * handle) {
  delete handle;
}

NAN_METHOD(WpaClientSocket::New) {
  Nan::HandleScope scope;

  WpaClientSocket* p = new WpaClientSocket();
  p->Wrap(info.This());
  p->This.Reset(info.This());
  info.GetReturnValue().Set(info.This());
}

NAN_METHOD(WpaClientSocket::Start) {
  Nan::HandleScope scope;

  WpaClientSocket* p = node::ObjectWrap::Unwrap<WpaClientSocket>(info.This());
  p->start();

  info.GetReturnValue().SetUndefined();
}

NAN_METHOD(WpaClientSocket::Bind) {
  Nan::HandleScope scope;

  WpaClientSocket* p = node::ObjectWrap::Unwrap<WpaClientSocket>(info.This());

  if (info.Length() > 0) {
    if (info[0]->IsString()) {
      Nan::Utf8String args0(info[0]);
      char *str = *args0;

      if (str != NULL) {
        p->bind(str);
      }
    }
  }

  info.GetReturnValue().SetUndefined();
}

NAN_METHOD(WpaClientSocket::Write) {
  Nan::HandleScope scope;

  WpaClientSocket* p = node::ObjectWrap::Unwrap<WpaClientSocket>(info.This());

  if (info.Length() > 0 && info[0]->IsString()) {
    Nan::Utf8String args0(info[0]);
    char *command = *args0;
    char reply[2048];
    size_t reply_len = sizeof(reply) - 1;

    int ret = p->write(command, reply, &reply_len);
    if (ret == 0) {
      info.GetReturnValue().Set(Nan::CopyBuffer(reply, reply_len).ToLocalChecked());
    } else {
      info.GetReturnValue().SetUndefined();
    }
  } else {
    info.GetReturnValue().SetUndefined();
  }
}

NODE_MODULE(NODE_GYP_MODULE_NAME, WpaClientSocket::Init);
