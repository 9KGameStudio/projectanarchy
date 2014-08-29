/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file ParticleGroupBase.hpp

#ifndef PARTICLEGROUPBASE_HPP_INCLUDED
#define PARTICLEGROUPBASE_HPP_INCLUDED

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Particles/ParticleModule.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Particles/ParticleEffect.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Particles/ParticleDescriptor.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Particles/ParticleConstraint.hpp>

#include <Vision/Runtime/Engine/SceneElements/VisApiParticleGroup.hpp>

#include <Vision/Runtime/Base/Math/BoundingVolume/hkvAlignedBBox.h>
#include <Vision/Runtime/Base/Math/Random/VRandom.hpp>

// group flags
#define PGROUPFLAGS_NONE        0x00000000

// maximum recursion depth for particle groups that have a child group for destroyed particles
#define MAX_ONDESTROY_GROUPDEPTH    8

// minimum cache size, also used for static member size
#define PARTICLECACHESIZE_NOALLOC   32

// known trigger target constants
#define PARTICLETRIGGER_PAUSE     "Pause"
#define PARTICLETRIGGER_RESUME    "Resume"
#define PARTICLETRIGGER_RESTART   "Restart"

// Declarations
class ParticleGroupBase_cl;
class VisParticleGroupDescriptor_cl;
class VisParticleGroupManager_cl;
class VisParticleEffect_cl;
class VisParticleEmitter_cl;
typedef VSmartPtr<ParticleGroupBase_cl> ParticleGroupBasePtr;

///\brief
///Threaded task class used by ParticleGroupBase_cl to perform asynchronous simulation
class HandleParticlesTask_cl : public VThreadedTask
{
public:
  ///\brief
  ///Constructor
  HandleParticlesTask_cl(ParticleGroupBase_cl *pGroup);

  ///\brief
  ///Overridden Run function that performs the actual simulation
  virtual void Run(VManagedThread *pThread);

  ParticleGroupBase_cl *m_pParticleGroup; ///< owner group
  float m_fTimeDelta; ///< time delta used for simulation

public:
  //type management
  inline HandleParticlesTask_cl() {}
  V_DECLARE_DYNCREATE_DLLEXP( HandleParticlesTask_cl,  PARTICLE_IMPEXP );
};


///\brief
///Structure that represents a single particle of the extended particle system. Adds more particle specific properties
struct ParticleExt_t : public Particle_t
{
  float m_fDistortionMult;  ///< scaling of distortion length
  float m_fLifeTimeCounter; ///< for handling lifetime
  float m_fLifeTimeInc;     ///< for handling lifetime
  float m_fAnimPos,m_fAnimInc; ///< for handling animation
  float m_fRotationParam0;  ///< speed for mode PARTICLE_ROTATION_CONSTSPEED or curve pos for PARTICLE_ROTATION_ANGLECURVE
  float m_fRotationParam1;  ///< curve pos incrementor for PARTICLE_ROTATION_ANGLECURVE
  float m_fSizeGrowth;      ///< growth of the particle
  float m_fInertiaFactor;   ///< inertia value [0..1]
  VColorRef m_ModColor;     ///< color for modulating with the current frame color
};


///\brief
///Generic interface to access a particle array. Used for the constraint simulation. Implemented by particle effects and cloth
class IVPhysicsParticleCollection_cl
{
public:
  virtual ~IVPhysicsParticleCollection_cl(){}

  ///\brief
  ///  Must be implemented to return the array size (number of particles)
  PARTICLE_IMPEXP virtual int GetPhysicsParticleCount() const = 0;

  ///\brief
  ///  Must be implemented to return the stride in bytes between two particles in the array, e.g. sizeof(ParticleExt_t)
  PARTICLE_IMPEXP virtual int GetPhysicsParticleStride() const = 0;

  ///\brief
  ///  Must be implemented to return the pointer to the array itself
  PARTICLE_IMPEXP virtual Particle_t *GetPhysicsParticleArray() const = 0;

  ///\brief
  ///  Must be implemented to process particles that should be destroyed
  PARTICLE_IMPEXP virtual void DestroyParticle(Particle_t *pParticle,float fTimeDelta) = 0;
  
