/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file ParticleEffect.hpp

#ifndef PARTICLEEFFECT_HPP_INCLUDED
#define PARTICLEEFFECT_HPP_INCLUDED

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Particles/ParticleModule.hpp>

// Declarations
class VisParticleGroupDescriptor_cl;
class VisParticleEffectFile_cl;
class VisParticleConstraint_cl;
class VisParticleConstraintList_cl;
class ParticleGroupBase_cl;
class VisParticleEffect_cl;

typedef VSmartPtr<VisParticleEffectFile_cl> VisParticleEffectFilePtr;
typedef VSmartPtr<ParticleGroupBase_cl> ParticleGroupBasePtr;
typedef VSmartPtr<VisParticleEffect_cl> VisParticleEffectPtr;

///\brief
/// This class represents a single particle effect instance. It is the native counterpart to vForge's Particle System shape
///
/// An effect instance can be instantiated via VisParticleEffectFile_cl::CreateParticleEffectInstance.
/// An instance of this class holds an array of active effect layer instances (each of type ParticleGroupBase_cl)
/// which correspond to instances of particle layers in the editor.
/// This class inherits positioning etc. from its parent VisObject3D_cl class.
///
///\see
///  VisParticleEffectFile_cl
class VisParticleEffect_cl : public VisObject3D_cl
{
public:
  V_DECLARE_SERIAL_DLLEXP( VisParticleEffect_cl,  PARTICLE_IMPEXP );

  ///\brief
  ///Constructor, same as VisParticleEffectFile_cl::CreateParticleEffectInstance
  PARTICLE_IMPEXP VisParticleEffect_cl(VisParticleEffectFile_cl *pEffect, const hkvVec3& vPos, const hkvVec3& vOri, unsigned int uiRandomSeed);
  ///\brief
  ///Constructor, same as VisParticleEffectFile_cl::CreateParticleEffectInstance
  PARTICLE_IMPEXP VisParticleEffect_cl(VisParticleEffectFile_cl *pEffect, const hkvVec3& vPos, const hkvVec3& vOri, float fScaling, unsigned int uiRandomSeed);

  ///\brief
  ///Internal creation helper
  ///\internal
  PARTICLE_IMPEXP void CreateFromDescriptors(VisParticleGroupDescriptor_cl **pDescList,int iDescCount);

  ///\brief
  ///Destructor
  PARTICLE_IMPEXP virtual ~VisParticleEffect_cl();

  ///\brief
  ///Sets the visible status. Obsolete since SetVisibleBitmask
  PARTICLE_IMPEXP void SetVisible(bool bStatus);
 
  ///\brief
  ///Sets the visible filter bitmask (on all layers)
  PARTICLE_IMPEXP void SetVisibleBitmask(unsigned int iMask);

  ///\brief
  ///Returns the visible state set via SetVisible
  PARTICLE_IMPEXP bool IsVisible() const;

  ///\brief
  ///Obsolete; Wraps around GetObjectKey
  inline const char *GetEffectKey() const {return GetObjectKey();}
  ///\brief
  ///Obsolete; Wraps around SetObjectKey
  inline void SetEffectKey(const char *szKey) {SetObjectKey(szKey);}

  /// \brief
  ///   Modifies the random base seed. In order for this to have any effect the effect must be restarted.
  /// \param uiRandomBaseSeed
  ///   The seed that is applied when the particle effect is started.
  /// \sa
  ///   Restart
  inline void SetRandomBaseSeed(unsigned int uiRandomBaseSeed) { m_uiRandomBaseSeed = uiRandomBaseSeed; }

  ///\brief
  /// Gets the random number generator base seed for this particle effect. This can
  /// be defined in vForge. If 0, a true random seed will be used (default).
  inline unsigned int GetRandomBaseSeed() const { return m_uiRandomBaseSeed; }

  ///\brief
  /// Moves this instance by specified delta. Optionally moves all particle positions (used by vForge positioning)
  PARTICLE_IMPEXP void IncPosition(const hkvVec3& vDelta, bool bMoveParticles = false);

