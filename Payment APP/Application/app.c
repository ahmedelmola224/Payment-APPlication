#include <stdio.h>
#include <stdlib.h>
#include "../card/card.h"
#include "../terminal/terminal.h"
#include "../server/server.h"
#include "app.h"

void appStart(void)
{
	ST_transaction_t transction;
	getCardHolderName(&(transction.cardHolderData));
	getCardExpiryDate(&(transction.cardHolderData));
	getCardPAN(&(transction.cardHolderData));
	setMaxAmount(&(transction.terminalData));
	getTransactionAmount(&(transction.terminalData));
	getTransactionDate(&(transction.terminalData));
	
	if(isCardExpired((transction.cardHolderData),(transction.terminalData))==EXPIRED_CARD)
	{
		printf("Your card is expired\n");	
		return; 		
	}
	
	if(isValidCardPAN(&(transction.cardHolderData))==INVALID_CARD)
	{
		printf("Invalid Card PAN\n");	
		return; 		
	}
	
	
	if(isBelowMaxAmount(&(transction.terminalData))==EXCEED_MAX_AMOUNT)
	{
		printf("Exceeded the max amount of the terminal\n");	
		return; 		
	}
	
	EN_transState_t transctionstate=recieveTransactionData(&transction);
	switch(transctionstate)
	{
		case DECLINED_STOLEN_CARD:
			printf("This card is Blocked\n");
			break;
		case DECLINED_INSUFFECIENT_FUND:
			printf("This card is havn't suffecient fund\n");
			break;
		case INTERNAL_SERVER_ERROR:	
			printf("Server Error\n");
			break;
		case APPROVED:
			printf("Approved Transction\n");
			break;
		default:
			printf("Please, Try in another time\n");
			break;
	}
	
	
	
	
	
	
	
}