  /// \brief
  ///  Updates the seed offset using the previously set base seed and current engine
  ///  frame information. The application can control this behavior by calling 
  ///  Vision::Game.GetUpdateSceneCount().
  inline void UpdateSeed();

  /// \brief
  ///  Get const access to the generator 
  /// \note
  ///  By only allowing const access, application cannot call the seed functions
  ///  directly, which would break the guaranteed synchronization of pseudo-random
  ///  numbers between runs of the engine.
  inline const VRandom& GetRandom() const { return m_generator; }

  /// \brief
  ///   Returns the base random number seed.
  inline unsigned int GetBaseSeed() const { return m_uiBaseSeed; }

  /// \brief
  ///   Specifies the base random number seed.
  inline void SetBaseSeed(unsigned int uiBaseSeed) { m_uiBaseSeed = uiBaseSeed; }

protected:
  IVPhysicsParticleCollection_cl(unsigned int uiBaseSeed=0);

private:
  unsigned int m_uiBaseSeed; ///< Set in vForge using RandomBaseSeed in the properties
  VRandom m_generator;
};


///\brief
/// This callback data object type is fired by the VisParticleGroupManager_cl::OnLayerEvent event. It holds the particle effect layer instance and the event
class VisParticleLayerEventData_cl : public IVisCallbackDataObject_cl
{
public:
  VisParticleLayerEventData_cl(ParticleGroupBase_cl *pLayer, VisAnimEvent_cl *pEvent);

  ParticleGroupBase_cl *m_pLayer; ///< The particle effect layer instance that triggered the event
  VisAnimEvent_cl *m_pEvent;      ///< the event (see class VisAnimEvent_cl)
};


///\brief
/// This class corresponds to the instance of a particle layer
///
///This class is usually not directly instantiated but rather part of a full effect instance which is represented
///by class VisParticleEffect_cl 
class ParticleGroupBase_cl : public VisParticleGroup_cl, public IVPhysicsParticleCollection_cl
{
public:
  ///\brief
  ///Constructor
  PARTICLE_IMPEXP ParticleGroupBase_cl(VisParticleGroupDescriptor_cl *pDescr, VisParticleEmitter_cl *pEmitter, const hkvVec3& vSpawnPos, unsigned int uiRandomSeed);
  ///\brief
  ///Constructor
  PARTICLE_IMPEXP ParticleGroupBase_cl(VisParticleGroupDescriptor_cl *pDescr, VisParticleEmitter_cl *pEmitter, const hkvVec3& vSpawnPos, const hkvVec3& vOrientation, bool bSpawnParticles, unsigned int uiRandomSeed);
  ///\brief
  ///Constructor
  PARTICLE_IMPEXP ParticleGroupBase_cl(VisParticleGroupDescriptor_cl *pDescr, VisParticleEmitter_cl *pEmitter, const hkvVec3& vSpawnPos, const hkvVec3& vOrientation, float fScaling, bool bSpawnParticles, unsigned int uiRandomSeed);
  ///\brief
  ///Destructor
  PARTICLE_IMPEXP virtual ~ParticleGroupBase_cl();

  ///\brief
  ///Associates an emitter instance with this layer
  inline void SetEmitter(VisParticleEmitter_cl *pEmitter);
  ///\brief
  ///Returns the current emitter instance of this layer
  inline VisParticleEmitter_cl* GetEmitter() const;

  ///\brief
  ///Returns the descriptor that provides this layer's properties
  inline VisParticleGroupDescriptor_cl *GetDescriptor() const;

  ///\brief
  ///Internal - do not use
  ParticleGroupBase_cl *GetOnDestroyCreateGroup() const {return m_spOnDestroyCreateGroup;}

  ///\brief
  ///Internal - do not use
  inline VisParticleEffect_cl *GetParentEffect() const
  {
    if (VISION_UNLIKELY(m_pParentGroup!=NULL)) 
      return m_pParentGroup->GetParentEffect();
    return m_pParentEffect;
  }
  ///\brief
  ///Internal - do not use
  void SetInitialTransformation();

  ///\brief
  ///Overridden VisParticleGroup_cl function to calculate a distance to camera that is appropriate for sorting. It uses the parent's effect center
  virtual float GetSortingDistance(const VisObject3D_cl *pCamera) HKV_OVERRIDE;
  