  ///\brief
  /// Set scaling factor for all layers
  PARTICLE_IMPEXP void SetScaling(float fScale);

  /// \brief
  ///  set additive ambient color
  PARTICLE_IMPEXP void SetAmbientColor(VColorRef iColor);

  /// \brief
  ///   Re-evaluate the scene brightness
  PARTICLE_IMPEXP void EvaluateSceneBrightness();

  /// \brief
  ///   Calls respective function on all layers.
  /// 
  /// \deprecated
  ///   Use UpdateBoundingBoxes instead.
  HKV_DEPRECATED_2014_1 PARTICLE_IMPEXP void UpdateVisibilityBoundingBox();

  /// \brief
  ///   Updates the bounding boxes of all children.
  ///
  /// Usually it is not necessary to call this function manually, since it is called whenever an option is changed that influences the bounding box.
  ///
  /// \param bImmediateVisibilityBoundingBoxUpdate
  ///   If true the visibility bounding box will be updated immediately, otherwise this action will be delayed. This can be useful if you want to call this function in a seperate thread.
  PARTICLE_IMPEXP void UpdateBoundingBoxes(bool bImmediateVisibilityBoundingBoxUpdate = true);

  ///\brief
  ///Sets the pause status. If paused, the emitter does not spawn new particles
  PARTICLE_IMPEXP void SetPause(bool bStatus);

  ///\brief
  ///Returns the current pause status.
  inline bool IsPaused() const {return m_bPaused;}

  ///\brief
  ///Sets the halted status. When halted, existing particles do not move anymore
  PARTICLE_IMPEXP void SetHalted(bool bStatus);

  ///\brief
  ///Returns the current halted status.
  inline bool IsHalted() const {return m_bHalted;}

  ///\brief
  /// Stops the emitters and waits until all particles have disappeared. Then removes the particle effect
  PARTICLE_IMPEXP void SetFinished();

  ///\brief
  /// If set to true, the light of the time-of-day component will be added to the particles color.
  PARTICLE_IMPEXP void SetApplyTimeOfDayLight(bool bApply);

  ///\brief
  /// Returns whether time-of-day light is supposed to be applied on this effect.
  PARTICLE_IMPEXP bool GetApplyTimeOfDayLight(void) const;

  ///\brief
  /// When this flag is enabled (default) then this instance is deleted as soon as it is dead (all layers finished).
  ///
  /// Disable this flag to be able to call Restart()
  ///
  ///\param bStatus
  /// New status 
  inline void SetRemoveWhenFinished(bool bStatus);

  ///\brief
  /// Returns the flag that has been set via SetRemoveWhenFinished. Default is true.
  inline bool GetRemoveWhenFinished() const;

  ///\brief
  /// Overridden function. Removes this instance from the simulation collection
  PARTICLE_IMPEXP virtual void DisposeObject() HKV_OVERRIDE;

  ///\brief
  /// Obsolete; Wraps around DisposeObject
  inline void Dispose();

  ///\brief
  /// Changes the simulation state for all layers. If false, the particle effect is also simulated when not visible; If true, the particle effect is simulated only when visible.
  PARTICLE_IMPEXP void SetHandleWhenVisible(bool bStatus);

  ///\brief
  /// Returns true if ALL of its particle groups:
  ///  - are marked as handled only when visible (m_bHandleWhenVisible==true)
  ///  - do not have to update their lifetime if not visible (m_bUpdateLifetimeIfInvisible==false)
  PARTICLE_IMPEXP bool IsUpdatedOnlyWhenVisible();

  ///\brief
  /// Returns true if any of its particle groups was rendered in the last frame. Returns false otherwise.
  PARTICLE_IMPEXP bool WasRecentlyRendered();

  ///\brief
  /// Restarts the particle effect. 
  ///
  /// \note
  ///   If RemoveWhenFinished is enabled, this will only restart the particle groups that haven't finished yet.
  ///
  /// \see SetRemoveWhenFinished
  PARTICLE_IMPEXP void Restart();

