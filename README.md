# `touch_emulate`
Send input events to your eReader when you're lazy of actually doing it physically.
## Compilation
```
pushd src/ && armv7l-linux-musleabihf-gcc src/touch_emulate.c -o touch_emulate && armv7l-linux-musleabihf-strip touch_emulate && popd
```
## Usage
### Emulate power button event
```
touch_emulate power
```
