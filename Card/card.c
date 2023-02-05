#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include<ctype.h>
#include "card.h"

#define MAX_NAME_SIZE   (uint8_t)27
#define MAX_EXP_DATE    (uint8_t)8
#define MAX_PAN_SIZE    (uint8_t)22




static bool isAllowedFormat(uint8_t *expdate)
{
	uint8_t month=0;
	bool flag=false;
	if(expdate[0]>='0'&&expdate[0]<='1'&&expdate[1]>='0'&&expdate[1]<='9'&&expdate[3]>='0'&&expdate[3]<='9'&&expdate[4]>='0'&&expdate[4]<='9'&&expdate[2]=='/')
	{
		month=(expdate[0]-'0')*10+(expdate[1]-'0');
		if(month>0&&month<=12)
		{
			flag=true;
		}
	}
	return flag;
	
}

static bool isAlphanumeric(uint8_t *pan)
{
	bool flag=true;
	for(uint8_t i=0;pan[i];i++)
	{
		if(!isalnum(pan[i]))
		{
			flag=false;
			break;
		}
	}
	return flag;
}


EN_cardError_t getCardHolderName(ST_cardData_t *cardData)
{
	uint8_t *name =(uint8_t*)malloc(sizeof(uint8_t)*MAX_NAME_SIZE);
	uint8_t len=0;
	EN_cardError_t result=WRONG_NAME;
	while(true)
	{
		printf("Enter Card Holder's Name: ");
		fgets(name,MAX_NAME_SIZE,stdin);
		fflush(stdin);
		len=strlen(name)-1;
		name[len]=0;//to get rid of \n and put null
		if(len>=(uint8_t)20&&len<=(uint8_t)24)
		{
			result=CARD_OK;
			strcpy(cardData->cardHolderName,name);
			break;
		}
		printf("Invalid Holder Name\n");
	}
	free(name);
	return result;
}


EN_cardError_t getCardExpiryDate(ST_cardData_t *cardData)
{
	uint8_t *expdate=(uint8_t*)malloc(sizeof(uint8_t)*MAX_EXP_DATE);
	uint8_t len=0;
	EN_cardError_t result =WRONG_EXP_DATE;
	while(true)
	{
		printf("Enter Card Expiry Date: ");
		fgets(expdate,MAX_EXP_DATE,stdin);
		fflush(stdin);
		len=strlen(expdate)-1;
		expdate[len]=0;
		if(len==(uint8_t)5&&isAllowedFormat(expdate))
		{
			result=CARD_OK;
			strcpy(cardData->cardExpirationDate,expdate);
			break;
		}
		printf("Invalid Expiry Date\n");
	}
	free(expdate);
	return result;
	
}

EN_cardError_t getCardPAN(ST_cardData_t *cardData)
{
	uint8_t *pan=(uint8_t*)malloc(sizeof(uint8_t)*MAX_PAN_SIZE);
	uint8_t len=0;
	EN_cardError_t result =WRONG_PAN;
	while(true)
	{
		printf("Enter Card Pan: ");
		fgets(pan,MAX_PAN_SIZE,stdin);
		fflush(stdin);
		len=strlen(pan)-1;
		pan[len]=0;
		if(len>=(uint8_t)16&&len<=(uint8_t)19&&isAlphanumeric(pan))
		{
			result=CARD_OK;
			strcpy(cardData->primaryAccountNumber,pan);
			break;
		}
		printf("Invalid Pan\n");
	}
	free(pan);
	return result;
}