  ///\brief
  /// Sets the emitter intensity on all layers. This factor modulates the spawn frequency of the emitters  
  ///
  ///\param fIntensity
  /// The new intensity value. Should be in 0..1 range
  ///
  ///An intensity of 1.0f (default) spawns particles at full frequency as defined in the editor. An intensity of 0.0f
  ///pauses the emitter. However, in that case SetPaused(true) should be used.
  PARTICLE_IMPEXP virtual void SetIntensity(float fIntensity);

  ///\brief
  ///Re-spawns all particles in all layers
  ///
  ///\param bUseOldCount
  ///  if true, the current number of active particles is used, otherwise the predicted count at spawn time
  PARTICLE_IMPEXP void RespawnAllParticles(bool bUseOldCount=true);
  
  ///\brief
  ///   Moves the position of the emitters to the current position of the corresponding particle groups
  ///
  /// The particle system does by default interpolate the emitter position over time. This means that moving
  /// the particle group in one frame will by default still cause particles to be emitted at the old position and
  /// possibly between the old and new position.
  ///
  /// If you want to move a particle effect to a new position, and want to make sure that all newly created
  /// particles of this effect are created at the new position as well, then simply call this method after you have
  /// moved the particle effect.
  PARTICLE_IMPEXP void TeleportSpawnPosition();

  /// \brief
  ///   Get the local bounding box of the layer descriptors, merged from all children.
  ///
  /// \param destBox
  ///   Box that will be inflated with the boxes of the children. Old contents will be invalidated!
  ///
  /// \deprecated
  ///   Use either GetLocalSpaceBoundingBox or GetWorldSpaceBoundingBox
  HKV_DEPRECATED_2014_1 PARTICLE_IMPEXP bool GetLocalBoundingBox(hkvAlignedBBox &destBox) const;

  /// \brief
  ///   Get the updated bounding box (merged from all children)
  ///
  /// \param destBox
  ///   Box that will be inflated with the boxes of the children. Old contents will be invalidated!
  ///
  /// \deprecated
  ///   Use either GetLocalSpaceBoundingBox or GetWorldSpaceBoundingBox
  HKV_DEPRECATED_2014_1 PARTICLE_IMPEXP bool GetCurrentBoundingBox(hkvAlignedBBox &destBox) const;

  /// \brief
  ///   Returns a reference to the local space bounding box. (merged from all children)
  ///
  /// \param destBox
  ///   Box that will be inflated with the boxes of the children. Old contents will be invalidated!
  ///
  /// \see
  ///   GetWorldSpaceBoundingBox
  PARTICLE_IMPEXP void GetLocalSpaceBoundingBox(hkvAlignedBBox& destBox) const;

  /// \brief
  ///   Returns a reference to the world space bounding box. (merged from all children)
  ///
  /// \param destBox
  ///   Box that will be inflated with the boxes of the children. Old contents will be invalidated!
  ///
  /// \see
  ///   GetLocalSpaceBoundingBox
  PARTICLE_IMPEXP void GetWorldSpaceBoundingBox(hkvAlignedBBox& destBox) const;

  ///\brief
  ///Adds constraints from the passed source list to all layers and optionally performs early out using the constraint's Influences function
  PARTICLE_IMPEXP void AddRelevantConstraints(const VisParticleConstraintList_cl *pSrcList, bool bCheckInfluence=true);

  ///\brief
  ///Adds a single constraint to all layers and optionally performs early out using the constraint's Influences function
  PARTICLE_IMPEXP void AddConstraint(VisParticleConstraint_cl *pConstraint, bool bCheckInfluence=true);

  ///\brief
  ///Detach the passed constraint from all layers
  PARTICLE_IMPEXP void RemoveConstraint(VisParticleConstraint_cl *pConstraint);
  ///\brief
  ///Detach all constraints from all layers
  PARTICLE_IMPEXP void RemoveAllConstraints();

