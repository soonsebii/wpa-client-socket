{
  "targets": [
    {
      "target_name": "addon",
      "dependencies": [ "libwpa_supplicant" ],
      "include_dirs": [ "./src/common", "<!(node -e \"require('nan')\")" ],
      "cflags": [ "-O2" ],
      "sources": [ "./src/WpaClientSocket.cpp" ]
    },
    {
      "target_name": "libwpa_supplicant",
      "type": "static_library",
      "defines": [ "CONFIG_CTRL_IFACE", "CONFIG_CTRL_IFACE_UNIX" ],
      "include_dirs": [ "./src/common", "./src/utils" ],
      "cflags": [ "-Wall", "-Wextra" ],
      "sources": [
        "./src/common/wpa_ctrl.c",
        "./src/utils/os_unix.c"
      ]
    }
  ]
}
