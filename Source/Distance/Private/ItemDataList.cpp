// Fill out your copyright notice in the Description page of Project Settings.

#include "Distance.h"
#include "ItemDataList.h"

class TSubclassOf<AItem> UItemDataList::ItemAtIndex(uint32 index)
{
	if (ItemTypes.IsValidIndex(index))
	{
		return ItemTypes[index];
	}
	else
	{
		printScreen(FColor::Red, "Not valid index in Data List");
		return NULL;
	}
}