  ///\brief
  ///Sets a wind speed vector for all layers. Simple wind can be applied with no performance impact
  PARTICLE_IMPEXP void SetWindSpeed(const hkvVec3& vWind, bool bApplyInLocalSpace=false);

  ///\brief
  ///Associate all layers with emitter type 'mesh' with the passed entity instance
  PARTICLE_IMPEXP void SetMeshEmitterEntity(VisBaseEntity_cl *pEntity);

  ///\brief
  /// All layers using dynamic lighting will prefer directional lights with the given key.
  /// 
  /// If no preferred directional light with the given key is available, it will be chosen automatically.
  ///
  /// \param szPreferredDirectionalLightSourceKey
  ///   Object key of the preferred directional light source
  PARTICLE_IMPEXP void SetPreferredDirLightKey(const char* szPreferredDirectionalLightSourceKey);

  ///\brief
  /// Sets an offset for lightgrid or mobile dynamic light sampling.
  inline void SetLightSamplingOffset(const hkvVec3& vLightSamplingOffset);

  ///\brief
  /// Gets value previously set using SetLightSamplingOffset
  ///
  /// Defalt is (0,0,0)
  inline hkvVec3 GetLightSamplingOffset() const;


  /// \brief
  ///   Sets dynamic lighting priority threshold. All light with priorities under this threshold will be ignored.
  ///   The priority is given by the approximate screen area in pixels.
  ///
  /// \param uiDynamicLightingPriorityThreshhold
  ///   New threshold value. Default is 64.
  ///
  /// \see GetDynamicLightingPriorityThreshhold
  inline void SetDynamicLightingPriorityThreshhold(unsigned int uiDynamicLightingPriorityThreshhold);

  /// \brief
  ///   Gets threshold value previously set with SetDynamicLightingPriorityThreshhold.
  ///
  /// \see SetDynamicLightingPriorityThreshhold
  inline unsigned int GetDynamicLightingPriorityThreshhold() const;

  /// \brief
  ///  Renders a bounding box for every single particle.
  PARTICLE_IMPEXP void RenderParticleBoundingBoxes();

  /// \brief
  ///   Performs the simulation for all layers
  PARTICLE_IMPEXP void Tick(float fTimeDiff);

  /// \brief
  ///  Indicates whether the lifetime of all layers is used up
  PARTICLE_IMPEXP bool IsLifeTimeOver() const;

  ///\brief
  /// Indicates whether this instance is ready for removal.
  /// 
  /// That is if there are no groups anymore and RemoveWhenFinished is true.
  /// \see SetRemoveWhenFinished
  inline bool IsDead() const;

  ///\brief
  /// Return the (remaining) number of layers in this effect
  inline int GetParticleGroupCount() const;

  ///\brief
  /// Return the layer with specified index
  inline ParticleGroupBase_cl *GetParticleGroup(int iIndex) const;

  ///\brief
  ///Returns the remaining lifetime of this effect in seconds. If any layer has infinite lifetime, this function returns -1.0f
  PARTICLE_IMPEXP float GetRemainingLifeTime() const;

  ///\brief
  /// Adds remaining layers to the passed collection
  PARTICLE_IMPEXP void AddGroupsToCollection(VisParticleGroupCollection_cl &dest);

  ///\brief
  /// Returns the particle effect resource that was used to create this instance
  ///
  ///\returns
  /// The particle effect resource 
  inline VisParticleEffectFile_cl* GetSourceEffect() const { return m_spSourceFXFile; }

  /// \brief
  ///   Returns the bitmask that defines the dynamic light influence behaviour.
  /// 
  /// The bitmask is set via VisParticleEffect_cl::SetLightInfluenceBitMask. 
  /// 
  /// \return
  ///   unsigned int uiLightMask : The light influence bitmask for this particle effect. Only relevant if GetUseDynamicLighting() == true.
  ///   The effect will only be influenced by light sources that have a matching bit in their bitmask.
  /// 
  /// \sa VisParticleEffect_cl::SetLightInfluenceBitMask
  /// \sa VisParticleEffect_cl::GetUseDynamicLighting
  /// \sa VisLightSource_cl::SetLightInfluenceBitMasks
  /// \sa VisLightSource_cl::GetLightInfluenceBitMasks
  inline unsigned int GetLightInfluenceBitMask() const;

