(function (root, factory) {
    module.exports = factory(typeof Promise === "undefined" ? require("promise") : Promise, require('nbind').init().lib);
})(this, function (Promise, library) {
    if (library.DeviceJS.checkCompatible() === true) {
        const lib = library.DeviceJS.getInstance(function (unicodeArray) {
            return String.fromCharCode.apply(null, unicodeArray);
        });

        Object.defineProperty(lib,
            "ondevicechange",
            {
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
    } else {
        return null;
    }
});