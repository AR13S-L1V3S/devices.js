(function (root, factory) {
    module.exports = factory(typeof Promise === "undefined" ? require("promise") : Promise);
})(this, function (Promise) {
    const lib = require('nbind').init(__dirname).lib.DeviceJS.getInstance(function (unicodeArray) {
        return String.fromCharCode.apply(null, unicodeArray);
    });

    Object.defineProperty(lib, "ondevicechange", {
        get: function () {
            return this._odc;
        },
   
        set: function (func) {
            this.setOnDeviceChangeCallback(func);
            this._odc = func;
        }
    });

    const enumerateDevices = lib.enumerateDevices.bind(lib);
    lib.enumerateDevices = function () {
        return new Promise(function (resolve, reject) {
            if (enumerateDevices(resolve) === false)
                reject();
        });
    };

    return lib;
});