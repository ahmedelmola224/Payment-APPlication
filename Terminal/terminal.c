#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include "../card/card.h"
#include "terminal.h"


static void getCurrentDate(uint8_t *str)
{
	time_t t = time(NULL);
    struct tm tm = *localtime(&t);
	uint8_t day=tm.tm_mday;
	uint8_t month=tm.tm_mon +(uint8_t)1;
	uint16_t year =tm.tm_year +(uint16_t) 1900;
	sprintf(str,"%d/%d/%d",day,month ,year);
}

static bool isLuhn(uint8_t *str)
{
    int nDigits =strlen(str);
 
    int nSum = 0, isSecond = false;
    for (int i = nDigits - 1; i >= 0; i--) {
 
        int d = str[i] - '0';
 
        if (isSecond == true)
            d = d * 2;
 
        // We add two digits to handle
        // cases that make two digits after
        // doubling
        nSum += d / 10;
        nSum += d % 10;
 
        isSecond = !isSecond;
    }
    return (nSum % 10 == 0);
}


EN_terminalError_t getTransactionDate(ST_terminalData_t *termData)
{
	getCurrentDate(termData->transactionDate);
	return TERMINAL_OK;
}


EN_terminalError_t isCardExpired(ST_cardData_t cardData, ST_terminalData_t termData)
{
	EN_terminalError_t result=EXPIRED_CARD;
	int cardyear,cardmonth,tranctionyear,tranctionmonth,tranctionday;
	sscanf(cardData.cardExpirationDate,"%d/%d",&cardmonth,&cardyear);
	sscanf(termData.transactionDate,"%d/%d/%d",&tranctionday,&tranctionmonth,&tranctionyear);
	tranctionyear%=100;
	if(tranctionyear<cardyear)
		result=TERMINAL_OK;
	else if(tranctionyear==cardyear)
	{
		if(tranctionmonth<=cardmonth)
		{
			result=TERMINAL_OK;
		}
	}
	return result;
}


EN_terminalError_t isValidCardPAN(ST_cardData_t *cardData)
{
	EN_terminalError_t result=INVALID_CARD;
	if(isLuhn(cardData->primaryAccountNumber)==true)
	{
		result=TERMINAL_OK;
	}
	return result;
	
}


EN_terminalError_t getTransactionAmount(ST_terminalData_t *termData)
{
	float amount;
	EN_terminalError_t result=INVALID_AMOUNT;
	while(true)
	{
		printf("Enter Transction Amount: ");
		scanf("%f",&amount);
		if(amount>(uint8_t)0)
		{
			termData->transAmount=amount;
			result=TERMINAL_OK;
			break;
		}
		printf("Invalid Transction Amount\n");
	}
	
	return result;
}


EN_terminalError_t isBelowMaxAmount(ST_terminalData_t *termData)
{
	EN_terminalError_t result=EXCEED_MAX_AMOUNT;
	if((termData->transAmount) <= (termData->maxTransAmount))
	{
		result=TERMINAL_OK;
	}
	return result;
	
}


EN_terminalError_t setMaxAmount(ST_terminalData_t *termData)
{
	int maxamount;
	EN_terminalError_t result=INVALID_MAX_AMOUNT;
	while(true)
	{
		printf("Enter Max Allowed Amount: ");
		scanf("%d",&maxamount);
		if(maxamount>(uint8_t)0)
		{
			termData->maxTransAmount=maxamount;
			result=TERMINAL_OK;
			break;
		}
		printf("Invalid Max Transction Amount\n");
	}
	return result;
}