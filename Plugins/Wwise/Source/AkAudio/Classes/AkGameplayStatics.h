/*******************************************************************************
The content of this file includes portions of the proprietary AUDIOKINETIC Wwise
Technology released in source code form as part of the game integration package.
The content of this file may not be used without valid licenses to the
AUDIOKINETIC Wwise Technology.
Note that the use of the game engine is subject to the Unreal(R) Engine End User
License Agreement at https://www.unrealengine.com/en-US/eula/unreal
 
License Usage
 
Licensees holding valid licenses to the AUDIOKINETIC Wwise Technology may use
this file in accordance with the end user license agreement provided with the
software or, alternatively, in accordance with the terms contained
in a written agreement between you and Audiokinetic Inc.
Copyright (c) 2024 Audiokinetic Inc.
*******************************************************************************/

/*=============================================================================
	AkGameplayStatics.h:
=============================================================================*/
#pragma once

#include "AkAudioDevice.h"
#include "AkInclude.h"
#include "AkGameplayTypes.h"
#include "WwiseUnrealDefines.h"
#include "WwiseUnrealObjectHelper.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "AkGameplayStatics.generated.h"

// PostEvent functions need to return the PlayingID (uint32), but Blueprints only work with int32.
// Make sure AkPlayingID is always 32 bits, or else we're gonna have a bad time.
static_assert(sizeof(AkPlayingID) == sizeof(int32), "AkPlayingID is not 32 bits anymore. Change return value of PostEvent functions and callback info structures members!");

UCLASS()
class AKAUDIO_API UAkGameplayStatics : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UAkGameplayStatics(const class FObjectInitializer& ObjectInitializer);

	/** Get an AkComponent attached to and following the specified component. 
 * @param AttachToComponent - The parent component to search
 * @param ComponentCreated - True if a new component was created
 * @param AttachPointName - Optional named point within the AttachComponent to play the sound at.
 * @param Location Position the Component is situated at
 * @param LocationType KeepRelativeOffset for position relative to AttachToComponent, KeepWorldPosition for global
 */
