/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file ParticleGroupManager.hpp

#ifndef PARTICLEGROUPMANAGER_HPP_INCLUDED
#define PARTICLEGROUPMANAGER_HPP_INCLUDED

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Particles/ParticleModule.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Particles/ParticleDescriptor.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Particles/ParticleConstraint.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Particles/ParticleGroupBase.hpp>

extern VisParticleGroupManager_cl g_ParticleGroupManager;

class VisParticleEffect_cl;
class VisParticleGroupDescriptor_cl;
class VisParticleGroupManager_cl;
class VisParticleEffectFile_cl;
typedef VSmartPtr<VisParticleEffectFile_cl> VisParticleEffectFilePtr;
typedef VSmartPtr<ParticleGroupBase_cl> ParticleGroupBasePtr;

///\brief
///  Interface that can be installed to supply all particle effects in the scene with a dedicated dynamic wind speed. One global instance can be installed via VisParticleGroupManager_cl::GlobalManager().SetWindHandler.
///
/// The interface function GetWindVelocityAtPosition is called for every particle effect instance (not every single particle!) in each simulation tick.
/// Thus the wind velocity can vary over time and over emitter position (e.g. wind fields).
class IVWindControlHandler
{
public:
  IVWindControlHandler() {}
  virtual ~IVWindControlHandler() {}

  ///\brief
  ///  Key function of this interface which is called per simulation tick and per particle effect instance.
  ///
  /// Note that this function is called asynchronously from inside the particle update threads, so the implementation must gracefully handle this.
  ///
  /// \param vPos
  ///   The pivot position of the particle effect in world space is passed here.
  ///
  /// \return
  ///   The interface must provide the current wind velocity (a 3-dimensional vector specified in units/sec) for the passed position
  virtual hkvVec3 GetWindVelocityAtPosition(const hkvVec3 &vPos) = 0;
};


///\brief
///  This class corresponds to the resource side of particle effects, i.e. each instance corresponds to a particle XML file (or its binary vpfx counterpart respectively).
///
///Effect instances can be created via the VisParticleEffectFile_cl::CreateParticleEffectInstance member function.
///Effect resources are managed by an instance of VisParticleGroupManager_cl. Effects can be loaded through
///VisParticleGroupManager_cl::GlobalManager().LoadFromFile
///
///\see
///  VisParticleEffect_cl
class VisParticleEffectFile_cl : public VManagedResource, public VisTypedEngineObject_cl
{
public:
  ///\brief
  ///  Constructor. Do not use, instead load effects via VisParticleGroupManager_cl::GlobalManager().LoadFromFile
  PARTICLE_IMPEXP VisParticleEffectFile_cl(VisParticleGroupManager_cl *pManager);

  ///\brief
  ///  Destructor
  PARTICLE_IMPEXP virtual ~VisParticleEffectFile_cl();

  ///\brief
  ///  Cast the parent manager to class VisParticleGroupManager_cl
  inline VisParticleGroupManager_cl* GetParentManager() const {return (VisParticleGroupManager_cl *)VManagedResource::GetParentManager();}

  ///\brief
  ///  Internal loading routine
  PARTICLE_IMPEXP bool LoadFromFile(const char *szFilename);

  ///\brief
  ///  Internal saving routine
  PARTICLE_IMPEXP bool SaveToXML(const char *szFilename=NULL);

  PARTICLE_IMPEXP bool LoadFromBinaryFile(IVFileInStream *pIn, bool bCloseFile=true);
  PARTICLE_IMPEXP bool SaveToBinaryFile(IVFileOutStream *pOut, bool bCloseFile=true);

  ///\brief
  ///  Finds a layer descriptor by name
  inline VisParticleGroupDescriptor_cl *FindDescriptor(const char *szName)
  {
    EnsureLoaded();
    return m_Descriptors.FindDescriptor(szName);
  }

  ///\brief
  ///  Obsolete
  inline VisParticleGroupDescriptor_cl *GetDefaultDescriptor() const
  {
    if (m_Descriptors.Count()>0)
      return m_Descriptors.GetAt(0);
    return NULL;
  }

  ///\brief
  ///  Get the local bounding box (merged from all layer descriptors involved)
  PARTICLE_IMPEXP bool GetBoundingBox(hkvAlignedBBox &destBox) const;

  ///\brief
  ///  Internal function. Do not use
  ///\internal
  PARTICLE_IMPEXP void InitParticleEffectInstance(VisParticleEffect_cl *pInstance);
  ///\brief
  ///  Internal function. Do not use
  ///\internal
  PARTICLE_IMPEXP void InitParticleEffectInstance(VisParticleEffect_cl *pInstance, const hkvVec3& vPos, const hkvVec3& vOri, float fScaling, bool bSpawnParticles=true);

