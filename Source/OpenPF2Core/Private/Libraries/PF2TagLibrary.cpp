// OpenPF2 for UE Game Logic, Copyright 2023-2024, Guy Elsmore-Paddock. All Rights Reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

#include "Libraries/PF2TagLibrary.h"

FGameplayTag UPF2TagLibrary::FindChildTag(const FGameplayTagContainer& AllTags,
                                          const FGameplayTag&          ParentTag,
                                          bool&                        bMatchFound)
{
	FGameplayTag                Result;
	const FGameplayTagContainer AllChildren = AllTags.Filter(FGameplayTagContainer(ParentTag));

	if (AllChildren.IsEmpty())
	{
		bMatchFound = false;
	}
	else
	{
		Result      = AllChildren.First();
		bMatchFound = true;

		if (AllChildren.Num() > 1)
		{
			UE_LOG(
				LogPf2Core,
				Warning,
				TEXT("More than one child tag ('%s') matched parent tag ('%s')."),
				*(AllChildren.ToStringSimple()),
				*(ParentTag.ToString())
			);
		}
	}

	return Result;
}

uint8 UPF2TagLibrary::FindAndParseConditionLevel(const FGameplayTagContainer& AllTags, const FGameplayTag& ParentTag)
{
	uint8              Result      = 0;
	bool               bMatchFound = false;
	const FGameplayTag ChildTag    = FindChildTag(AllTags, ParentTag, bMatchFound);

	if (bMatchFound)
	{
		Result = ParseConditionLevel(ChildTag, ParentTag);
	}

	return Result;
}

uint8 UPF2TagLibrary::ParseConditionLevel(const FGameplayTag& Tag, const FGameplayTag& ParentTag)
{
	uint8 Result = 0;

	if (Tag.RequestDirectParent().MatchesTagExact(ParentTag))
	{
		// If ParentTag is "Condition.Dying" and the tag is "Condition.Dying.3", then starting at ParentTag.Len() + 1
		// in the tag should give us "3".
		const FString ParentTagString = ParentTag.ToString(),
		              SuffixString    = Tag.ToString().Mid(ParentTagString.Len() + 1);

		Result = FCString::Atoi(*SuffixString);
	}

	return Result;
}
