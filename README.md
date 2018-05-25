# wpa-client-socket

WPA Client socket binding for Node.js

wpa-client-socket is clearly inspired by the [Noble](https://github.com/noble/noble).

## Prerequisites
__NOTE:__ This only works on linux.

* wpa_supplicant

## Reference
[http://w1.fi/wpa_supplicant/devel/ctrl_iface_page.html](http://w1.fi/wpa_supplicant/devel/ctrl_iface_page.html)

## Examples
```javascript
var WpaClientSocket = require('wpa-client-socket');
var socket = new WpaClientSocket();

var ctrl_path = '/var/run/wpa_supplicant/wlan0';

socket.on('data', function(buf) {
  console.log(buf.toString());
});

socket.bind(ctrl_path);
socket.start();

var result = socket.write('SCAN');

if (result.toString() == 'OK\n')
  console.log('write success');

```