  ///\brief
  ///  Creates a new particle effect instance from this resource
  ///
  ///\param vPos
  /// World space position for creation
  ///
  ///\param vOri
  /// Euler angles
  ///
  ///\param uiRandomSeed
  /// Seed for random number generator for this effect
  ///
  ///\return
  /// A new effect instance
  PARTICLE_IMPEXP VisParticleEffect_cl* CreateParticleEffectInstance(const hkvVec3& vPos, const hkvVec3& vOri, unsigned int uiRandomSeed=0);

  ///\brief
  ///  Creates a new particle effect instance from this resource
  ///
  ///\param vPos
  /// World space position for creation
  ///
  ///\param vOri
  /// Euler angles
  ///
  ///\param fScaling
  /// Uniform scaling
  ///
  ///\param uiRandomSeed
  /// Seed for random number generator for this effect
  ///
  ///\return
  /// A new effect instance
  PARTICLE_IMPEXP VisParticleEffect_cl* CreateParticleEffectInstance(const hkvVec3& vPos, const hkvVec3& vOri, float fScaling, unsigned int uiRandomSeed=0);
  
  //serialization
  V_DECLARE_SERIAL_DLLEXP( VisParticleEffectFile_cl,  PARTICLE_IMPEXP );

public:
  // overridden resource functions
  virtual BOOL Reload() HKV_OVERRIDE;
  virtual BOOL Unload() HKV_OVERRIDE;
#ifdef SUPPORTS_SNAPSHOT_CREATION
  virtual void GetDependencies(VResourceSnapshot &snapshot) HKV_OVERRIDE;
#endif
  virtual void AccumulateMemoryFootprint(size_t &iUniqueSys, size_t &iUniqueGPU, size_t &iDependentSys, size_t &iDependentGPU) HKV_OVERRIDE;
  PARTICLE_IMPEXP virtual void Serialize( VArchive &ar ) HKV_OVERRIDE;

protected:
  PARTICLE_IMPEXP VisParticleEffectFile_cl();
  void FinalizeLoading();

  int CountGroupInstances(VisParticleEffect_cl *pInstance, const hkvVec3& vPos, const hkvVec3& vOri, float fScaling, bool bSpawnParticles);

public:
  VisParticleDescriptorList_cl m_Descriptors; ///< List of layer descriptors
  VisParticleConstraintList_cl m_Constraints; ///< List of constraints (not supported)
  VisParticleEmitterList_cl m_Emitter; ///< List of emitters (not supported)
};



///\brief
///  Resource manager class for particle effect file resources
///
///A global instance of this manager can be accessed via VisParticleGroupManager_cl::GlobalManager().
///Loading of effect resources should go via this class
///
///\see
///  VisParticleEffectFile_cl
class VisParticleGroupManager_cl : public VisResourceManager_cl, public IVisCallbackHandler_cl
{
public:
  ///\brief
  ///  Constructor
  PARTICLE_IMPEXP VisParticleGroupManager_cl(const char *szManagerName="ParticleDescriptor");
  ///\brief
  ///  Destructor
  PARTICLE_IMPEXP virtual ~VisParticleGroupManager_cl();

  ///\brief
  ///  Initialization function, called by the plugin initialization
  PARTICLE_IMPEXP void OneTimeInit();
  ///\brief
  ///  Deinitialization function, called by the plugin deinitialization
  PARTICLE_IMPEXP void OneTimeDeInit();

  // 
  // returns pointer to effect file

  ///\brief
  ///  Loads a particle xml file (or its vpfx binary file counterpart) and returns the resource
  ///
  /// Use bForceUnique if you want the effect file instance to be unique so that you can make run-time changes to its descriptor.
  /// Filenames are specified with the .xml extension. This is how they appear in the resource viewer tool. Inside the loading code the loading
  /// however re-directs to the vpfx binary file counterpart if available.
  ///
  ///\param szFilename
  /// Project relative filename of the xml file
  ///
  ///\param bForceUnique
  /// If true, every call of this function returns a unique instance rather than re-using existing resources 
  ///
  PARTICLE_IMPEXP VisParticleEffectFile_cl* LoadFromFile(const char *szFilename, bool bForceUnique = false);

  ///\brief
  /// Static helper function that returns last error string in case loading failed
  static inline const char *GetLastError() {return g_sLastError;}

  ///\brief
  /// Static function to access the global instance of the particle manager
  PARTICLE_IMPEXP static VisParticleGroupManager_cl& GlobalManager();