  ///\brief
  ///Accesses the array as an array of ParticleExt_t
  inline ParticleExt_t* GetParticlesExt() const {return (ParticleExt_t *)GetParticles();}

  ///\brief
  ///Implements the IVPhysicsParticleCollection_cl interface
  virtual int GetPhysicsParticleStride() const HKV_OVERRIDE {return sizeof(ParticleExt_t);}
  ///\brief
  ///Implements the IVPhysicsParticleCollection_cl interface
  virtual int GetPhysicsParticleCount() const HKV_OVERRIDE {return m_iHighWaterMark;}
  ///\brief
  ///Implements the IVPhysicsParticleCollection_cl interface
  virtual Particle_t *GetPhysicsParticleArray() const HKV_OVERRIDE {return (ParticleExt_t *)GetParticles();}
  ///\brief
  ///Implements the IVPhysicsParticleCollection_cl interface
  PARTICLE_IMPEXP virtual void DestroyParticle(Particle_t *pParticle,float fTimeDelta) HKV_OVERRIDE;

  ///\brief
  ///Orientation helper function
  inline void SetOrientation(float fYaw,float fPitch,float fRoll);
  ///\brief
  ///Orientation helper function
  inline void SetOrientation(const hkvMat3 &mRotMat);
  ///\brief
  ///Orientation helper function
  inline void SetOrientation(const hkvVec3& vDir, const hkvVec3& vUp, const hkvVec3& vRight);
  ///\brief
  ///Moves this layer by certain position delta
  PARTICLE_IMPEXP void IncPosition(const hkvVec3& vDelta);
  ///\brief
  ///Returns the current direction
  PARTICLE_IMPEXP hkvVec3 GetDirection();

  ///\brief
  ///   Updates the visibility bounding box by evaluating the particle's box.
  ///
  /// \deprecated
  ///   Use UpdateBoundingBoxes instead.
  HKV_DEPRECATED_2014_1 PARTICLE_IMPEXP void UpdateVisibilityObject();

  ///\brief
  ///Applies a uniform scaling factor
  inline void SetScaling(float fScale=1.f);
  ///\brief
  ///Return current scaling factor
  inline float GetScaling() const;

  ///\brief
  ///Moves all particle positions by specified position delta
  PARTICLE_IMPEXP void MoveParticles(const hkvVec3& vDelta);
  ///\brief
  ///Simulation properties as defined in vForge
  PARTICLE_IMPEXP void SetLocalFactors(float fAtLifetimeStart, float fAtLifetimeEnd);

  ///\brief
  ///Sets whether to add the time-of-day light to the particles color.
  inline void SetApplyTimeOfDayLight(bool bApply) {m_bApplyTimeOfDayLight = bApply; EvaluateSceneBrightness ();}
  ///\brief
  ///Returns whether to add the time-of-day light to the particles color.
  inline bool GetApplyTimeOfDayLight() const {return (m_bApplyTimeOfDayLight);}

  ///\brief
  ///Sets the pause status. If paused, the emitter does not spawn new particles
  inline void SetPause(bool bStatus);
  ///\brief
  ///Returns the current pause status.
  inline bool IsPaused() const;

  ///\brief
  ///Sets the halted status. When halted, existing particles do not move anymore
  inline void SetHalted(bool bStatus);
  ///\brief
  ///Returns the current halted status.
  inline bool IsHalted() const;

  ///\brief
  ///If true, the particle lifetime is still updated while particles are invisible (default: false)
  ///This makes particles to get destroyed even if they are invisible (and thus not simulated).
  ///This can be used in combination with SetHandleWhenVisible(true).
  ///
  ///\param bStatus
  ///  If true, life is drained even if particles are invisible.
  inline void SetUpdateLifetimeIfInvisible(bool bStatus) { m_bUpdateLifetimeIfInvisible = bStatus; }

  ///\brief
  ///Returns true if particles lifetime is updated even while they are invisible.
  inline bool IsLifetimeUpdatedIfInvisible() const  { return m_bUpdateLifetimeIfInvisible; }

  ///\brief
  ///A "dead" group just waits until there are no refs anymore to delete itself
  inline bool IsDead() const {return m_bIsDead;}
  ///\brief
  ///Indicates whether the layer's lifetime is used up
  inline bool IsLifeTimeOver()
  {
    if (m_bInfiniteLifeTime)
      return false;

    return (m_fLifeTime<0.f);
  }