UFUNCTION(BlueprintCallable, Category="Audiokinetic|AkComponent",
	meta=(DeprecatedFunction, DeprecationMessage="Please use GetOrCreateAkComponent instead."))
	static class UAkComponent* GetAkComponent(USceneComponent* AttachToComponent, bool& ComponentCreated,
		FName AttachPointName = NAME_None, FVector Location = FVector(ForceInit), EAttachLocation::Type LocationType =
			EAttachLocation::KeepRelativeOffset);


	/** Get an AkComponent attached to and following the specified component. 
	 * @param AttachToComponent - The parent component to search
	 * @param ComponentCreated - True if a new component was created
	 * @param AttachPointName - Optional named point within the AttachComponent to play the sound at.
	 */
	UFUNCTION(BlueprintCallable, Category="Audiokinetic|AkComponent")
	static class UAkComponent* GetOrCreateAkComponent(class USceneComponent* AttachToComponent, bool& ComponentCreated,
		FName AttachPointName = NAME_None);

	UFUNCTION(BlueprintCallable, Category="Audiokinetic")
	static bool IsEditor();

	UFUNCTION(BlueprintCallable, Category = "Audiokinetic")
	static bool IsGame(UObject* WorldContextObject);

	/** Posts a Wwise Event attached to and following the root component of the specified actor.
	 *
	 * @param AkEvent - Event to play.
	 * @param Actor - Actor on which to play the event. If the Actor is left empty, the Event will be played as an Ambient sound.
	 * @param bStopWhenAttachedToDestroyed - Specifies whether the sound should stop playing when the owner of the attach to component is destroyed.
	 * @param EventName - Deprecated: Event name in case the AkEvent is not set.
	 */
	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category="Audiokinetic|Actor", meta=(AdvancedDisplay="2", AutoCreateRefTerm = "PostEventCallback"))
	static int32 PostEvent(	class UAkAudioEvent* AkEvent, 
							class AActor* Actor, 
							UPARAM(meta = (Bitmask, BitmaskEnum = "/Script/AkAudio.EAkCallbackType")) int32 CallbackMask,
							const FOnAkPostEventCallback& PostEventCallback,
							bool bStopWhenAttachedToDestroyed = false,
							FString EventName = FString(""));

	/**
	 * Posts a Wwise Event attached to and following the root component of the specified actor, and waits for the end of the event to continue execution.
	 * Additional calls made while an event is active are ignored.
	 *
	 * @param AkEvent - Event to play.
	 * @param Actor - actor on which to play the event.
	 * @param bStopWhenAttachedToDestroyed - Specifies whether the sound should stop playing when the owner of the attach to component is destroyed.
	 */
	UFUNCTION(BlueprintCallable, Category = "Audiokinetic|Actor", meta = (Latent, LatentInfo = "LatentInfo", AdvancedDisplay = "2"))
	static int32 PostAndWaitForEndOfEvent(class UAkAudioEvent* AkEvent,
		class AActor* Actor,
		bool bStopWhenAttachedToDestroyed,
		FLatentActionInfo LatentInfo);

	/** Posts a Wwise Event attached and following the root component of the specified actor, wait for the media to be loaded and waits for the end of the event to continue execution.
	 * Additional calls made while an event is active are ignored.
	 *
	 * @warning This function is deprecated. Use \ref PostAndWaitForEndOfEvent or \ref UAkAudioEvent::PostOnActorAndWait.
	 *			Async operations are deprecated.
	 *
	 * @param AkEvent - ak event to play.
	 * @param Actor - actor on which to play the event.
	 * @param bStopWhenAttachedToDestroyed - Specifies whether the sound should stop playing when the owner of the attach to component is destroyed.
	 */
	UFUNCTION(BlueprintCallable, Category = "Audiokinetic|Actor", meta = (Latent, LatentInfo = "LatentInfo", AdvancedDisplay = "3", bStopWhenAttachedToDestroyed = "false", AutoCreateRefTerm = "ExternalSources", DeprecatedFunction, DeprecationMessage = "Use \"UAkAudioEvent::PostOnActorAndWait\"."))
	static void PostAndWaitForEndOfEventAsync(class UAkAudioEvent* AkEvent,
			class AActor* Actor,
			int32& PlayingID,
			bool bStopWhenAttachedToDestroyed,
			FLatentActionInfo LatentInfo
			);

	/** Posts a Wwise Event by name attached to and following the root component of the specified actor.
	 *
	 * @warning This function is deprecated. You are expected to use an UAkAudioEvent. Please see \ref PostEvent or \ref UAkAudioEvent::PostOnActor.
	 *
	 * @param AkEvent - ak event to play.
	 * @param Actor - actor on which to play the event.
	 * @param bStopWhenAttachedToDestroyed - Specifies whether the sound should stop playing when the owner of the attach to component is destroyed.
	 */
	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category="Audiokinetic|Actor", meta=(DeprecatedFunction, DeprecationMessage = "Use the \"Event Name\" field of PostEvent"))
	static void PostEventByName(	const FString& EventName, 
									class AActor* Actor, 
									bool bStopWhenAttachedToDestroyed = false);

	/** Posts a Wwise Event at the specified location. This is a fire and forget sound, created on a temporary Wwise Game Object. Replication is also not handled at this point.
	 *
	 * @param AkEvent - Wwise Event to post.
	 * @param Location - Location from which to post the Wwise Event.
	 * @param Orientation - Orientation of the event.
	 * @param EventName - Deprecated: Event name in case the AkEvent is not set.
	 */
	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category="Audiokinetic", meta=(WorldContext="WorldContextObject", AdvancedDisplay = "3"))
	static int32 PostEventAtLocation(UAkAudioEvent* AkEvent, FVector Location, FRotator Orientation,
	                                 const FString& EventName, UObject* WorldContextObject);

	/** Posts a Wwise Event by name at the specified location. This is a fire and forget sound, created on a temporary Wwise Game Object. Replication is also not handled at this point.
	 *
	 * @warning This function is deprecated. You are expected to use an UAkAudioEvent. Please see \ref UAkAudioEvent::PostAtLocation.
	 *
	 * @param AkEvent - Wwise Event to post.
	 * @param Location - Location from which to post the Wwise Event.
	 * @param Orientation - Orientation of the event.
	 */
	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category="Audiokinetic", meta=(WorldContext="WorldContextObject", DeprecatedFunction, DeprecationMessage = "Use \"UAkAudioEvent::PostAtLocation\"."))
	static void PostEventAtLocationByName(const FString& EventName, FVector Location, FRotator Orientation, UObject* WorldContextObject );

	/** Execute action on event attached to and following the root component of the specified actor
	 *
	 * @warning This function is deprecated. Please see \ref UAkAudioEvent::ExecuteAction.
	 *
	 * @param AkEvent - Wwise Event to act upon.
	 * @param ActionType - Which action to do.
	 * @param Actor - Which actor to use.
	 * @param TransitionDuration - Transition duration in milliseconds.
	 * @param FadeCurve - The interpolation curve of the transition.
	 * @param PlayingID - Use the return value of a Post Event to act only on this specific instance of an event.
	 */
	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category = "Audiokinetic|Actor", meta=(DeprecatedFunction, DeprecationMessage = "Use \"UAkAudioEvent::ExecuteAction\"."))
	static void ExecuteActionOnEvent(class UAkAudioEvent* AkEvent, AkActionOnEventType ActionType, class AActor* Actor, int32 TransitionDuration = 0, EAkCurveInterpolation FadeCurve = EAkCurveInterpolation::Linear, int32 PlayingID = 0);

	/** Execute action on specific playing ID
	 *
	 * @warning This function is deprecated. You are expected to use an UAkAudioEvent. Please see \ref UAkAudioEvent::ExecuteAction.
	 *
	 * @param ActionType - Which action to do.
	 * @param PlayingID - Use the return value of a Post Event to act only on this specific instance of an event.
	 * @param TransitionDuration - Transition duration in milliseconds.
	 * @param FadeCurve - The interpolation curve of the transition.
	 */
	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category = "Audiokinetic|Actor", meta=(DeprecatedFunction, DeprecationMessage = "Use \"UAkAudioEvent::ExecuteAction\"."))
	static void ExecuteActionOnPlayingID(AkActionOnEventType ActionType, int32 PlayingID, int32 TransitionDuration = 0, EAkCurveInterpolation FadeCurve = EAkCurveInterpolation::Linear);

	/** Spawn an AkComponent at a location. Allows, for example, to set a switch on a fire and forget sound.
	 * @param AkEvent - Wwise Event to post.
	 * @param Location - Location from which to post the Wwise Event.
	 * @param Orientation - Orientation of the event.
	 * @param AutoPost - Automatically post the event once the AkComponent is created.
	 * @param AutoDestroy - Automatically destroy the AkComponent once the event is finished.
	 */
	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category="Audiokinetic|AkComponent", meta=(WorldContext="WorldContextObject", AdvancedDisplay = "6"))
	static class UAkComponent* SpawnAkComponentAtLocation(UObject* WorldContextObject, class UAkAudioEvent* AkEvent, FVector Location, FRotator Orientation, bool AutoPost, const FString& EventName, bool AutoDestroy = true);

	/**
	* Sets the value of a Game Parameter, optionally targetting the root component of a specified actor.
	* @param RTPC - The name of the Game Parameter to set
	* @param Value - The value of the Game Parameter
	* @param InterpolationTimeMs - Duration during which the Game Parameter is interpolated towards Value (in ms)
	* @param Actor - (Optional) Actor on which to set the Game Parameter value
	*/
	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category = "Audiokinetic", meta = (AdvancedDisplay = "4"))
	static void SetRTPCValue(class UAkRtpc const* RTPCValue, float Value, int32 InterpolationTimeMs, class AActor* Actor, FName RTPC);

	/**
	* Gets the value of a Game Parameter, optionally targetting the root component of a specified actor.
	* @param RTPC - The name of the Game Parameter to set
	* @param Value - The value of the Game Parameter
	* @param InterpolationTimeMs - Duration during which the Game Parameter is interpolated towards Value (in ms)
	* @param Actor - (Optional) Actor on which to set the Game Parameter value
	*/
	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category = "Audiokinetic", meta = (AdvancedDisplay = "7"))
	static void GetRTPCValue(class UAkRtpc const* RTPCValue, int32 PlayingID, ERTPCValueType InputValueType, float& Value, ERTPCValueType& OutputValueType, class AActor* Actor, FName RTPC);

	/**
	* Resets the value of a Game Parameter to its default value, optionally targetting the root component of a specified actor.
	* @param RTPCValue - The name of the Game Parameter to reset
	* @param InterpolationTimeMs - Duration during which the Game Parameter is interpolated towards its default value (in ms)
	* @param Actor - (Optional) Actor on which to reset the Game Parameter value
	* @param RTPC - The name of the Game Parameter to reset
	*/
	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category = "Audiokinetic", meta = (AdvancedDisplay = "8"))
	static void ResetRTPCValue(class UAkRtpc const* RTPCValue, int32 InterpolationTimeMs, class AActor* Actor, FName RTPC);

	/**
	 * Set the active State for a given State Group.
	 * @param StateGroup - Name of the State Group to be modified
	 * @param State - Name of the State to be made active
	 */
	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category="Audiokinetic", meta = (AdvancedDisplay = "1"))
	static void SetState(class UAkStateValue const* StateValue, FName StateGroup, FName State);

	/**
	 * Posts a Trigger, targetting the root component of a specified actor.
	 * @param Trigger - Name of the Trigger
	 * @param Actor - Actor on which to post the Trigger
	 */
	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category="Audiokinetic|Actor", meta = (AdvancedDisplay = "2"))
	static void PostTrigger(class UAkTrigger const* TriggerValue, class AActor* Actor, FName Trigger);
	
	/**
	 * Sets the active Switch for a given Switch Group, targetting the root component of a specified actor.
	 * @param SwitchGroup - Name of the Switch Group to be modified
	 * @param SwitchState - Name of the Switch to be made active
	 * @param Actor - Actor on which to set the switch
	 */
	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category="Audiokinetic|Actor", meta = (AdvancedDisplay = "2"))
	static void SetSwitch(class UAkSwitchValue const* SwitchValue, class AActor* Actor, FName SwitchGroup, FName SwitchState);

    /** Sets multiple positions to a single game object.
    *  Setting multiple positions on a single game object is a way to simulate multiple emission sources while using the resources of only one voice.
    *  This can be used to simulate wall openings, area sounds, or multiple objects emitting the same sound in the same area.
    *  Note: Calling SetMultiplePositions() with only one position is the same as calling SetPosition()
    *  @param GameObjectAkComponent AkComponent of the game object on which to set positions.
    *  @param Positions Array of transforms to apply.
    *  @param MultiPositionType Position type
    *  @return AK_Success when successful, AK_InvalidParameter if parameters are not valid.
    *
    */
    UFUNCTION(BlueprintCallable, Category = "Audiokinetic")
    static void SetMultiplePositions(UAkComponent* GameObjectAkComponent, TArray<FTransform> Positions,
                                     AkMultiPositionType MultiPositionType = AkMultiPositionType::MultiDirections);

    /** Sets multiple positions to a single game object, with flexible assignment of input channels.
    *  Setting multiple positions on a single game object is a way to simulate multiple emission sources while using the resources of only one voice.
    *  This can be used to simulate wall openings, area sounds, or multiple objects emitting the same sound in the same area.
    *  Note: Calling AK::SoundEngine::SetMultiplePositions() with only one position is the same as calling AK::SoundEngine::SetPosition()
    *  @param GameObjectAkComponent AkComponent of the game object on which to set positions.
    *  @param ChannelMasks Array of channel configuration to apply for each position.
    *  @param Positions Array of transforms to apply.
    *  @param MultiPositionType Position type
    *  @return AK_Success when successful, AK_InvalidParameter if parameters are not valid.
    */
    UFUNCTION(BlueprintCallable, Category = "Audiokinetic")
    static void SetMultipleChannelEmitterPositions(UAkComponent* GameObjectAkComponent,
			TArray<AkChannelConfiguration> ChannelMasks,
			TArray<FTransform> Positions,
			AkMultiPositionType MultiPositionType = AkMultiPositionType::MultiDirections
	);

	/** Sets multiple positions to a single game object, with flexible assignment of input channels.
	*  Setting multiple positions on a single game object is a way to simulate multiple emission sources while using the resources of only one voice.
	*  This can be used to simulate wall openings, area sounds, or multiple objects emitting the same sound in the same area.
	*  Note: Calling AK::SoundEngine::SetMultiplePositions() with only one position is the same as calling AK::SoundEngine::SetPosition()
	*  @param GameObjectAkComponent AkComponent of the game object on which to set positions.
	*  @param ChannelMasks Array of channel mask to apply for each position.
	*  @param Positions Array of transforms to apply.
	*  @param MultiPositionType Position type
	*  @return AK_Success when successful, AK_InvalidParameter if parameters are not valid.
	*/
	UFUNCTION(BlueprintCallable, Category = "Audiokinetic")
	static void SetMultipleChannelMaskEmitterPositions(UAkComponent* GameObjectAkComponent,
			TArray<FAkChannelMask> ChannelMasks,
			TArray<FTransform> Positions,
			AkMultiPositionType MultiPositionType = AkMultiPositionType::MultiDirections
	);

	/**
	* Sets UseReverbVolumes flag on a specified actor. Set value to true to use reverb volumes on this component.
	*
	* @param inUseReverbVolumes - Whether to use reverb volumes or not.
	* @param Actor - Actor on which to set the flag
	*/
	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category="Audiokinetic|Actor")
	static void UseReverbVolumes(bool inUseReverbVolumes, class AActor* Actor);

	UFUNCTION(BlueprintCallable, Category = "Audiokinetic|Actor", meta = (AdvancedDisplay = "6", DeprecatedFunction, DeprecationMessage = "This function is deprecated and will be removed in future releases."))
		static void UseEarlyReflections(class AActor* Actor,
			class UAkAuxBus* AuxBus,
			int Order = 1,
			float BusSendGain = 1.f,
			float MaxPathLength = 100000.f,
			bool SpotReflectors = false,
			const FString& AuxBusName = FString(""));

	/**
	* Sets the Reflections Order for Spatial Audio Reflect.
	*
	* @param Order - The order of Reflection. Can be 0 to 4.
	* @param RefreshPaths - whether the paths should be refreshed immediately.
	*/
	UFUNCTION(BlueprintCallable, Category = "Audiokinetic|Spatial Audio")
	static void SetReflectionsOrder(int Order, bool RefreshPaths);

	/**
	* Sets the obstruction and occlusion value of sounds going through this portal.
	*
	* @param PortalComponent - The portal through which sound path need to pass to get obstructed and occluded.
	* @param ObstructionValue - The obstruction value. Can be 0 to 1.
	* @param OcclusionValue - The occlusion value. Can be 0 to 1.
	*/
	UFUNCTION(BlueprintCallable, Category = "Audiokinetic|Spatial Audio")
	static void SetPortalObstructionAndOcclusion(UAkPortalComponent* PortalComponent, float ObstructionValue, float OcclusionValue);

	/**
	* Sets the obstruction value of sounds going from this game object through this portal.
	*
	* @param GameObjectAkComponent - The game object emitting the sound that we want to obstruct.
	* @param PortalComponent - The portal through which the sound from the game object can go.
	* @param OcclusionValue - The occlusion value. Can be 0 to 1.
	*/
	UFUNCTION(BlueprintCallable, Category = "Audiokinetic|Spatial Audio")
	static void SetGameObjectToPortalObstruction(UAkComponent* GameObjectAkComponent, UAkPortalComponent* PortalComponent, float ObstructionValue);

	/**
	* Sets the obstruction value of sounds going from a first portal through the next portal.
	*
	* @param PortalComponent0 - The first portal through which a sound path goes.
	* @param PortalComponent1 - The next portal throuh which the sound path goes from the first portal.
	* @param OcclusionValue - The occlusion value. Can be 0 to 1.
	*/
	UFUNCTION(BlueprintCallable, Category = "Audiokinetic|Spatial Audio")
	static void SetPortalToPortalObstruction(UAkPortalComponent* PortalComponent0, UAkPortalComponent* PortalComponent1, float ObstructionValue);

	/**
	* Set the output bus volume (direct) to be used for the specified game object.
	* The control value is a number ranging from 0.0f to 1.0f.
	*
	* @param BusVolume - Bus volume to set
	* @param Actor - Actor on which to set the flag
	*/
	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category = "Audiokinetic|Actor")
	static void SetOutputBusVolume(float BusVolume, class AActor* Actor);

	/**
	* Force channel configuration for the specified bus.
	* This function has unspecified behavior when changing the configuration of a bus that
	* is currently playing.
	* You cannot change the configuration of the master bus.
	*
	* @param BusName				Bus Name
	* @param ChannelConfiguration	Desired channel configuration.
	* @return Always returns AK_Success
	*/
	UFUNCTION(BlueprintCallable, Category = "Audiokinetic")
	static void SetBusConfig(const FString& BusName, AkChannelConfiguration ChannelConfiguration);

	/**
	*  Set the panning rule of the specified output.
	*  This may be changed anytime once the sound engine is initialized.
	*  @warning This function posts a message through the sound engine's internal message queue, whereas GetPanningRule() queries the current panning rule directly.
	*
	* @param PanRule	Panning rule.
	*/
	UFUNCTION(BlueprintCallable, Category = "Audiokinetic")
	static void SetPanningRule(PanningRule PanRule);
	
	/**
	* Adds an output to the sound engine. Use this to add controller-attached headphones, controller speakers, DVR output, etc.
	* The in_Settings parameter contains an Audio Device ShareSet to specify the output plugin to use and a device ID to specify the instance, if necessary (e.g. which game controller).
	*
	* Like most functions of AK::SoundEngine, AddOutput is asynchronous.
	* A successful return code merely indicates that the request is properly queued. Error codes returned by this function indicate various invalid parameters.
	* To know if this function succeeds or not, and the failure code, register an AkDeviceStatusCallbackFunc callback with RegisterAudioDeviceStatusCallback.
	*
	* @param in_Settings Creation parameters for this output.
	* @param out_pDeviceID (Optional) Output ID to use with all other Output management functions. Leave to NULL if not required.
	* @param in_pListenerIDs Specific listener(s) to attach to this device. If specified, only the sounds routed to game objects linked to those listeners will play in this device. It is necessary to have separate listeners if multiple devices of the same type can coexist (e.g. controller speakers) If not specified, sound routing simply obey the associations between Master Busses and Audio Devices setup in the Wwise Project.
	*/
	UFUNCTION(BlueprintCallable, meta = (AutoCreateRefTerm = "in_ListenerIDs"), Category = "Audiokinetic")
	static void AddOutput(const FAkOutputSettings& in_Settings, FAkOutputDeviceID& out_DeviceID, UPARAM(ref) TArray<UAkComponent*>& in_ListenerIDs);

	/**
	* Removes one output added through AK::SoundEngine::AddOutput If a listener was associated with the device, you should consider unregistering the listener prior to call RemoveOutput so that Game Object/Listener routing is properly updated according to your game scenario.
	*
	* @param in_OutputDeviceId ID of the output to remove. Use the returned ID from AddOutput, GetOutputID, or ReplaceOutput.
	*/
	UFUNCTION(BlueprintCallable, Category = "Audiokinetic")
	static void RemoveOutput(FAkOutputDeviceID in_OutputDeviceId);

	/**
	 * Replaces the main output device previously created during engine initialization with a new output device.
	 * In addition to simply removing one output device and adding a new one, the new output device will also be used on all of the master busses
	 * that the old output device was associated with, and preserve all listeners that were attached to the old output device.
	 *
	 * @param MainOutputSettings	Creation parameters for this output
	 */
	UFUNCTION(BlueprintCallable, Category = "Audiokinetic")
	static void ReplaceMainOutput(const FAkOutputSettings& MainOutputSettings);

	/**
	 * Gets speaker angles of the specified device. Speaker angles are used for 3D positioning of sounds over standard configurations.
	 * Note that the current version of Wwise only supports positioning on the plane.
	 * The speaker angles are expressed as an array of loudspeaker pairs, in degrees, relative to azimuth ]0,180].
	 * Supported loudspeaker setups are always symmetric; the center speaker is always in the middle and thus not specified by angles.
	 * Angles must be set in ascending order.
	 *
	 * @param SpeakerAngles Returned array of loudspeaker pair angles, in degrees relative to azimuth [0,180].
	 * @param HeightAngle Elevation of the height layer, in degrees relative to the plane [-90,90].
	 * @param DeviceShareSet ShareSet for which to get the angles.
	 *
	 */
	UFUNCTION(BlueprintCallable, Category = "Audiokinetic")
	static void GetSpeakerAngles(TArray<float>& SpeakerAngles, float& HeightAngle, const FString& DeviceShareSet = "");

	/**
	 * Sets speaker angles of the specified device. Speaker angles are used for 3D positioning of sounds over standard configurations.
	 * Note that the current version of Wwise only supports positioning on the plane.
	 * The speaker angles are expressed as an array of loudspeaker pairs, in degrees, relative to azimuth ]0,180].
	 * Supported loudspeaker setups are always symmetric; the center speaker is always in the middle and thus not specified by angles.
	 * Angles must be set in ascending order.
	 *
	 * @param SpeakerAngles Array of loudspeaker pair angles, in degrees relative to azimuth [0,180]
	 * @param HeightAngle Elevation of the height layer, in degrees relative to the plane [-90,90]
	 * @param DeviceShareSet ShareSet for which to set the angles on.
	 */
	UFUNCTION(BlueprintCallable, Category = "Audiokinetic")
	static void SetSpeakerAngles(const TArray<float>& SpeakerAngles, float HeightAngle, const FString& DeviceShareSet = "");

	/**
	 * Sets the occlusion calculation refresh interval, targetting the root component of a specified actor.
	 * @param RefreshInterval - Value of the wanted refresh interval
	 * @param Actor - Actor on which to set the refresh interval
	 */
	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category="Audiokinetic|Actor")
	static void SetOcclusionRefreshInterval(float RefreshInterval, class AActor* Actor );

	/**
	 * Stop all sounds for an actor.
	 */
	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category="Audiokinetic|Actor")
	static void StopActor(class AActor* Actor);

	/**
	 * Stop all sounds.
	 */
	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category="Audiokinetic")
	static void StopAll();

	/**
	 * Cancels an Event callback
	 */
	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category = "Audiokinetic")
	static void CancelEventCallback(const FOnAkPostEventCallback& PostEventCallback);

	/**
	 * Start all Ak ambient sounds.
	 */
	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category="Audiokinetic|AkAmbientSound", meta=(WorldContext = "WorldContextObject"))
	static void StartAllAmbientSounds(UObject* WorldContextObject);
	
	/**
	 * Stop all Ak ambient sounds.
	 */
	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category="Audiokinetic|AkAmbientSound", meta=(WorldContext = "WorldContextObject"))
	static void StopAllAmbientSounds(UObject* WorldContextObject);
	
	/**
	 * Clear all loaded banks
	 */
	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category = "Audiokinetic|SoundBanks", meta = (DeprecatedFunction, DeprecationMessage = "Use the \"ClearSoundBanksAndMedia\" instead."))
	static void ClearBanks();

	/**
	 * Clear all loaded banks
	 */
	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category = "Audiokinetic|SoundBanks")
	static void ClearSoundBanksAndMedia();

	/**
	 * Loads the Init SoundBank
	 */
	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category = "Audiokinetic|SoundBanks")
	static void LoadInitBank();

	/**
	 * Unloads the Init SoundBank
	 */
	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category = "Audiokinetic|SoundBanks")
	static void UnloadInitBank();

	/**
	 * Loads a bank by its name.
	 * @param Bank - The bank to load.
	 */
	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category = "Audiokinetic|SoundBanks", meta = (DeprecatedFunction, DeprecationMessage = "To manually load and unload the bank and media resources for your Wwise Assets, use the Wwise Asset LoadData and UnloadData Blueprint functions."))
	static void LoadBankByName(const FString& BankName);

	/**
	 * Unloads a bank by its name.
	 * @param Bank - The bank to unload.
	 */
	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category = "Audiokinetic|SoundBanks", meta = (DeprecatedFunction, DeprecationMessage = "To manually load and unload the bank and media resources for your Wwise Assets, use the Wwise Asset LoadData and UnloadData Blueprint functions."))
	static void UnloadBankByName(const FString& BankName);

	/**
	 * Starts a Wwise output capture. The output file will be located in the same folder as the SoundBanks.
	 * @param Filename - The name to give to the output file.
	 */
	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category="Audiokinetic|Debug")
	static void StartOutputCapture(const FString& Filename);

	/**
	 * Add text marker in output capture file.
	 * @param MarkerText - The name text to put in the marker.
	 */
	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category="Audiokinetic|Debug")
	static void AddOutputCaptureMarker(const FString& MarkerText);

	/**
	 * Stops a Wwise output capture. The output file will be located in the same folder as the SoundBanks.
	 */
	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category="Audiokinetic|Debug")
	static void StopOutputCapture();
	
	/**
	 * Starts a Wwise profiler capture. The output file will be located in the same folder as the SoundBanks.
	 * @param Filename - The name to give to the output file.
	 */
	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category="Audiokinetic|Debug")
	static void StartProfilerCapture(const FString& Filename);

	/**
	 * Stops a Wwise profiler capture. The output file will be located in the same folder as the SoundBanks.
	 */
	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category="Audiokinetic|Debug")
	static void StopProfilerCapture();

	UFUNCTION(BlueprintCallable, Category = "Audiokinetic|Culture")
	static FString GetCurrentAudioCulture();

	UFUNCTION(BlueprintCallable, Category = "Audiokinetic|Culture")
	static TArray<FString> GetAvailableAudioCultures();

	UFUNCTION(BlueprintCallable, Category = "Audiokinetic|Culture", meta = (WorldContext = "WorldContextObject", Latent, LatentInfo = "LatentInfo"))
	static void SetCurrentAudioCulture(const FString& AudioCulture, FLatentActionInfo LatentInfo, UObject* WorldContextObject);

	UFUNCTION(BlueprintCallable, Category = "Audiokinetic|Culture")
	static void SetCurrentAudioCultureAsync(const FString& AudioCulture, const FOnSetCurrentAudioCultureCallback& Completed);

	UFUNCTION(BlueprintCallable, Category = "Audiokinetic")
	static UObject* GetAkAudioTypeUserData(const UAkAudioType* Instance, const UClass* Type);

	/** Sets an effect ShareSet on an output device
	*
	*  @param InDeviceID Output ID, as returned from AddOutput or GetOutputID. You can pass 0 for the main (default) output
	*  @param InEffectIndex Effect slot index (0-3)
	*  @param InEffectShareSet  Effect ShareSet asset
	*  @return Always returns True
	*/
	UFUNCTION(BlueprintCallable, Category = "Audiokinetic")
	static bool SetOutputDeviceEffect(const FAkOutputDeviceID InDeviceID, const int32 InEffectIndex, const UAkEffectShareSet* InEffectShareSet);

	/** Sets an Effect ShareSet at the specified Bus and Effect slot index.	
	*
	*  @param InBusName Bus name
	*  @param InEffectIndex Effect slot index (0-3)
	*  @param InEffectShareSet  Effect ShareSet asset
	*  @return True when successfully posted,  False otherwise.
	*/
	UFUNCTION(BlueprintCallable, Category = "Audiokinetic")
	static bool SetBusEffectByName(const FString InBusName, const int32 InEffectIndex, const UAkEffectShareSet* InEffectShareSet);

	/** Sets an Effect ShareSet at the specified Bus and Effect slot index.
	* 
	*  @param InBusID Bus Short ID.
	*  @param InEffectIndex Effect slot index (0-3)
	*  @param InEffectShareSet  Effect ShareSet asset
	*  @return True when successfully posted, False otherwise.
	*/
	UFUNCTION(BlueprintCallable, Category = "Audiokinetic")
	static bool SetBusEffectByID(const FAkUniqueID InBusID, const int32 InEffectIndex, const UAkEffectShareSet* InEffectShareSet);

	/** Sets an Effect ShareSet at the specified Bus and Effect slot index.
	* 
	*  @param InAuxBus Aux Bus Asset.
	*  @param InEffectIndex Effect slot index (0-3)
	*  @param InEffectShareSet  Effect ShareSet asset
	*  @return True when successfully posted, False otherwise.
	*/
	UFUNCTION(BlueprintCallable, Category = "Audiokinetic")
	static bool SetAuxBusEffect(const UAkAuxBus* InAuxBus, const int32 InEffectIndex, const UAkEffectShareSet* InEffectShareSet);

	/** Sets an Effect ShareSet at the specified audio node and Effect slot index.
	*
	* @param InAudioNodeID Can be a member of the Actor-Mixer or Interactive Music Hierarchy (not a bus).
	* @param InEffectIndex Effect slot index (0-3)
	* @param InEffectShareSet Effect ShareSet asset
	* @return Always returns True.
	*/
	UFUNCTION(BlueprintCallable, Category = "Audiokinetic")
	static bool SetActorMixerEffect(const FAkUniqueID InAudioNodeID, const int32 InEffectIndex, const UAkEffectShareSet* InEffectShareSet);

	/**
	 * Use the position of a separate Actor for distance calculations for a specified listener.
	 * When called, Wwise calculates distance attenuation and filtering
	 * based on the distance between the AkComponent on the distance probe Actor and the sound source.
	 * Useful for third-person perspective applications, the distance probe may be set to the player character's position,
	 * and the listener position to that of the camera. In this scenario, attenuation is based on
	 * the distance between the character and the sound, whereas panning, spatialization, and spread and focus calculations are base on the camera.
	 * @param Listener - The listener that is being affected. By default, the listener is attached to the Player Camera Manager.
	 * @param DistanceProbe - An actor to assign as the distance probe. 
	 */
	UFUNCTION(BlueprintCallable, Category = "Audiokinetic|Actor")
	static void SetDistanceProbe(AActor* Listener, AActor* DistanceProbe);

	static bool m_bSoundEngineRecording;

};