  ///\brief
  /// Returns the collection of all effect instances
  VisParticleEffectCollection_cl &Instances() {return m_Instances;}

  ///\brief
  /// Returns the collection of constraints that are always executed by all effects but filtered by the m_iAffectBitMask member of the constraints
  VisParticleConstraintList_cl &GlobalConstraints() {return m_GlobalConstraints;}

  ///\brief
  /// This callback is triggered for particle layers that have events. The data object can be casted to VisParticleLayerEventData_cl
  static PARTICLE_IMPEXP VisCallback_cl OnLayerEvent;

  ///\brief
  /// Sets a global time scaling value that is applied to all particle simulations. The default is 1.0f-
  inline void SetGlobalTimeScaling(float fScaling)
  {
    m_fGlobalTimeScaling = fScaling;
  }

  ///\brief
  /// Returns the value that has been set via SetGlobalTimeScaling
  inline float GetGlobalTimeScaling() const
  {
    return m_fGlobalTimeScaling;
  }

  ///\brief
  /// Set a global value that scales fade distances of all effects relatively. The default value is 1.0.
  PARTICLE_IMPEXP void SetGlobalFadeDistanceScaling(float fScale);

  ///\brief
  /// Implements the resource manager's create function
  PARTICLE_IMPEXP virtual VManagedResource *CreateResource(const char *szFilename, VResourceSnapshotEntry *pExtraInfo) HKV_OVERRIDE;

  /// \brief
  ///   Return the respective vpfx file rather than xml
  PARTICLE_IMPEXP virtual const char *GetStreamingReplacementFilename(VResourceSnapshotEntry &resourceDesc, const char *szResolvedFilename, char *szBuffer) HKV_OVERRIDE;

#ifdef _VISION_WIN32
  PARTICLE_IMPEXP static void SetLoopAllEffects(bool bStatus);
  static bool g_bLoopAllEffects; ///< Relevant inside vForge
#endif

  /// \brief
  ///   Installs a global wind handler that affects all particle effects in the scene. See IVWindControlHandler. NULL can be passed for no global wind.
  PARTICLE_IMPEXP void SetGlobalWindHandler(IVWindControlHandler *pWindHandler);

  /// \brief
  ///   Returns the global wind handler that is currently installed. By default this function returns NULL.
  inline IVWindControlHandler *GetGlobalWindHandler() const
  {
    return m_pWindHandler;
  }

  /// \brief
  ///   Enable this to guarantee that all particle effects behave deterministically regardless of their setting. Multichannel applications should enable this.
  ///
  /// The particle simulation itself is deterministic when a fixed time stepper is installed for the simulation updates. Still,
  /// there is a per particle effect layer setting that breaks determinism: If the 'handle only when visible' flag is enabled,
  /// simulation is only performed when the effect is visible in the view. While this potentially saves some performance, it certainly breaks
  /// determinism in a multichannel environment. This global flag disables this optimization for all particle effects that are instantiated
  /// afterwards.
  /// 
  /// \param bStatus
  ///   If true, the 'handle only when visible' flag of all new effect instances is ignored to guarantee deterministic behavior. By default this status is set to false
  inline void SetForceDeterministicSimulation(bool bStatus)
  {
    m_bForceDeterministicSimulation = bStatus;
  }

  /// \brief
  ///   Returns the status that has previously been set via SetForceDeterministicSimulation
  inline bool GetForceDeterministicSimulation() const
  {
    return m_bForceDeterministicSimulation;
  }

protected:
  // overridden IVisCallbackHandler_cl function
  virtual void OnHandleCallback(IVisCallbackDataObject_cl *pData) HKV_OVERRIDE;
  virtual int64 GetCallbackSortingKey(VCallback *pCallback) HKV_OVERRIDE
  {
    if (pCallback==&Vision::Callbacks.OnUpdateSceneFinished)
      return 100; // provide a defined order between other listeners (e.g. VSkeletalBoneProxy) [#4116]
    return IVisCallbackHandler_cl::GetCallbackSortingKey(pCallback);
  }

private:
  friend class VisParticleEffectFile_cl;
  friend class ParticleGroupBase_cl;
  static VString g_sLastError; ///< last error
  VisParticleEffectCollection_cl m_Instances; ///< flat list of all instances

  VisParticleConstraintList_cl m_GlobalConstraints;
  IVWindControlHandler *m_pWindHandler;

  float m_fGlobalTimeScaling;
  float m_fLastToDUpdate;
  bool m_bForceDeterministicSimulation;

  static float g_fGlobalFadeScaling;
};

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
