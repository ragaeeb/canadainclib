import bb.cascades 1.3

ListView
{
    rearrangeHandler
    {
        property int itemOrigin: -1
        property int lastItemDestination: -1
        
        function undoItemMove()
        {
            if (lastItemDestination != -1 && itemOrigin != -1)
            {
                dataModel.move(lastItemDestination, itemOrigin);
                itemOrigin = -1;
                lastItemDestination = -1;
            }
        }
        
        onMoveUpdated: {
            event.denyMove();
            lastItemDestination = event.toIndexPath[0];
            dataModel.move(event.fromIndexPath[0], event.toIndexPath[0]);
        }
        
        onMoveAborted: {
            undoItemMove();
        }
        
        onMoveStarted: {
            itemOrigin = event.startIndexPath[0];
        }
    }
}