  ///\brief
  ///Marks the layer as finished, i.e. no new particles are spawned. As soon as all particles have disappeared, the layer can be destroyed.
  inline void SetFinished() 
  {
    m_bRepeatLifetime=false;
    m_bInfiniteLifeTime=false;
    m_fLifeTime=-1.f;
  }

  ///\brief
  ///Returns the remaining lifetime of the layer in seconds. If the layer has infinite lifetime, this function returns -1.0f
  inline float GetRemainingLifeTime() const
  {
    if (m_bInfiniteLifeTime) 
      return -1.f;
    return m_fLifeTime / m_fTimeScale;
  }

  ///\brief
  ///Returns a new particle from the layer. The result can be NULL
  PARTICLE_IMPEXP ParticleExt_t* GetFreeParticle();
  ///\brief
  ///Cache helper function
  PARTICLE_IMPEXP bool FillFreeParticleCache();

  ///\brief
  ///Simulation function; called by the handling task
  PARTICLE_IMPEXP void HandleParticles(float dtime);

  ///\brief
  ///Simulation function of a single particle
  inline bool HandleSingleParticle(ParticleExt_t *pParticle, float fDeltaTime);

  ///\brief
  ///Respawns all particles
  ///
  ///\param bUseOldCount
  ///  if true, the current number of active particles is used, otherwise the predicted count at spawn time
  PARTICLE_IMPEXP void RespawnAllParticles(bool bUseOldCount);

  ///\brief
  ///   Moves the position of the emitter to the current position of particle group
  ///
  /// The particle system does by default interpolate the emitter position over time. This means that moving
  /// the particle group in one frame will by default still cause particles to be emitted at the old position and
  /// possibly between the old and new position.
  ///
  /// If you want to move a particle effect to a new position, and want to make sure that all newly created
  /// particles of this effect are created at the new position as well, then simply call this method after you have
  /// moved the particle group.
  PARTICLE_IMPEXP void TeleportSpawnPosition();


  ///\brief
  ///Loops through the particle array and counts the alive particles
  PARTICLE_IMPEXP int CountActiveParticles() const;

  ///\brief
  ///Initializes a single new particle (called by the emitter)
  inline void InitSingleParticle(ParticleExt_t *pParticle);

  ///\brief
  ///Copies over properties from source particle
  inline bool CreateParticleFromSource(const ParticleExt_t *pSrcParticle, float fTimeDelta, int iCopyFlags);

  ///\brief
  ///Waits for the simulation task (if there is a task)
  inline void EnsureUpdaterTaskFinished()
  {
    VThreadedTask *pUpdaterTask = GetUpdateTask(); // this is not necessarily m_pHandlingTask
    if (pUpdaterTask != NULL && pUpdaterTask->GetState() != TASKSTATE_UNASSIGNED)
      Vision::GetThreadManager()->WaitForTask(pUpdaterTask, true);
  }

  ///\brief
  ///   Ensures the visibility bounding box is updated.
  ///
  /// \deprecated
  ///   You should use UpdateBoundingBoxes() directly.
  HKV_DEPRECATED_2014_1 inline void UpdateVisibilityBoundingBox() { UpdateBoundingBoxes(); }
    
  /// \brief
  ///   Returns a reference to a bounding box.
  ///
  /// For local space particles (GetUseLocalSpaceMatrix() == true) it is the local space bounding box, otherwise the world space bounding box.
  ///
  /// \deprecated
  ///   Use GetLocalSpaceBoundingBox or GetWorldSpaceBoundingBox instead.
  HKV_DEPRECATED_2014_1 inline const hkvAlignedBBox& BoundingBox() const;

  /// \brief
  ///   Returns a reference to the local space bounding box.
  ///
  /// Note that it may be inaccurate for world space particles (GetUseLocalSpaceMatrix() == true)
  ///
  /// \see
  ///   GetWorldSpaceBoundingBox
  inline const hkvAlignedBBox& GetLocalSpaceBoundingBox() const;

  ///\brief
  ///   Returns a reference to the world space bounding box.
  ///
  /// \see
  ///   GetLocalSpaceBoundingBox
  inline const hkvAlignedBBox& GetWorldSpaceBoundingBox() const;

