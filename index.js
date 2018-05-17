var os = require('os');
var events = require('events');

var platform = os.platform();

if (platform === 'linux') {
  var addon = require('./build/Release/addon.node');
  var WpaClientSocket = addon.WpaClientSocket;

  for (var k in events.EventEmitter.prototype) {
    WpaClientSocket.prototype[k] = events.EventEmitter.prototype[k];
  }

  module.exports = WpaClientSocket;
} else {
  throw new Error('Unsupported platform');
}
