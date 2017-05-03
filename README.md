# devices.js

[![AppVeyor Build Status](https://ci.appveyor.com/api/projects/status/0ckgpa2jojjd1wpc?svg=true)](https://ci.appveyor.com/project/async3619/devices-js)
[![Build Dependencies](https://david-dm.org/async3619/devices.js/status.svg)](https://david-dm.org/async3619/devices.js)
[![NPM published version](https://img.shields.io/npm/v/devices.js.svg)](https://www.npmjs.com/package/devices.js)
[![Project license](https://img.shields.io/npm/l/devices.js.svg)]()

Handle media devices (webcam, microphone) with node.js.  
This library follows W3C specification of [navigator.mediaDevices](https://www.w3.org/TR/mediacapture-streams/#mediadevices) perfectly.

## Installation

```bash
npm install devicesjs
```

or use git

``` bash
git clone https://github.com/async3619/devices.js
cd device.js
npm install
```

## Example

```js
const devicesjs = require("devicesjs");
devicesjs.enumerateDevice().then(streams => {
    // do something with streams ...
})

devicejs.ondevicechange = (e) => {
    // do something ...
}
```

## Support table

| Windows (Vista or higher) | Linux | OS X |
|:-------------------------:|:-----:|:----:|
|             O             |   X   |   X  |

## Functions

#### devicesjs.enumerateDevices()

__type:__ _function ()_<br/>
__return:__ _promise.<Device[]>_

get all available media devices that installed on computer.
<br/>
<br/>

#### devicesjs.ondevicechange

__type:__ _function (DeviceChangeEventArgs)_

Event listener that'll be called when device has removed, attached, changed.
<br/>
<br/>

#### DeviceChangeEventArgs

__type:__ class<br/>
__members:__

|  Name  |  Type  | Value                                                   | Description                                                 |
|:------:|:------:|---------------------------------------------------------|-------------------------------------------------------------|
| target | Device | -                                                       | the device that makes event fire.                           |
|  which | string | "active" / "disabled" / "added" / "removed" / "changed" | event type; you can determine device status by this member. |

<br/>

#### Device

__type:__ class <br/>
__members:__

|   Name   |  Type  | Value | Description                                                                             |
|:--------:|:------:|-------|-----------------------------------------------------------------------------------------|
| deviceId | string | -     | contains device unique id.                                                              |
|   label  | string | -     | contains device user-friendly name.<br/> _ex) Microphone (Realtek High Definition Audio)_ |

## License
<img align="right" src="http://opensource.org/trademarks/opensource/OSI-Approved-License-100x137.png">

The project is licensed under the [MIT License](http://opensource.org/licenses/MIT).

Copyright ? 2017 [TerNer (async3619@naver.com)](https://terner.me)

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.