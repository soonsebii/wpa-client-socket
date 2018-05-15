var os = require('os');
var events = require('events');

var platform = os.platform();

if (platform === 'linux') {
  var addon = require('./build/Release/addon.node');
  var WpaSupplicantClientSocket = addon.WpaSupplicantClientSocket;

  for (var k in events.EventEmitter.prototype) {
    WpaSupplicantClientSocket.prototype[k] = events.EventEmitter.prototype[k];
  }

  module.exports = WpaSupplicantClientSocket;
} else {
  throw new Error('Unsupported platform');
}
