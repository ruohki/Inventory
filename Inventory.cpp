// Fill out your copyright notice in the Description page of Project Settings.

#include "Survisland.h"
#include "Inventory.h"
#include "UnrealNetwork.h"


//
// *** Basic Functionality ***
//
bool UInventory::Initialize(int32 inSlots, TArray<FInventoryStructure> & inInventory) {
	TArray<FInventoryStructure> bInv;
	//bInv.Init(inSlots);
	bInv.SetNum(inSlots);
	inInventory = bInv;

	return true;
}

bool UInventory::GetStackByClass(TSubclassOf<class AMasterItemActor> inClass, bool bReturnFullStacks, TArray<FInventoryStructure> inInventory, FInventoryStructure & outStructure, int32 & FoundIndex) {

	for (auto bIterator(inInventory.CreateIterator()); bIterator; bIterator++)
	{
		if (!bIterator) continue;

		if (bIterator->bClass == inClass) {

			FItemInfo bItemInfo;
			GetItemInfo(inClass, bItemInfo);

			if (bIterator->bCount < bItemInfo.bItemStackSize) {
				outStructure = *bIterator;
				FoundIndex = bIterator.GetIndex();

				return true;
			}
			else {
				if (bReturnFullStacks) {
					outStructure = *bIterator;
					FoundIndex = bIterator.GetIndex();

					return true;
				}
			}

		}

	}

	return false;
}

bool UInventory::GetItemCount(TSubclassOf<class AMasterItemActor> inClass, TArray<FInventoryStructure> inInventory, int32 & ItemCount) {
	int32 intCounter = 0;

	for (auto bIterator(inInventory.CreateIterator()); bIterator; bIterator++)
	{
		if (!bIterator) continue;

		if (bIterator->bClass == inClass) {
			intCounter += bIterator->bCount;
		}

	}

	if (intCounter <= 0)
		return false;

	ItemCount = intCounter;
	return true;
}

bool UInventory::GetItemStackCount(TSubclassOf<class AMasterItemActor> inClass, TArray<FInventoryStructure> inInventory, int32 & StackCount) {
	int32 intCounter = 0;

	for (auto bIterator(inInventory.CreateIterator()); bIterator; bIterator++)
	{
		if (!bIterator) continue;

		if (bIterator->bClass == inClass) {
			intCounter++;
		}

	}

	if (intCounter <= 0)
		return false;

	StackCount = intCounter;
	return true;
}

bool UInventory::GetStackByIndex(int32 inIndex, TArray<FInventoryStructure> inInventory, FInventoryStructure & outStructure) {
	if ((inIndex < 0) || (inIndex > inInventory.Num()))
		return false;

	outStructure = inInventory[inIndex];
	return true;
}

bool UInventory::GetStackByID(FString inID, TArray<FInventoryStructure> inInventory, FInventoryStructure & outStructure, int32 & FoundIndex) {

	if (inID == "")
		return false;

	for (auto bIterator(inInventory.CreateIterator()); bIterator; bIterator++)
	{
		if (!bIterator) continue;

		if (FString(*bIterator->bID) == inID) {
			outStructure = *bIterator;
			FoundIndex = bIterator.GetIndex();

			return true;
		}
	}

	return false;
}

bool UInventory::GetFreeInventorySpace(TSubclassOf<class AMasterItemActor> inClass, TArray<FInventoryStructure> inInventory, int32 & outCount) {
	int32 intCounter = 0, intSlots = 0;

	FItemInfo bItemInfo;

	for (auto bIterator(inInventory.CreateIterator()); bIterator; bIterator++)
	{
		if (!bIterator) continue;

		if (bIterator->bClass == inClass) {
			GetItemInfo(inClass, bItemInfo);
			if (bIterator->bCount < bItemInfo.bItemStackSize) {
				intCounter += (bItemInfo.bItemStackSize - bIterator->bCount);
			}
		}

	}

	GetItemInfo(inClass, bItemInfo);
	GetFreeInventorySlots(inInventory, intSlots);
	intCounter += intSlots * bItemInfo.bItemStackSize;

	if (intCounter <= 0)
		return false;

	outCount = intCounter;
	return true;
}

