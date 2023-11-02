# Linux version of VoipClient depends on WebRTC VoiceEngine

![](https://storage-1258719774.cos.ap-nanjing.myqcloud.com/image/20231014151923.png)

# BUILD
download and build [WebRTC](https://webrtc.github.io/webrtc-org/native-code/development/) first.

then we have 2 way to build voip client

## a. build in WebRTC

1. cd WebRTC src directory and clone this repo to examples/voipclient.
2. change gn build config `examples/BUILD.gn`.
```
--- a/examples/BUILD.gn
+++ b/examples/BUILD.gn
@@ -54,6 +54,7 @@ group("examples") {
       ":peerconnection_server",
       ":stunserver",
       ":turnserver",
+      "voipclient:voip_client",
     ]
     if (current_os != "winuwp") {
       deps += [ ":peerconnection_client" ]
```
3. rebuild WebRTC, executable file will be in build out directory.

## b. build with cmake
[TODO]




-------------------------------------

Android Version is [here](https://webrtc.googlesource.com/src/+/refs/heads/main/examples/androidvoip/)