  ///\brief
  ///   Renders a bounding box.
  ///
  /// Depending on the GetUseLocalSpaceMatrix() either the local space or world space bounding box will be drawn.
  ///
  /// \param iColor
  ///   Color the bounding box is rendered with.
  ///
  /// \deprecated
  ///   Use Vision::Game.DrawBoundingBox together either GetLocalSpaceBoundingBox or GetWorldSpaceBoundingBox.
  HKV_DEPRECATED_2014_1 inline void RenderBoundingBox(VColorRef iColor = V_RGBA_WHITE);
  
  
  ///\brief
  ///   Renders a bounding box for every single particle.
  PARTICLE_IMPEXP void RenderParticleBoundingBoxes();

  ///\brief
  ///Returns the highwater mark in the particle array
  inline int GetActiveParticleCount() const {return m_iValidCount;}

  ///\brief
  ///Prepares some values for faster access
  PARTICLE_IMPEXP void SetPerFrameConstants(float dtime);

  ///\brief
  ///Sets the animation frame and weighting for a single particle
  inline void SetParticleAnimFrame(ParticleExt_t *pParticle, float fFrame, bool bWrapAround = false);

  ///\brief
  ///Sets the visible status of this layer (obsolete since SetVisibleBitmask)
  inline void SetVisible(bool bStatus);

  /// \brief
  ///   Returns the current Distortion Type.
  ///   
  /// \returns
  ///   unsigned char: Current Distortion Type
  inline unsigned char GetDistortionType() const;

  ///\brief
  ///Sets the visible filter bitmask for this layer and for on-destroy children
  PARTICLE_IMPEXP void SetVisibleBitmask(unsigned int iMask);

  ///\brief
  ///   Returns the updated bounding box.
  ///
  /// \deprecated
  ///   Use UpdateBoundingBoxes instead and then use GetLocalSpaceBoundingBox/GetWorldSpaceBoundingBox to get the desired bounding box.
  HKV_DEPRECATED_2014_1 PARTICLE_IMPEXP const hkvAlignedBBox *CalcCurrentBoundingBox();

  /// \brief
  ///   Updates all internal bounding boxes.
  ///
  /// Will update both local and world space bounding boxes.
  /// Depending on the update VisParticleGroupDescriptor_cl::m_fDynamicInflateInterval, InflateBoundingBox will be used (which determines a bounding box around all particles).
  /// Usually it is not necessary to call this function manually, since it is called whenever an option is changed that influences the bounding box.
  ///
  /// \param bImmediateVisibilityBoundingBoxUpdate
  ///   If true the visibility bounding box will be updated immediately, otherwise this action will be delayed. This can be useful if you want to call this function in a seperate thread.
  PARTICLE_IMPEXP void UpdateBoundingBoxes(bool bImmediateVisibilityBoundingBoxUpdate = true);

  ///\brief
  ///Sets simulation flag. If false, the layer is also simulated when it is not visible; If true, the layer will only be simulated when visible.
  inline void SetHandleWhenVisible(bool bStatus) 
  {
    m_bHandleWhenVisible = bStatus;
  }

  ///\brief
  ///Gets simulation flag. If false, the layer is also simulated when it is not visible; If true, the layer is only simulated when it is visible.
  inline bool GetHandleWhenVisible() 
  { 
    return m_bHandleWhenVisible; 
  }

  ///\brief
  ///Returns true if the particle group was rendered in the last frame. Returns false otherwise.
  inline bool WasRecentlyRendered() 
  { 
    return (m_uiLastRenderFrame == Vision::Video.GetFrameCount()-1); 
  }

  ///\brief
  ///Sets an ambient color that might contribute to the per-frame color
  PARTICLE_IMPEXP void SetAmbientColor (VColorRef iColor);

  ///\brief
  ///Re-evaluates the scene brightness, e.g. from the lightgrid
  PARTICLE_IMPEXP void EvaluateSceneBrightness();

  ///\brief
  ///Adds constraints from the passed source list and optionally performs early out using the constraint's Influences function
  ///
  /// Does not work with local space particles!
  ///
  /// \param pSrcList
  ///   List of particle constraints to add.
  ///
  /// \param bCheckInfluence
  ///   If true, constraints outside the particle group's current bounding box will be ignored.
  PARTICLE_IMPEXP int AddRelevantConstraints(const VisParticleConstraintList_cl *pSrcList, bool bCheckInfluence=true);

