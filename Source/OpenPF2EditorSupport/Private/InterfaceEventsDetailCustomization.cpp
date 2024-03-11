// OpenPF2 for UE Game Logic, Copyright 2023-2024, Guy Elsmore-Paddock. All Rights Reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

#include "InterfaceEventsDetailCustomization.h"

#include <DetailCategoryBuilder.h>
#include <DetailLayoutBuilder.h>
#include <DetailWidgetRow.h>

#include <Engine/Blueprint.h>

#include <Kismet2/BlueprintEditorUtils.h>
#include <Kismet2/KismetEditorUtilities.h>

#include <Widgets/Input/SButton.h>
#include <Widgets/Layout/SWidgetSwitcher.h>

#include "PF2EventEmitterInterface.h"
#include "PF2K2Node_EventEmitterBoundEvent.h"

#include "Utilities/PF2BlueprintEditorUtilities.h"

#define LOCTEXT_NAMESPACE "InterfaceEventsDetailCustomization"

void FInterfaceEventsDetailCustomization::CustomizeDetails(IDetailLayoutBuilder& DetailBuilder)
{
	TArray<TWeakObjectPtr<UObject>> SelectedObjects;

	SelectedObjects = DetailBuilder.GetSelectedObjects();

	if ((SelectedObjects.Num() == 1) && SelectedObjects[0].IsValid())
	{
		const UObject* SelectedObject      = SelectedObjects[0].Get();
		UBlueprint*    ContainingBlueprint = PF2BlueprintEditorUtilities::GetBlueprintContainingObject(SelectedObject);

		const FSubobjectEditorTreeNode* SelectedTreeNode =
			PF2BlueprintEditorUtilities::GetSelectedTreeNodeInBlueprint(ContainingBlueprint);

		if ((ContainingBlueprint != nullptr) && (SelectedTreeNode != nullptr))
		{
			this->AddEventsCategory(
				DetailBuilder,
				ContainingBlueprint,
				SelectedTreeNode->GetVariableName(),
				SelectedObject
			);
		}
	}
}

void FInterfaceEventsDetailCustomization::AddEventsCategory(IDetailLayoutBuilder& DetailBuilder,
                                                            UBlueprint*           Blueprint,
                                                            const FName           SelectedVariableName,
                                                            const UObject*        SelectedObject) const
{
	if ((SelectedObject != nullptr) && (Blueprint != nullptr) && Blueprint->AllowsDynamicBinding())
	{
		// If the object property can't be resolved for the property or it is missing a valid events object, we can't
		// use its events.
		const FObjectProperty* VariableProperty = FindPropertyInBlueprint(Blueprint, SelectedVariableName);

		if (VariableProperty != nullptr)
		{
			UClass*       SelectedObjectClass = SelectedObject->GetClass();
			const UClass* EventsObjectClass   = IPF2EventEmitterInterface::GetEventsClassOfObject(SelectedObject);

			if ((SelectedObjectClass != nullptr) && (EventsObjectClass != nullptr) &&
			    FBlueprintEditorUtils::CanClassGenerateEvents(EventsObjectClass))
			{
				for (TFieldIterator<FMulticastDelegateProperty> PropertyIt(EventsObjectClass, EFieldIteratorFlags::IncludeSuper); PropertyIt; ++PropertyIt)
				{
					const FMulticastDelegateProperty* Property = *PropertyIt;
					static const FName                HideInDetailPanelName("HideInDetailPanel");

					// Check for multicast delegates that we can safely assign.
					if (!Property->HasAnyPropertyFlags(CPF_Parm) && Property->HasAllPropertyFlags(CPF_BlueprintAssignable) &&
					    !Property->HasMetaData(HideInDetailPanelName))
					{
						const FName EventName = Property->GetFName();
						const FText EventText = Property->GetDisplayNameText();

						IDetailCategoryBuilder& EventCategory =
							DetailBuilder.EditCategory(
								TEXT("Events"),
								LOCTEXT("Events", "Events"),
								ECategoryPriority::Uncommon
							);

						EventCategory
							.AddCustomRow(EventText)
							.WholeRowContent()
							[
								SNew(SHorizontalBox)
								.ToolTipText(Property->GetToolTipText())

								+ SHorizontalBox::Slot()
								.AutoWidth()
								.VAlign(VAlign_Center)
								.Padding(0.0f, 0.0f, 5.0f, 0.0f)
								[
									SNew(SImage)
									.Image(FAppStyle::Get().GetBrush("GraphEditor.Event_16x"))
								]

								+ SHorizontalBox::Slot()
								.VAlign(VAlign_Center)
								[
									SNew(STextBlock)
									.Font(IDetailLayoutBuilder::GetDetailFont())
									.Text(EventText)
								]

								+ SHorizontalBox::Slot()
								.HAlign(HAlign_Left)
								.VAlign(VAlign_Center)
								.Padding(0.0f)
								[
									SNew(SButton)
									.ContentPadding(FMargin(3.0, 2.0))
									.OnClicked(
										this,
										&FInterfaceEventsDetailCustomization::OnAddOrViewButtonClicked,
										MakeWeakObjectPtr(Blueprint),
										SelectedVariableName,
										MakeWeakObjectPtr(SelectedObjectClass),
										EventName)
									[
										SNew(SWidgetSwitcher)
										.WidgetIndex(
											this,
											&FInterfaceEventsDetailCustomization::GetIconIndexForAddOrViewButton,
											MakeWeakObjectPtr(Blueprint),
											SelectedVariableName,
											EventName)

										+ SWidgetSwitcher::Slot()
										[
											SNew(SImage)
											.ColorAndOpacity(FSlateColor::UseForeground())
											.Image(FAppStyle::Get().GetBrush("Icons.SelectInViewport"))
										]

										+ SWidgetSwitcher::Slot()
										[
											SNew(SImage)
											.ColorAndOpacity(FSlateColor::UseForeground())
											.Image(FAppStyle::Get().GetBrush("Icons.Plus"))
										]
									]
								]
							];
					}
				}
			}
		}
	}
}

