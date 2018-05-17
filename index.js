var os = require('os');
var path = require('path');
var events = require('events');

var platform = os.platform();
var binary = path.resolve(path.join(__dirname,'./build/Release/addon.node'));

if (platform === 'linux') {
  var addon = require(binary);
  var WpaClientSocket = addon.WpaClientSocket;

  for (var k in events.EventEmitter.prototype) {
    WpaClientSocket.prototype[k] = events.EventEmitter.prototype[k];
  }

  module.exports = WpaClientSocket;
} else {
  throw new Error('Unsupported platform');
}