  /// \brief
  ///   Sets a bitmask that defines the light influence behaviour.
  /// 
  /// Only relevant if GetUseDynamicLighting() == true. This value allows you to configure which dynamic light sources affect this entity.
  /// At runtime, the light's bitmask (set via VisLightSource_cl::SetLightInfluenceBitMasks will be AND-ed with the entity's
  /// bitmask, and if the result is nonzero, the light will affect the entity.
  /// 
  /// \param uiLightMask
  ///   The light influence bitmask for this entity. The entity will only be influenced by
  ///   light sources that have a matching bit in their bitmask.
  /// 
  /// \note
  ///   The default bitmask for lights and entities is 1.
  /// 
  /// \sa VisParticleEffect_cl::GetLightInfluenceBitMask
  /// \sa VisParticleEffect_cl::GetUseDynamicLighting
  /// \sa VisLightSource_cl::SetLightInfluenceBitMasks
  /// \sa VisLightSource_cl::GetLightInfluenceBitMasks
  inline void SetLightInfluenceBitMask(unsigned int uiLightMask);

  ///\brief
  /// Returns the own cached light grid color
  ///
  ///\returns
  /// Pointer to the lightgrid colors
  inline const hkvVec3* GetCachedLightgridColors() const;

  ///\brief
  /// Updates cached lightgrid colors.
  PARTICLE_IMPEXP void UpdateLightGrid();

  ///\brief
  /// Update lightgrid and dynamic lighting settings.
  /// 
  /// Iterates over all particle groups to determine the need for lightgrid or dynamic light tracking.
  /// Dynamic lighting is determined from shader flags, lightgrid is on if lightgrid color pointer matches m_OwnLGColors.
  PARTICLE_IMPEXP void UpdateLightTrackingSettings();

  ///\brief
  /// Gets whether to use light grid or not
  inline bool GetUseLightgrid() const;

  ///\brief
  /// Gets whether to use dynamic lighting or not
  inline bool GetUseDynamicLighting() const;

  /// \brief
  ///   Returns a hkvVec4* array with s_uiNumDynamicLightShaderConstantRegisters elements containing dynamic lighting informations
  inline const hkvVec4* GetDynamicLightingShaderConstants();

  /// \brief
  ///   Returns shadow map texture of shadowmap used for shadow receive.
  ///  
  /// \returns  Returns NULL if no shadow receive map was selected so far.
  inline VTextureObject* GetShadowReceiveShadowMap();

  ///
  /// @name Network related
  /// @{
  ///

  PARTICLE_IMPEXP int GetSynchronizationGroupList(const VNetworkViewContext &context, VNetworkSynchronizationGroupInstanceInfo_t *pDestList);

  ///
  /// @}
  ///
  

public:
  // VisObject3D_cl class overrides
  PARTICLE_IMPEXP virtual void OnObject3DChanged(int iO3DFlags) HKV_OVERRIDE;
  PARTICLE_IMPEXP virtual void OnReposition(const VisZoneRepositionInfo_t &info, const hkvVec3 &vLocalPos) HKV_OVERRIDE;

  PARTICLE_IMPEXP virtual void ModSysNotifyFunctionParentAltered(int iFlags);
  