// ReSharper disable once CppMemberFunctionMayBeStatic
int32 FInterfaceEventsDetailCustomization::GetIconIndexForAddOrViewButton(const TWeakObjectPtr<UBlueprint> BlueprintPtr,
                                                                          const FName VariableName,
                                                                          const FName EventName) const
{
	int32             IconIndex;
	const UBlueprint* Blueprint = BlueprintPtr.Get();

	if (Blueprint == nullptr)
	{
		// Blueprint has been garbage collected; default to not allowing add.
		IconIndex = 0;
	}
	else if (UPF2K2Node_EventEmitterBoundEvent::FindExisting(Blueprint, VariableName, EventName) == nullptr)
	{
		// Does not exist; will need to add.
		IconIndex = 1;
	}
	else
	{
		// Already exists; will navigate to where it appears.
		IconIndex = 0;
	}

	return IconIndex;
}

// ReSharper disable once CppMemberFunctionMayBeStatic
FReply FInterfaceEventsDetailCustomization::OnAddOrViewButtonClicked(const TWeakObjectPtr<UBlueprint> BlueprintPtr,
                                                                     const FName                      VariableName,
                                                                     const TWeakObjectPtr<UClass>     VariableClass,
                                                                     const FName                      EventName) const
{
	const UBlueprint* Blueprint = BlueprintPtr.Get();

	if (Blueprint != nullptr)
	{
		FObjectProperty* VariableProperty = FindPropertyInBlueprint(Blueprint, VariableName);

		if (VariableProperty != nullptr)
		{
			const UPF2K2Node_EventEmitterBoundEvent* ExistingNode =
				UPF2K2Node_EventEmitterBoundEvent::FindExisting(Blueprint, VariableProperty->GetFName(), EventName);

			if (ExistingNode == nullptr)
			{
				// Does not exist; let's add it!
				UPF2K2Node_EventEmitterBoundEvent::CreateNew(
					Blueprint,
					VariableProperty,
					VariableClass.Get(),
					EventName);
			}
			else
			{
				// Already exists; bring it into view.
				FKismetEditorUtilities::BringKismetToFocusAttentionOnObject(ExistingNode);
			}
		}
	}

	return FReply::Handled();
}

#undef LOCTEXT_NAMESPACE