bool UInventory::GetFreeInventorySlots(TArray<FInventoryStructure> inInventory, int32 & outCount) {
	int32 intCounter = 0;

	for (auto bIterator(inInventory.CreateIterator()); bIterator; bIterator++)
	{
		if (!bIterator) continue;

		if (bIterator->bClass == NULL) {
			intCounter++;
		}

	}

	if (intCounter <= 0)
		return false;

	outCount = intCounter;
	return true;
}

//
// This function returns a Structure with the default variables
//
bool UInventory::GetItemInfo(TSubclassOf<AMasterItemActor> bItemClass, FItemInfo & bIteminfo) {
	if (!bItemClass.GetDefaultObject())
		return false;

	bIteminfo = bItemClass.GetDefaultObject()->bIteminfo;
	return true;
}

//
// Generates a FString from multible random numbers. This FString can be used as a Unique ID since the chances are good that this ID is Unique in a gamelife cycle
//
// (MIN MAX without division will bug the RandRange)
//
void UInventory::GetUniqueID(FString & bUniqueID) {
	bUniqueID = FString();

	for (int i = 0; i < 5; i++)
		bUniqueID.AppendInt(FMath::RandRange(INT32_MIN / 11, INT32_MAX / 11));
}

int32 UInventory::StacksFromAmount(TSubclassOf<class AMasterItemActor> inClass, int32 inAmount) {
	FItemInfo bItemInfo;
	GetItemInfo(inClass, bItemInfo);

	if (inAmount == 0)
		return 0;

	int32 Amount = FMath::DivideAndRoundUp(inAmount, bItemInfo.bItemStackSize);
	return Amount;
}

bool UInventory::AddToInventory(TSubclassOf<class AMasterItemActor> inClass, TArray<FInventoryStructure>& inInventory, int32 Amount, bool AddNewStack) {
	bool result;
	FItemInfo bItemInfo;
	int32 FoundStack = -1;

	if (Amount <= 0)
		return false;

	if (!AddNewStack) {

		FInventoryStructure bInvStruct;

		result = GetStackByClass(inClass, false, inInventory, bInvStruct, FoundStack);
		if (result) {

			GetItemInfo(inClass, bItemInfo);

			FInventoryStructure bItemStruct;
			GetStackByIndex(FoundStack, inInventory, bItemStruct);

			if (bItemInfo.bItemStackSize < bItemStruct.bCount + Amount) {
				int32 Remainder = bItemInfo.bItemStackSize - bItemStruct.bCount;
				Remainder = Amount - Remainder;

				inInventory[FoundStack].bCount = bItemInfo.bItemStackSize;

				AddToInventory(inClass, inInventory, Remainder, false);

				return true;
			}
			else {
				inInventory[FoundStack].bCount += Amount;

				return true;
			}
		}
		else {
			//New Stack
			GetItemInfo(inClass, bItemInfo);

			if (Amount > bItemInfo.bItemStackSize) {
				//More than 1 Stack
				int32 FreeSlots;
				GetFreeInventorySlots(inInventory, FreeSlots);

				if (StacksFromAmount(inClass, Amount) > FreeSlots)
					return false;

				int32 Remainder = Amount - bItemInfo.bItemStackSize;
				FInventoryStructure bItemStruct;

				result = GetStackByClass(NULL, true, inInventory, bItemStruct, FoundStack);

				if (!result)
					return false;

				inInventory[FoundStack].bCount = bItemInfo.bItemStackSize;
				inInventory[FoundStack].bClass = inClass;

				FString uID;
				GetUniqueID(uID);

				inInventory[FoundStack].bID = uID;

				AddToInventory(inClass, inInventory, Remainder, false);

				return true;
			}
			else {
				//Less than 1 Stack
				FInventoryStructure bItemStruct;

				result = GetStackByClass(NULL, true, inInventory, bItemStruct, FoundStack);

				if (!result)
					return false;

				inInventory[FoundStack].bCount = Amount;
				inInventory[FoundStack].bClass = inClass;

				FString uID;
				GetUniqueID(uID);

				inInventory[FoundStack].bID = uID;

				return true;
			}
		}
	}
	else {
		int32 FreeSlots;
		GetFreeInventorySlots(inInventory, FreeSlots);

		if (StacksFromAmount(inClass, Amount) > FreeSlots)
			return false;

		FItemInfo bItemInfo;
		GetItemInfo(inClass, bItemInfo);

		int32 SlotIndex;
		FInventoryStructure bInventoryStructure;

		GetStackByClass(NULL, true, inInventory, bInventoryStructure, SlotIndex);

		if (bItemInfo.bItemStackSize >= Amount) {
			inInventory[SlotIndex].bClass = inClass;
			inInventory[SlotIndex].bCount = Amount;

			FString bID;
			GetUniqueID(bID);

			inInventory[SlotIndex].bID = bID;

			return true;
		}
		else {
			inInventory[SlotIndex].bClass = inClass;
			inInventory[SlotIndex].bCount = bItemInfo.bItemStackSize;

			FString bID;
			GetUniqueID(bID);

			inInventory[SlotIndex].bID = bID;

			AddToInventory(inClass, inInventory, Amount - bItemInfo.bItemStackSize, true);
			return true;
		}
	}
	return true;
}

