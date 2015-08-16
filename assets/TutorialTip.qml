import bb.cascades 1.3

Delegate
{
    id: tutorialDelegate
    property bool suppressTutorials
    property variant data: []
    property variant keys: {}
    property int currentIndex: -1
    signal tutorialFinished(string key)
    
    function count() {
        return data.length;
    }
    
    function reset()
    {
        data = [];
        keys = {};
        
        if (active) {
            object.close();
            active = false;
        }
    }
    
    function onSettingChanged(newValue, key) {
        suppressTutorials = newValue == 1;
    }
    
    onSuppressTutorialsChanged: {
        persist.setFlag("suppressTutorials", suppressTutorials ? 1 : 0);
    }
    
    onCreationCompleted: {
        persist.registerForSetting(tutorialDelegate, "suppressTutorials", true);
    }
    
    function showNext()
    {
        if (currentIndex < data.length)
        {
            ++currentIndex;
            var current = data[currentIndex];
            object.apply(current);
        }
    }
    
    onObjectChanged: {
        if (object) {
            object.open();
        }
    }
    
    function isTopPane(navPane, p) {
        return navPane.parent && navPane.parent.parent ? navPane.parent.parent.activePane == navPane && navPane.top == p : navPane.parent ? navPane.parent.activePane == navPane && navPane.top == p : navPane.top == p;
    }
    
    function execBelowTitleBar(key, text, topPadding, pos, type, imageSource)
    {
        var ui = Application.scene.ui;
        
        if (!topPadding) {
            topPadding = 0;
        }

        return exec( key, text, pos == "l" ? HorizontalAlignment.Left : pos == "r" ? HorizontalAlignment.Right : HorizontalAlignment.Center, VerticalAlignment.Top, 0, 0, ui.du(14) + topPadding, 0, imageSource, type );
    }
    
    function execCentered(key, text, imageSource)
    {
        return exec(key, text, HorizontalAlignment.Center, VerticalAlignment.Center, 0, 0, 0, 0, imageSource);
    }
    
    function execActionBar(key, text, pos)
    {
        if (!Application.scene) {
            return false;
        }
        
        var ui = Application.scene.ui;
        
        return exec( key, text, pos == "x" ? HorizontalAlignment.Right : pos == "b" ? HorizontalAlignment.Left : HorizontalAlignment.Center, VerticalAlignment.Bottom, pos == "r" ? ui.du(31) : pos == "b" ? ui.du(2) : 0, pos == "l" ? ui.du(31) : 0, 0, ui.du(2) );
    }
    
    function execSwipe(key, text, h, v, direction)
    {
        if (!Application.scene) {
            return false;
        }
        
        var ui = Application.scene.ui;
        
        exec(key, text, h, v, 0, 0, 0, 0, undefined, direction);
    }
    
    function exec(key, text, h, v, left, right, top, bottom, imageUri, type)
    {
        if (key) {
            key = "tutorial" + key.charAt(0).toUpperCase() + key.slice(1);
        }
        
        if ( text.length > 0 && !suppressTutorials && ( !key || !persist.containsFlag(key) ) )
        {
            var allKeys = keys;

            if ( !(key in allKeys) )
            {
                var allData = data;
                allData.push( {'key': key, 'body': text, 'icon': imageUri, 'h': h, 'v': v, 'l': left, 'r': right, 't': top, 'b': bottom, 'type': type} );
                data = allData;
                
                if (key) {
                    allKeys[key] = allData.length-1;
                    keys = allKeys;
                }
                
                if (!active) {
                    active = true;
                } else if (active && object) {
                    object.open();
                }
            }
            
            return true;
        }
        
        return false;
    }
    
    function onFinished(confirmed, data)
    {
        if (data.cookie == "video")
        {
            if (confirmed) {
                persist.openUri(data.value);
            }
            
            persist.setFlag("tutorialVideo", Application.applicationVersion);
        }
        
        reporter.record( "TutorialPromptResult", data.cookie+":"+confirmed.toString() );
    }
    
    
    function promptVideo(uri)
    {
        if ( !suppressTutorials && reporter.online && ( persist.getFlag("tutorialVideo") != Application.applicationVersion ) )
        {
            persist.showDialog( tutorialDelegate, {'cookie': 'video', 'value': uri}, qsTr("Video Tutorial"), qsTr("Would you like to see a video tutorial on how to use the app?"), qsTr("Yes"), qsTr("No") );
            return true;
        }
        
        return false;
    }
    
    sourceComponent: ComponentDefinition
    {
        Dialog
        {
            id: fsd
            property variant current: {}
            
            function apply(d)
            {
                swipeAnim.stop();
                
                current = d;
                assetContainer.resetTranslation();
                assetContainer.horizontalAlignment = current.h != undefined ? current.h : HorizontalAlignment.Center;
                assetContainer.verticalAlignment = current.v != undefined ? current.v : VerticalAlignment.Center;
                bodyControl.text = current.body;
                bodyLabel.verticalAlignment = assetContainer.horizontalAlignment == HorizontalAlignment.Center && assetContainer.verticalAlignment == VerticalAlignment.Center ? VerticalAlignment.Top : VerticalAlignment.Center
                icon.imageSource = current.icon ? current.icon : current.type == "d" ? "images/menu/ic_bottom.png" : current.type == "u" ? "images/menu/ic_top.png" : current.type == "r" ? "images/tutorial/ic_next.png" : current.type == "l" ? "images/tutorial/ic_prev.png" : "images/tutorial/pointer.png";
                
                swipeAnim.resetFromY();
                swipeAnim.resetToY();
                swipeAnim.resetFromX();
                swipeAnim.resetToX();
                
                if (current.type == "r")
                {
                    swipeAnim.fromX = -ui.du(2);
                    swipeAnim.toX = ui.du(45);
                    swipeAnim.play();
                } else if (current.type == "l") {
                    swipeAnim.fromX = ui.du(2);
                    swipeAnim.toX = -ui.du(45);
                    swipeAnim.play();
                } else if (current.type == "d") {
                    swipeAnim.fromY = -ui.du(2);
                    swipeAnim.toY = ui.du(45);
                    swipeAnim.play();
                } else if (current.type == "u") {
                    swipeAnim.fromY = ui.du(2);
                    swipeAnim.toY = -ui.du(45);
                    swipeAnim.play();
                }
            }
            
            function dismiss()
            {
                var key = current.key;
                
                if (key && key.length > 0)
                {
                    persist.setFlag(key, 1);
                    
                    key = key.substring("tutorial".length);
                    key = key.charAt(0).toLowerCase() + key.slice(1);
                    tutorialFinished(key);
                }
                
                if (currentIndex < data.length-1) {
                    showNext();
                } else {
                    fsd.close();
                }
            }
            
            onOpened: {
                mainAnim.play();
            }
            
            onClosed: {
                icon.resetImageSource();
                bodyControl.resetText();
            }
            
            onCreationCompleted: {
                open();
            }
            
            Container
            {
                id: dialogContainer
                preferredWidth: Infinity
                preferredHeight: Infinity
                background: Color.create(0.0, 0.0, 0.0, 0.7)
                layout: DockLayout {}
                opacity: 0
                
                onCreationCompleted: {
                    if ( "navigation" in dialogContainer ) {
                        var nav = dialogContainer.navigation;
                        nav.defaultHighlightEnabled = false;
                    }
                }
                
                contextActions: [
                    ActionSet
                    {
                        title: qsTr("Stop all Tutorials") + Retranslate.onLanguageChanged
                        subtitle: qsTr("Prevent any tutorials from showing...") + Retranslate.onLanguageChanged
                        
                        DeleteActionItem
                        {
                            id: dai
                            imageSource: "images/bugs/ic_bugs_cancel.png"
                            title: qsTr("Suppress Tutorials") + Retranslate.onLanguageChanged
                            
                            function onFinished(confirmed)
                            {
                                if (confirmed)
                                {
                                    suppressTutorials = true;
                                    data = [];
                                    keys = {};
                                    fsd.dismiss();
                                }
                                
                                reporter.record( "SuppressTutorialConfirm", confirmed.toString() );
                            }
                            
                            onTriggered: {
                                console.log("UserEvent: SuppressTutorials");
                                persist.showDialog( dai, qsTr("Suppress Tutorials"), qsTr("Are you sure you want to prevent all further tutorials? If you ever want to enable them again, swipe-down from the top-bezel, go to Help, tap on the title bar and uncheck the 'Suppress Tutorials' checkmark.") );
                                
                                reporter.record("SuppressTutorials");
                            }
                        }
                    }
                ]
                
                gestureHandlers: [
                    TapHandler
                    {
                        onTapped: {
                            if (current.key) {
                                console.log("UserEvent: TutorialTapped", current.key);
                            } else {
                                console.log("UserEvent: FixedTutorialTapped");
                            }
                            
                            if ( !mainAnim.isPlaying() ) {
                                fsd.dismiss();
                            }
                        }
                    }
                ]
                
                Container
                {
                    layout: DockLayout {}
                    horizontalAlignment: HorizontalAlignment.Fill
                    verticalAlignment: VerticalAlignment.Fill
                    
                    Container
                    {
                        id: assetContainer
                        opacity: 0
                        scaleX: 0
                        scaleY: 0
                        leftPadding: fsd.current.l ? fsd.current.l : 0;
                        rightPadding: fsd.current.r ? fsd.current.r : 0;
                        bottomPadding: fsd.current.b ? fsd.current.b+iconLuh.layoutFrame.height/2 : 0;
                        topPadding: fsd.current.t ? fsd.current.t-iconLuh.layoutFrame.height/2 : 0;
                        
                        ImageView
                        {
                            id: icon
                            loadEffect: ImageViewLoadEffect.FadeZoom
                            
                            attachedObjects: [
                                LayoutUpdateHandler {
                                    id: iconLuh
                                }
                            ]
                            
                            onCreationCompleted: {
                                if ( "navigation" in icon ) {
                                    var nav = icon.navigation;
                                    nav.focusPolicy = 0x2;
                                    nav.defaultHighlightEnabled = false;
                                }
                            }
                        }
                        
                        animations: [
                            ParallelAnimation
                            {
                                id: breathe
                                repeatCount: AnimationRepeatCount.Forever
                                
                                SequentialAnimation
                                {
                                    ScaleTransition {
                                        fromX: 1
                                        toX: 1.2
                                        fromY: 1
                                        toY: 1.2
                                        easingCurve: StockCurve.ExponentialOut
                                        duration: 750
                                    }
                                    
                                    ScaleTransition {
                                        fromX: 1.2
                                        toX: 1
                                        fromY: 1.2
                                        toY: 1
                                        easingCurve: StockCurve.ExponentialIn
                                        duration: 750
                                    }
                                }
                            },
                            
                            TranslateTransition
                            {
                                id: swipeAnim
                                repeatCount: AnimationRepeatCount.Forever
                                easingCurve: StockCurve.ExponentialOut
                                duration: 2000
                                delay: 500
                            }
                        ]
                    }
                    
                    Container
                    {
                        id: bodyLabel
                        scaleX: 1.25
                        scaleY: 1.25
                        opacity: 0
                        horizontalAlignment: HorizontalAlignment.Fill
                        verticalAlignment: VerticalAlignment.Center
                        leftPadding: 10; rightPadding: 10; topPadding: 10; bottomPadding: 10
                        
                        Label
                        {
                            id: bodyControl
                            textStyle.color: Color.White
                            textStyle.fontStyle: FontStyle.Italic
                            textStyle.textAlign: TextAlign.Center
                            horizontalAlignment: HorizontalAlignment.Fill
                            verticalAlignment: VerticalAlignment.Center
                            multiline: true
                        }
                    }
                }
            }
            
            attachedObjects: [
                SequentialAnimation
                {
                    id: mainAnim
                    
                    FadeTransition {
                        id: dialogFt
                        target: dialogContainer
                        fromOpacity: 0
                        toOpacity: 1
                        duration: 250
                        easingCurve: StockCurve.SineOut
                    }
                    
                    ParallelAnimation
                    {
                        FadeTransition
                        {
                            id: toastIconFt
                            fromOpacity: 0
                            toOpacity: 0.7
                            target: assetContainer
                            duration: 400
                            easingCurve: StockCurve.ExponentialInOut
                        }
                        
                        ScaleTransition
                        {
                            id: toastIconRt
                            fromX: 0
                            toX: 1
                            fromY: 0
                            toY: 1
                            target: assetContainer
                            duration: 500
                            delay: 250
                            easingCurve: StockCurve.CubicInOut
                        }
                    }
                    
                    ParallelAnimation
                    {
                        target: bodyLabel
                        
                        FadeTransition
                        {
                            id: bodyLabelFt
                            fromOpacity: 0
                            toOpacity: 1
                            duration: 250
                            easingCurve: StockCurve.QuadraticOut
                        }
                        
                        ScaleTransition
                        {
                            id: bodyLabelSt
                            fromX: 1.2
                            fromY: 1.2
                            toX: 1
                            toY: 1
                            duration: 500
                            easingCurve: StockCurve.ElasticOut
                        }
                    }
                    
                    onEnded: {
                        breathe.play();
                        showNext();
                    }
                }
            ]
        }
    }
}