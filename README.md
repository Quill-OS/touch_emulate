# `touch_emulate`
Send input events to your Kobo when you're lazy of actually doing it physically.
<br>
Compile:
```
pushd src/ && armv7l-linux-musleabihf-gcc src/touch_emulate.c -o touch_emulate && armv7l-linux-musleabihf-strip touch_emulate && popd
```