bool UInventory::RemoveFromInventory(TSubclassOf<class AMasterItemActor> inClass, TArray<FInventoryStructure>& inInventory, int32 Amount) {
	int32 intInteger, intRemainder;
	FItemInfo bItemInfo;
	FInventoryStructure bInventoryStructure;

	GetItemInfo(inClass, bItemInfo);

	GetItemCount(inClass, inInventory, intInteger);

	if (Amount > intInteger)
		return false;

	GetStackByClass(inClass, true, inInventory, bInventoryStructure, intInteger);

	if (bInventoryStructure.bCount > Amount) {
		inInventory[intInteger].bCount -= Amount;
		return true;
	}
	else {
		intRemainder = Amount - bInventoryStructure.bCount;

		inInventory[intInteger] = FInventoryStructure();

		RemoveFromInventory(inClass, inInventory, intRemainder);
		return true;
	}
}

bool UInventory::RemoveFromStack(int32 inIndex, TArray<FInventoryStructure>& inInventory, int32 Amount, bool RemoveWholeStack) {
	FInventoryStructure bInventoryStructure;

	GetStackByIndex(inIndex, inInventory, bInventoryStructure);

	if (Amount > bInventoryStructure.bCount)
		return false;

	if (RemoveWholeStack) {
		inInventory[inIndex] = FInventoryStructure();
		return true;
	}
	else {
		inInventory[inIndex].bCount -= Amount;
		return true;
	}
}

bool UInventory::SwapInventoryPosition(TArray<FInventoryStructure>& inInventory, int32 inIndexA, int32 inIndexB) {
	FInventoryStructure bInventoryStructure;

	GetStackByIndex(inIndexA, inInventory, bInventoryStructure);

	inInventory[inIndexA] = inInventory[inIndexB];
	inInventory[inIndexB] = bInventoryStructure;

	return true;
}

bool UInventory::SplitStack(TArray<FInventoryStructure>& inInventory, int32 inIndex, int32 inSplit) {
	FInventoryStructure bInventoryStructure;
	int32 Remainder, FreeSlots;

	GetStackByIndex(inIndex, inInventory, bInventoryStructure);

	GetFreeInventorySlots(inInventory, FreeSlots);
	if (FreeSlots < 1)
		return false;

	if ((inSplit >= bInventoryStructure.bCount) || (inSplit <= 0))
		return false;

	Remainder = bInventoryStructure.bCount - inSplit;
	inInventory[inIndex].bCount = Remainder;

	AddToInventory(bInventoryStructure.bClass, inInventory, inSplit, true);
	return true;
}

bool UInventory::MergeStacks(TArray<FInventoryStructure>& inInventory, int32 inIndexA, int32 inIndexB) {
	FItemInfo bItemInfo;
	FInventoryStructure bInventoryStructureA, bInventoryStructureB;

	GetStackByIndex(inIndexA, inInventory, bInventoryStructureA);
	GetStackByIndex(inIndexB, inInventory, bInventoryStructureB);

	GetItemInfo(inInventory[inIndexA].bClass, bItemInfo);

	if (inInventory[inIndexA].bClass != inInventory[inIndexB].bClass)
		return false;

	if (bItemInfo.bItemStackSize >= bInventoryStructureA.bCount + bInventoryStructureB.bCount) {
		inInventory[inIndexA].bCount += inInventory[inIndexB].bCount;

		RemoveFromStack(inIndexB, inInventory, 0, true);
		return true;
	}
	else{
		int32 Remainder = bItemInfo.bItemStackSize - inInventory[inIndexA].bCount;
		inInventory[inIndexA].bCount = bItemInfo.bItemStackSize;
		inInventory[inIndexB].bCount -= Remainder;

		return true;
	}

}