  /// \brief
  ///   Uses the virtual function of the child system to define the behavior upon dispose.
  PARTICLE_IMPEXP virtual void ModSysNotifyFunctionCommand(int command, void *param) HKV_OVERRIDE;

  
#ifdef SUPPORTS_SNAPSHOT_CREATION
  PARTICLE_IMPEXP virtual void GetDependencies(VResourceSnapshot &snapshot) HKV_OVERRIDE;
#endif
  PARTICLE_IMPEXP virtual void MessageFunction(int iID, INT_PTR iParamA, INT_PTR iParamB) HKV_OVERRIDE;
  PARTICLE_IMPEXP VisParticleEffect_cl();

protected:
  PARTICLE_IMPEXP virtual void Serialize( VArchive &ar ) HKV_OVERRIDE;
  PARTICLE_IMPEXP virtual VBool WantsDeserializationCallback(const VSerializationContext &context) HKV_OVERRIDE
  {
    // only call it in this case as the emitter mesh gets lost otherwise
    return context.m_eType == VSerializationContext::VSERIALIZATION_ARCHIVE ? TRUE : FALSE;
  }
  PARTICLE_IMPEXP virtual void OnDeserializationCallback(const VSerializationContext &context) HKV_OVERRIDE;

  void EnsureComponentIDsRegistered();

  /// \brief Helper function for constructors to set members to default values.
  void ResetValues();

  /// \brief List entry for saving light with corresponding priority.
  struct DynamicLightListEntry
  {
    DynamicLightListEntry() : pLight(NULL), uiPriority(0) {}

    VisLightSource_cl* pLight;
    unsigned int uiPriority;
  };

  /// \brief Estimates how many screen space pixels of light and particle bounding box overlap.
  /// 
  /// Lights with priority 0 can be ignored!
  ///
  /// \param lightSource
  ///   Light source to check against the bounding box.
  /// \param particleEffectBoundingBox
  ///   Particle effect bounding box to check the light.
  /// \param viewProjectionMatrix
  ///   ViewProjection matrix that defines screen and camera orientation.
  ///
  /// \returns Priority value in pixel. Will use size of current rendercontext.
  unsigned int GetLightScreenSpaceOverlapArea(const VisLightSource_cl& lightSource, const hkvAlignedBBox& particleEffectBoundingBox, const hkvMat4& viewProjectionMatrix) const;


  /// \brief Updates dynamic light spherical harmonics coefficients.
  ///
  /// Only needed if dynamic lighting is active and m_bUseSHForDynamicLighting is true.
  PARTICLE_IMPEXP void UpdateDynamicLightSH();


  /// \brief Updates dynamic light lists.
  ///
  /// Only needed if dynamic lighting is active and m_bUseSPHForDynamicLighting is false.
  PARTICLE_IMPEXP void UpdateDynamicLightLists();

  /// \brief Updates dynamic light shader constants. See m_pDynamicLightingShaderConstants
  /// 
  /// Helper function called by UpdateDynamicLightLists.
  ///
  /// \param aDynamicLightListDirectional
  ///   Array of directional lights.
  /// \param aDynamicLightListPoint
  ///   Array of directional lights.
  /// \param aDynamicLightListSpot
  ///   Array of directional lights.
  ///
  PARTICLE_IMPEXP void UpdateDynamicLightListConstants(DynamicLightListEntry aDynamicLightListDirectional[],
                                                       DynamicLightListEntry aDynamicLightListPoint[],
                                                       DynamicLightListEntry aDynamicLightListSpot[]);

  /// \brief Updates shadow map selection - both texture and constants. See m_pDynamicLightingShaderConstants and m_spShadowReceiveShadowMap.
  /// 
  /// Helper function called by UpdateDynamicLightLists.
  ///
  /// \param pDirectionalLightSource
  ///   Directional light source that contains a shadow map component. If there is none, shadowmap will be reseted.
  ///
  PARTICLE_IMPEXP void UpdateShadowMapSelection(VisLightSource_cl* pDirectionalLightSource);

  friend class HandleParticlesTask_cl;
  friend class ParticleGroupBase_cl;
  friend class VisParticleEffectFile_cl;
  friend class VisParticleGroupDescriptor_cl;

  void OnSingleGroupFinished(ParticleGroupBase_cl *pFinishedGroup);
  VisParticleEffectFilePtr m_spSourceFXFile;