  ///\brief
  ///Adds a single constraint and optionally performs early out using the constraint's Influences function
  ///
  /// Does not work with local space particles!
  ///
  /// \param pConstraint
  ///   New constraint to add.
  ///
  /// \param bCheckInfluence
  ///   If true, constraints outside the particle group's current bounding box will be ignored.
  PARTICLE_IMPEXP bool AddConstraint(VisParticleConstraint_cl *pConstraint, bool bCheckInfluence=true);

  ///\brief
  ///Detach the passed constraint from this layer
  inline bool RemoveConstraint(VisParticleConstraint_cl *pConstraint);

  ///\brief
  ///Detach all constraints from this layer
  inline void RemoveAllConstraints();

  ///\brief
  ///Perform constraint simulation
  PARTICLE_IMPEXP void HandleAllConstraints(float dtime);

  ///\brief
  ///Return list of all attached constraints
  inline const VisParticleConstraintList_cl* GetConstraintList() const {return &m_Constraints;}
  ///\brief
  ///Copy passed constraint list
  inline void CopyConstraintList(const VisParticleConstraintList_cl* pSrc) {m_Constraints.CopyFrom(pSrc);}

  ///\brief
  ///Sets a wind speed vector. Simple wind can be applied with no performance impact
  PARTICLE_IMPEXP void SetWindSpeed(const hkvVec3& vWind, bool bApplyInLocalSpace=false);

  ///\brief
  ///Debug flag
  inline void SetRenderConstraints(bool bStatus) {m_bRenderConstraints=bStatus;}

  ///\brief
  /// Internal function triggered by the engine callback
  void ReassignShader(bool bRecreateFX);

public:
  V_DECLARE_SERIALX( ParticleGroupBase_cl, PARTICLE_IMPEXP );
  ///\brief
  ///Binary serialization
  PARTICLE_IMPEXP void SerializeX( VArchive &ar );
  PARTICLE_IMPEXP void OnDeserializationCallback(const VSerializationContext &context);

  ///\brief
  ///Set the mesh emitter to the passed mesh
  bool SetMeshEmitterEntity(VisBaseEntity_cl* pEntity);

  ///\brief
  ///Set the SizeMultiplier. If DistortionSizeMode is used then this is the distortion vector. The distortion vector is typically orthogonal to the normal
  ///The w component is not used
  inline void SetSizeMultiplier(const hkvVec4& vSizeMultiplier); 

  ///\brief
  ///Get the SizeMultiplier. If DistortionSizeMode is used then this is the distortion vector. The distortion vector is typically orthogonal to the normal
  ///The w component is not used
  inline hkvVec4 GetSizeMultiplier();

  ///\brief
  /// Sets the topology of the particle group. See VIS_PARTICLE_TOPOLOGY_e.
  inline void SetTopology(VIS_PARTICLE_TOPOLOGY_e eTopology);

  ///\brief
  /// Gets the topology of the particle group. See VIS_PARTICLE_TOPOLOGY_e.
  inline VIS_PARTICLE_TOPOLOGY_e GetTopology() const;
  
  /// \brief
  ///   Returns a hkvVec4* array with up to s_uiNumDynamicLightShaderConstantRegisters elements containing dynamic lighting informations.
  ///   
  /// VisParticleGroup_cl::s_uiNumMobileDynamicLightShaderConstantRegisters hkVec4 if mobile shading is active.
  virtual const hkvVec4* GetDynamicLightingShaderConstants() HKV_OVERRIDE;

  /// \brief
  ///   Returns light sampling offset used for lightgrid sampling and mobile lighting.
  virtual hkvVec3 GetLightSamplingOffset() const HKV_OVERRIDE;

