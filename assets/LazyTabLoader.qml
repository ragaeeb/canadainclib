import bb.cascades 1.0

QtObject
{
    property ComponentDefinition definition: ComponentDefinition {}
    
    function lazyLoad(actualSource, tab)
    {
        if (!tab.content)
        {
            definition.source = actualSource;
            
            var actual = definition.createObject();
            tab.content = actual;
            
            return actual;
        } else {
            return tab.content;
        }
    }
}