  unsigned int m_iGroupCount;        ///< remaining number of groups
  unsigned int m_uiRandomBaseSeed;  ///< if non-zero, contains the custom based seed for random number generation for this particle effect
  ParticleGroupBasePtr *m_spGroups; ///< array of smart pointers
  bool m_bPaused, m_bHalted, m_bUseLightgridLighting; ///< status flags
  bool m_bUseDynamicLighting;       ///< if enabled particle effect will track dynamic lights. Either through light lists or spherical harmonics.
  bool m_bUseSHForDynamicLighting; ///< if enabled dynamic particle lighting will make use of spherical harmonics instead of light lists.
  bool m_bRemoveWhenFinished;       ///< if enabled (default), all layers that have finished their simulation will be removed. This particle effect will be marked dead when all groups are removed and this flag is activated.
  bool m_bAnyMeshEmitter;           ///< indicates whether any layer uses an entity mesh emitter (special treatment in the main thread update)
  
  hkvVec3 m_vLightSamplingOffset;   ///< light sampling offset used for lightgrid and mobile lighting
  hkvVec3 m_OwnLGColors[6];         ///< own cached light grid colors

  unsigned int m_uiLightInfluenceBitmask; ///< Lightbitmask set with SetLightInfluenceBitmask. Only relevant if dynamic lighting is enabled.
  VString m_sPreferredDirectionalLightSourceKey; ///< Key of Directional light source that will be used by default. Automatically choosen if NULL (default).

  unsigned int m_uiLastDynamicLightListUpdateFrame; ///< Frame number of the last dynamic light list update
  VisRenderContext_cl* m_pLastDynamicLightListUpdateContext;

  /// Dynamic lighting shader constant settings. Updated within UpdateDynamicLightListConstants or UpdateDynamicLightSH.
  /// This is highly dependent on the default particle shader.
  hkvVec4* m_pDynamicLightingShaderConstants;


  /// True if m_pShadowReceiveShadowMap is updated with every UpdateLightLists.
  bool m_bNeedsShadowMapSelectionUpdate;
  /// Ptr to the shadow map used for shadow receiving. NULL if none is selected.
  VTextureObjectPtr m_spShadowReceiveShadowMap;

  /// During UpdateDynamicLightLists() lights with priorities below this threshold will be ignored.
  unsigned int m_uiDynamicLightingPriorityThreshhold;
};


///\brief
/// Collection class for refcounted particle effect instances
///
///This class adds convenience functions to perform simulation (Tick) or purge the list
class VisParticleEffectCollection_cl : public VRefCountedCollection<VisParticleEffect_cl>
{
public:

  ///\brief
  /// Performs a simulation tick on all instances. 
  PARTICLE_IMPEXP void Tick(float fTimeDelta, bool bPurgeDead=true);

  ///\brief
  /// Re-evaluate scene brightness for all instances
  PARTICLE_IMPEXP void EvaluateSceneBrightness();

  ///\brief
  /// Purges all instances that have finished simulation
  PARTICLE_IMPEXP void Purge(bool bDeadOnly=false);

  ///\brief
  /// Reassign all the shaders
  PARTICLE_IMPEXP void ReassignShader(bool bRecreateFX);

  ///\brief
  /// return the first occurance of an effect with specified key in this collection
  ///
  ///This is very similar to VisGame_cl::SearchEntity etc.
  ///
  ///\param szKey
  ///  Key to search for
  ///
  ///\param pStoreArray
  ///  Optional array that receives all hits (otherwise just the first hit is returned)
  ///
  ///\return
  ///  First hit with specified key, or NULL if key wasn't found
  PARTICLE_IMPEXP VisParticleEffect_cl *FindByKey(const char *szKey, DynArray_cl<VisParticleEffect_cl *> *pStoreArray=NULL) const;

};