  /// \brief
  ///   Returns shadow map texture of shadowmap used for shadow receive.
  ///  
  /// \returns  NULL if GetUseShadowReceive() returns false or no texture is set.
  /// \sa GetUseShadowReceive()
  virtual VTextureObject* GetShadowReceiveShadowMap() HKV_OVERRIDE;

protected:
  // VisObject3D_cl class overrides
  virtual void OnObject3DChanged(int iO3DFlags) HKV_OVERRIDE;
#ifdef SUPPORTS_SNAPSHOT_CREATION
  virtual void GetDependencies(VResourceSnapshot &snapshot) HKV_OVERRIDE;
#endif
  virtual void ModSysNotifyFunctionCommand(int command, void *param) HKV_OVERRIDE;

private:
  friend class VisParticleEffect_cl;
  friend class VisParticleGroupManager_cl;
  PARTICLE_IMPEXP ParticleGroupBase_cl(VisParticleGroupDescriptor_cl *pDescr, ParticleGroupBase_cl *pParent, int iGeneration, unsigned int uiRandomSeed);
  friend class VisParticleGroupDescriptor_cl;
  friend class VisParticleEmitter_cl;
  friend class VisParticleEffectFile_cl;
  friend class HandleParticlesTask_cl;
  friend class VParticleDesaturationShaderpass;
  friend class VParticleDesaturationManager;

  /// \brief
  ///   Recompute the bounding box using all particles.
  ///
  /// Will directly use the current particle positions. Those are either local or world space.
  PARTICLE_IMPEXP void InflateBoundingBox();

  void InitGroup(VisParticleGroupDescriptor_cl *pDescr, VisParticleEmitter_cl *pEmitter, const hkvVec3& vSpawnPos, const hkvVec3& vOrientation, float fScaling, bool bSpawnParticles, int iGeneration = 0);
  void CopyParentPosition();
  void OnDescriptorChanged();
  void FadeDistancesFromDesc();
  void Finalize();
  void RemoveUpdaterTaskRecursive(ParticleGroupBase_cl *pGroup);

  inline bool AddParticleToCache(ParticleExt_t *pParticle);

  PARTICLE_IMPEXP virtual void UpdateDefaultShaderFlags() HKV_OVERRIDE;

  ParticleGroupBase_cl *m_pParentGroup; ///< no smart pointer because of dead-lock

  hkvVec3 m_vDirection;
  hkvVec3 m_vUpDir;
  hkvVec3 m_vRightDir;                  ///< cached coordinate system (important if cone angle>0)
  float m_fScaling;                     ///< scaling of the group
  float m_fAnimFrameCount;              ///< number of (used) animation frames

  // lookup texture
  VColorRef *m_pColorLookup;            ///< points to m_spColorLookup->GetDataPtr for faster lookup
  VisBitmapPtr m_spColorLookup;         ///< copy of VisParticleGroupDescriptor_cl::m_spColorLookup for faster access
  VCurve2DPtr m_spSizeCurve;            ///< particle size curve [0..1] maximum value = maximum size
  VCurve2DPtr m_spAnimCurve;            ///< particle animation curve

  float m_fColorBitmapSizeX;            ///< bitmap X size for faster lookups
  float m_fColorBitmapSizeY;            ///< bitmap Y size for faster lookups

  // animation
  VIS_PARTICLE_ANIMATION_MODE_e m_eParticleAnimMode; ///< animation mode of particles
  short m_iMaxAnimFrame;                ///< from descriptor m_iUsedAnimFrames-1, similar to m_fAnimFrameCount
  short m_iChildIndex;                  ///< index within parent's collection

  // rotation
  VIS_PARTICLE_ROTATION_MODE_e m_eRotationMode; ///< particle rotation behavior
  VCurve2DPtr m_spRotationCurve;        ///< rotation angle over time, mode PARTICLE_ROTATION_ANGLECURVE

  // lighting
  float m_fLastTimeOfDayUpdate;         ///< stores when the particle was re-lit using the time-of-day to check when to do it again
  bool m_bEvaluateBrightnessNextFrame;  ///< flag to evaluate the brightness next frame
  VColorRef m_InstanceColor;            ///< internal final color result
  VColorRef m_AmbientColor;             ///< color set via VisParticleEffect_cl::SetAmbientColor

  // cache free particles
  short m_iCachedParticleCount, m_iMaxCachedParticleCount;  ///< number of currently cached free particles
  unsigned short *m_piCachedParticle;   ///< pointer to cached particles
  unsigned short m_iCachedParticleNoAlloc[PARTICLECACHESIZE_NOALLOC];

