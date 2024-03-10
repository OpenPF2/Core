// OpenPF2 for UE Game Logic, Copyright 2023-2024, Guy Elsmore-Paddock. All Rights Reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

#pragma once

#include <IDetailCustomization.h>
#include <SSubobjectEditor.h>

#include <Input/Reply.h>

class IPF2EventEmitterInterface;

/**
 * A customization to the Details Panel of the Blueprint editor so that it supports delegates exposed by Events Objects.
 *
 * Events Objects are exposed through OpenPF2 interfaces to enable consumers of an interface to bind delegates without
 * having to be coupled to the specific type of the object implementing each interface. This customization allows the
 * delegates exposed by an Events Object to be used in a Blueprint in the same way as delegates directly on the object
 * that implements the interface (the "Event Emitter").
 *
 * @see IPF2EventEmitterInterface
 */
// ReSharper disable once CppClassCanBeFinal
class FInterfaceEventsDetailCustomization : public IDetailCustomization
{
public:
	// =================================================================================================================
	// Public Static Methods
	// =================================================================================================================
	/**
	 * Makes a new instance of this detail layout class for a specific detail view requesting it.
	 */
	static TSharedRef<IDetailCustomization> MakeInstance()
	{
		return MakeShareable(new FInterfaceEventsDetailCustomization());
	}

	/**
	 * Searches the skeleton generated class of the given blueprint for the specified property.
	 *
	 * @param BlueprintObj
	 *	The blueprint in which to search for the property.
	 * @param PropertyName
	 *	The machine name of the property to locate in the Blueprint. For example, the name of an actor component.
	 *
	 * @return
	 *	Either a pointer to the property, or nullptr if it cannot be found.
	 */
	static FORCEINLINE FObjectProperty* FindPropertyInBlueprint(const UBlueprint* BlueprintObj,
	                                                            const FName       PropertyName)
	{
		return FindFProperty<FObjectProperty>(BlueprintObj->SkeletonGeneratedClass, PropertyName);
	}

public:
	// =================================================================================================================
	// Public Methods - IDetailCustomization Implementation
	// =================================================================================================================
	virtual void CustomizeDetails(IDetailLayoutBuilder& DetailBuilder) override;

	// =================================================================================================================
	// Protected Instance Methods
	// =================================================================================================================

protected:
	/**
	 * Modifies the "Events" section of the Blueprint Details Panel to include delegates from events objects.
	 *
	 * This enables these delegates to be bound in a blueprint that contains an event emitter sub-object as if the
	 * delegates were defined on the sub-object itself.
	 *
	 * @param DetailBuilder
	 *	The Details Panel builder object.
	 * @param Blueprint
	 *	The Blueprint being edited in this instance of the Detail Panel customization.
	 * @param SelectedVariableName
	 *	The name of the variable/actor component currently selected in the "My Blueprint" section of the Blueprint
	 *	Editor.
	 * @param SelectedObject
	 *	The variable/actor component currently selected in the "My Blueprint" section of the Blueprint Editor.
	 */
	void AddEventsCategory(IDetailLayoutBuilder& DetailBuilder,
	                       UBlueprint*           Blueprint,
	                       const FName           SelectedVariableName,
	                       const UObject*        SelectedObject) const;

	/**
	 * Determines what icon to show for a button in the "Events" section of the Blueprint Details Panel.
	 *
	 * If a delegate for a particular Blueprint variable and event type already exists in the graph, the "Add" button
	 * becomes a "View" button that will take the user to the spot in the graph where the delegate node appears.
	 *
	 * This only handles buttons added by this customization. The "FBlueprintDetails" class still manages the buttons
	 * for delegates defined directly on a sub-object.
	 *
	 * @param BlueprintPtr
	 *	A weak reference to the Blueprint being edited in this instance of the Detail Panel customization.
	 * @param VariableName
	 *	The name of the variable/sub-object in the blueprint that is being edited. The value of this variable implements
	 *	the IPF2EventEmitterInterface.
	 * @param EventName
	 *	The name of the event for which a button is desired. This is the name of an event on the *events object*
	 *	returned by the event emitter, not an event defined on the emitter itself.
	 *
	 * @return
	 *	- "0" if the event does not exist in the event graph and will be added if the button is clicked.
	 *	- "1" if the event already exists in the event graph and will be brought into focus if the button is clicked.
	 */
	int32 GetIconIndexForAddOrViewButton(const TWeakObjectPtr<UBlueprint> BlueprintPtr,
	                                     const FName                      VariableName,
	                                     const FName                      EventName) const;

	/**
	 * Handles responding to the user clicking on a button in the "Events" section of the Blueprint Details Panel.
	 *
	 * By default, clicking on the button will add a new event for a particular Blueprint variable and event type.
	 * Otherwise, clicking on the button will focus the editor on the existing event, taking the user to the spot in the
	 * graph where the delegate node appears.
	 *
	 * This only handles buttons added by this customization. The "FBlueprintDetails" class still manages the buttons
	 * for delegates defined directly on a sub-object.
	 *
	 * @param BlueprintPtr
	 *	A weak reference to the Blueprint being edited in this instance of the Detail Panel customization.
	 * @param VariableName
	 *	The name of the variable/sub-object in the blueprint that is being edited. The value of this variable implements
	 *	the IPF2EventEmitterInterface.
	 * @param VariableClass
	 *	The class/type of the variable.
	 * @param EventName
	 *	The name of the event for which a button is desired. This is the name of an event on the *events object*
	 *	returned by the event emitter, not an event defined on the emitter itself.
	 *
	 * @return
	 *	FReply::Handled(), always.
	 */
	FReply OnAddOrViewButtonClicked(const TWeakObjectPtr<UBlueprint> BlueprintPtr,
	                                const FName                      VariableName,
	                                const TWeakObjectPtr<UClass>     VariableClass,
	                                const FName                      EventName) const;
};