/// \brief
///   Implements IVNetworkSynchronizationGroup as a synchronization group that synchronizes particle effects. This version supports interpolation
///
/// There is one global instance: g_Instance
class VNetworkParticleEffectGroupI : public IVNetworkSynchronizationGroup
{
public:
  /// \brief
  ///   Data history
  class VHistoryDataParticleEffect : public IVNetworkSynchronizationGroupInstanceData
  {
  public:
    VisDataHistory_cl<BYTE, 3, VStepFunction<BYTE> > m_particleFlagHistory;
  };

  /// \brief
  ///   Flags defining the status of the particle effect group
  enum VParticleFlags
  {
    VCF_PAUSED = V_BIT(0),
    VCF_HALTED = V_BIT(1),
  };

  PARTICLE_IMPEXP virtual const char *GetName() const HKV_OVERRIDE {return "ParticleEffectSettings";}
  PARTICLE_IMPEXP virtual VGroupTypeFlags_e GetUsageTypeFlags() const HKV_OVERRIDE {return VGroupTypeBit_ParticleEffect;}
  PARTICLE_IMPEXP virtual IVNetworkSynchronizationGroupInstanceData* CreatePerInstanceData() const HKV_OVERRIDE {return (new VHistoryDataParticleEffect);}
  PARTICLE_IMPEXP virtual bool QuerySynchronize(const VNetworkViewContext& context, VNetworkSynchronizationGroupInstanceInfo_t &instanceInfo, VMessageSettings& out_paketSettings) HKV_OVERRIDE;
  PARTICLE_IMPEXP virtual void Synchronize(const VNetworkViewContext& context, VNetworkSynchronizationGroupInstanceInfo_t &instanceInfo, VArchive &ar) HKV_OVERRIDE;
  PARTICLE_IMPEXP virtual void TickFunction(const VNetworkViewContext &context, VNetworkSynchronizationGroupInstanceInfo_t &instanceInfo, float fTimeDelta) HKV_OVERRIDE;

  PARTICLE_IMPEXP static VNetworkParticleEffectGroupI g_Instance; ///< paused flag, halted flag
};


/// \brief
///   Similar to VNetworkParticleEffectGroupI but without interpolation
///
/// There is one global instance: g_Instance
class VNetworkParticleEffectGroup : public IVNetworkSynchronizationGroup
{
public:
  /// \brief
  ///   Flags defining the status of the particle effect group
  enum VParticleFlags
  {
    VCF_PAUSED = V_BIT(0),
    VCF_HALTED = V_BIT(1),
  };

  PARTICLE_IMPEXP virtual const char *GetName() const HKV_OVERRIDE {return "ParticleEffectSettings";}
  PARTICLE_IMPEXP virtual VGroupTypeFlags_e GetUsageTypeFlags() const HKV_OVERRIDE {return VGroupTypeBit_ParticleEffect;}
  PARTICLE_IMPEXP virtual bool QuerySynchronize(const VNetworkViewContext& context, VNetworkSynchronizationGroupInstanceInfo_t &instanceInfo, VMessageSettings& out_paketSettings) HKV_OVERRIDE;
  PARTICLE_IMPEXP virtual void Synchronize(const VNetworkViewContext& context, VNetworkSynchronizationGroupInstanceInfo_t &instanceInfo, VArchive &ar) HKV_OVERRIDE;

  PARTICLE_IMPEXP static VNetworkParticleEffectGroup g_Instance; ///< paused flag, halted flag
};

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Particles/ParticleEffect.inl>

#endif

/*
 * Havok SDK - Base file, BUILD(#20140618)
 * 
 * Confidential Information of Havok.  (C) Copyright 1999-2014
 * Telekinesys Research Limited t/a Havok. All Rights Reserved. The Havok
 * Logo, and the Havok buzzsaw logo are trademarks of Havok.  Title, ownership
 * rights, and intellectual property rights in the Havok software remain in
 * Havok and/or its suppliers.
 * 
 * Use of this software for evaluation purposes is subject to and indicates
 * acceptance of the End User licence Agreement for this product. A copy of
 * the license is included with this software and is also available from salesteam@havok.com.
 * 
 */
