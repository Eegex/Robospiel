#include "LeaderBoardWidget.h"
#include "PlayerBiddingWidget.h"

LeaderBoardWidget::LeaderBoardWidget(QWidget *parent) : QWidget(parent)
{
    dummyWidget[0] = new PlayerBiddingWidget(this);
    dummyWidget[0]->setName("TestName");
    dummyWidget[0]->setBidding(10);
    lay->addWidget(dummyWidget[0]);
}

void LeaderBoardWidget::sortByBidding()
{ //Sort by Bidding, Person with the lowest bid at the top, might not be a good idea
    PlayerBiddingWidget * sortedPlayers;
    sortedPlayers = (PlayerBiddingWidget*) calloc(numOfPlayers, sizeof(PlayerBiddingWidget*));
    u_int16_t minBid = dummyWidget[0]->getBidding();
    for(u_int8_t i = 0; i<numOfPlayers-1; i++)
    {
        //Do Stuff, this has to be done better
        //lay->addWidget(lay->takeAt(0), i, 0);
    }
    free(sortedPlayers);
}
