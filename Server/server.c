#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include "../card/card.h"
#include "../terminal/terminal.h"
#include "server.h"

#define MAX_ACCOUNTS_SIZE (uint8_t)255
#define MAX_TRANSCTIONS_SIZE (uint8_t)255

static uint32_t NextValidAccountsIndex=(uint32_t)2;
static ST_accountsDB_t ValidAccounts[MAX_ACCOUNTS_SIZE]={{1500.0f,"20210917131347022"},{123456.0f,"4101012665149063"}};

static uint32_t NextTransctionIndex=(uint32_t)2;
static ST_transaction_t Transctions[MAX_TRANSCTIONS_SIZE]={{{"ahmed essam nagah elmola","20210917131347022","05/23"},{200.0f,300.0f,"25/01/2023"},APPROVED,1},
															{{"mohamed ali salah sonbol","4101012665149063","06/24"},{100.0f,300.0f,"01/06/2023"},APPROVED,2}};

static uint32_t NextStolenIndex=(uint32_t)2;
static ST_accountsDB_t StolenAccounts[MAX_ACCOUNTS_SIZE]={{2500.0f,"4036493292234835"}};





static int16_t findAccount(ST_cardData_t *cardData)
{
	int16_t index=-1;
	for(uint8_t i=0;i<NextValidAccountsIndex;i++)
	{
		if(strcmp(ValidAccounts[i].primaryAccountNumber,cardData->primaryAccountNumber)==0)
		{
			index=i;
			break;
		}
	}
	return index;
	
}

void blockStolenCard(void)
{
	ST_cardData_t stolencard;
	getCardPAN(&stolencard);
	strcpy(StolenAccounts[NextStolenIndex].primaryAccountNumber,stolencard.primaryAccountNumber);
	NextStolenIndex++;
}


static void editBalance(ST_transaction_t *transData)
{
	int16_t accountindex=findAccount(&(transData->cardHolderData));
	ValidAccounts[accountindex].balance-=transData->terminalData.transAmount;
	printf("After the Transctions your balance is %.2f \n",ValidAccounts[accountindex].balance);
}


EN_transState_t recieveTransactionData(ST_transaction_t *transData)
{
	if(isValidAccount(&(transData->cardHolderData))==ACCOUNT_NOT_FOUND)
	{
		return DECLINED_STOLEN_CARD;
	}
	
	if(isAmountAvailable(transData)==LOW_BALANCE)
	{
		return DECLINED_INSUFFECIENT_FUND;
	}
	
	if(saveTransaction(transData)==SAVING_FAILED)
	{
		return INTERNAL_SERVER_ERROR;
	}
	
	/*Edit the balance*/
	editBalance(transData);
	return APPROVED;
}

EN_serverError_t isValidAccount(ST_cardData_t *cardData)
{
	EN_serverError_t result=ACCOUNT_NOT_FOUND;
	//search in the stolen accounts Data Base
	for(uint8_t i=(uint8_t)0;i<NextStolenIndex;i++)
	{
		if(strcmp(StolenAccounts[i].primaryAccountNumber,cardData->primaryAccountNumber)==0)
		{
			return ACCOUNT_NOT_FOUND;
		}
	}
	
	//search in the Valid accounts
	for(uint8_t i=(uint8_t)0;i<NextValidAccountsIndex;i++)
	{
		if(strcmp(ValidAccounts[i].primaryAccountNumber,cardData->primaryAccountNumber)==0)
		{
			result =SERVER_OK;
			break;
		}
	}
	
	return result;
}


EN_serverError_t isAmountAvailable(ST_transaction_t *transData)
{
	EN_serverError_t result=LOW_BALANCE;
	int16_t accountindex=findAccount(&(transData->cardHolderData));
	if(transData->terminalData.transAmount<=ValidAccounts[accountindex].balance)
	{
		result=SERVER_OK;
	}
	
	return result;
}


EN_serverError_t saveTransaction(ST_transaction_t *transData)
{
	if(NextTransctionIndex>MAX_TRANSCTIONS_SIZE)
	{
		return SAVING_FAILED;
	}
	/*Storing Card Data*/
	strcpy(Transctions[NextTransctionIndex].cardHolderData.cardHolderName,transData->cardHolderData.cardHolderName);
	strcpy(Transctions[NextTransctionIndex].cardHolderData.primaryAccountNumber,transData->cardHolderData.primaryAccountNumber);
	strcpy(Transctions[NextTransctionIndex].cardHolderData.cardExpirationDate,transData->cardHolderData.cardExpirationDate);
	
	/*Storing Terminal Data*/
	strcpy(Transctions[NextTransctionIndex].terminalData.transactionDate,transData->terminalData.transactionDate);
	Transctions[NextTransctionIndex].terminalData.transAmount=transData->terminalData.transAmount;
	Transctions[NextTransctionIndex].terminalData.maxTransAmount=transData->terminalData.maxTransAmount;
	
	/*Storing Rest Of Data*/
	Transctions[NextTransctionIndex].transState=transData->transState;
	Transctions[NextTransctionIndex].transactionSequenceNumber=NextTransctionIndex*(uint8_t)3;
	NextTransctionIndex++;
	
	return SERVER_OK;
}


EN_serverError_t getTransaction(uint32_t transactionSequenceNumber, ST_transaction_t *transData)
{
	EN_serverError_t result =TRANSACTION_NOT_FOUND;
	uint8_t i=(uint8_t)0;
	for(;i<MAX_TRANSCTIONS_SIZE;i++)
	{
		if(transactionSequenceNumber==Transctions[i].transactionSequenceNumber)
		{
			result =SERVER_OK;
			break;
		}
	}
	
	if(result==SERVER_OK)
	{
		/*Storing Card Data*/
		strcpy(transData->cardHolderData.cardHolderName,Transctions[i].cardHolderData.cardHolderName);
		strcpy(transData->cardHolderData.primaryAccountNumber,Transctions[i].cardHolderData.primaryAccountNumber);
		strcpy(transData->cardHolderData.cardExpirationDate,Transctions[i].cardHolderData.cardExpirationDate);
		
		/*Storing Terminal Data*/
		strcpy(transData->terminalData.transactionDate,Transctions[i].terminalData.transactionDate);
	    transData->terminalData.transAmount=Transctions[i].terminalData.transAmount;
	    transData->terminalData.maxTransAmount=Transctions[i].terminalData.maxTransAmount;
		
		/*Storing Rest Of Data*/
		transData->transState=Transctions[i].transState;
	    transData->transactionSequenceNumber=Transctions[i].transactionSequenceNumber;
	}
	
	
	return result;
}

