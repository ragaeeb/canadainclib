import QtMobility.sensors 1.2

Accelerometer
{
    id: alarm
    signal deviceShook();
    property variant lastUpdate: new Date();
    property variant lastSuccess: new Date()
    property real lastX
    property real lastY
    property real lastZ
    active: true
    accelerationMode: Accelerometer.User // Remove gravity, detect only user movement.
    alwaysOn: true
    axesOrientationMode: Accelerometer.FixedOrientation
    skipDuplicates: true

    onReadingChanged:
    { 
        var now = new Date();
        var diff = now-lastUpdate;
        
        if (diff > 100)
        {
            lastUpdate = now;
            
            var x = reading.x;
            var y = reading.y;
            var z = reading.z;
            
            var speed = Math.abs(x+y+z-lastX-lastY-lastZ) / diff * 10000;
            
            if (speed > 1000 && now-lastSuccess > 5000)
            {
                lastSuccess = now;
                deviceShook();
            }

            lastX = x;
            lastY = y;
            lastZ = z;
        }
    }
}