  // group instance vars
  float m_fLifeTime;                    ///< group lifetime
  float m_fTimeScale;                   ///< time scaling value
  float m_fInitialDelay;                ///< delay before simulation starts

  int m_iRemainingParticleCount;        ///< if greater or equal to 0, this group has a restricted amount of particles
  bool m_bInfiniteLifeTime;             ///< indicates infinite lifetime of group
  bool m_bApplyTimeOfDayLight;          ///< if true the time-of-day light will be added to the particles color
  
  bool m_bIsDead;                       ///< marks the group as dead (can be removed if no more references)
  bool m_bPaused;                       ///< marks the group as paused, i.e. no more new particles are spawned
  bool m_bHalted;                       ///< marks the group as halted, i.e. even the particles do not move
  bool m_bUpdateLifetimeIfInvisible;

  hkvVec3 m_vFrameWind;
  hkvVec3 m_vWindSpeed;
  hkvVec3 m_vFrameWindNoInertia;        ///< wind parameters
  float m_fFrameFriction;               ///< the friction multiplier in this frame
  short m_iGeneration;                  ///< recursion depth for recursively created groups (m_spOnDestroyCreateGroup)

  hkvAlignedBBox m_ParticleBoundingBox;   ///< Bounding box around the particle positions, computed by InflateBoundingBox. Note that particle positions are either local or world space!
  hkvAlignedBBox m_WorldSpaceBoundingBox; ///< Absolute world space bounding box.
  hkvAlignedBBox m_LocalSpaceBoundingBox; ///< Local space bounding box.
  bool m_bScheduleVisibilityUpdate;       ///< Visibility update needed.

  float m_fParticleBoundingBoxUpdateTimePos;    ///< current timer for updating the bounding box (related to VisParticleGroupDescriptor_cl::m_fDynamicInflateInterval)

  hkvVec4 m_vSizeMultiplier;            ///< for the distortion size mode

  // group descriptor
  VisParticleEffect_cl *m_pParentEffect;         ///< pointer to the particle effect this groups belongs to
  VisParticleGroupDescriptorPtr m_spDescriptor;  ///< pointer to group descriptor
  ParticleGroupBasePtr m_spOnDestroyCreateGroup; ///< group to create destroyed particles

  // constraints
  bool m_bHandleConstraints;                     ///< indicates whether constraints should be handled
  bool m_bRenderConstraints;                     ///< render constraints for debugging

  unsigned int m_iConstraintAffectBitMask;       ///< Constraint filter bitmask
  VisParticleConstraintList_cl m_Constraints;    ///< List of attached constraints

  // emitter
  VisParticleEmitterPtr m_spEmitter;             ///< Pointer to the emitter
  VisBaseEntity_cl* m_pEmitterMeshEntity;        ///< Temporary variable use when deserialization

  // misc
  bool m_bHandleWhenVisible;                     ///< if enabled, simulation will only be performed if group is visible
  bool m_bMovesWithEmitter;                      ///< internal flag evaluated from SetLocalFactors
  bool m_bRepeatLifetime;                        ///< from descriptor
  bool m_bInertiaAffectsGravity;                 ///< from descriptor

  hkvVec3 m_vOldPos;                             ///< cached old position to calculate the delta when new position is set
  hkvVec3 m_vGroupMoveDelta;
  hkvVec3 m_vGroupMoveDeltaAccum;                ///< internal accumulation vectors
  float m_fLocalFactorStart,m_fLocalFactorDiff;  ///< amount of how much particles move with the emitter at start and end of lifecycle
  volatile int m_iValidCount;                    ///< current number of valid particles

  float m_fTransformationCurveTime, m_fTransformationCurveSpeed;
  VIS_PARTICLE_TOPOLOGY_e m_eTopology;
  bool m_bHasTransformationCurves, m_bHasEvents;
  bool m_bWindInLocalSpace;                      ///< apply wind in local space (not default)

  short m_iTrailIndex;
  
  // task
  HandleParticlesTask_cl *m_pHandlingTask;       ///< pointer to simulation task

  #ifdef _VISION_WIN32
    void* m_pParticleDesaturationGroup;           ///< pointer to particle desaturation group used by simulation package
  #endif
};

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Particles/ParticleGroupBase.inl>

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
