/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/HavokPhysicsEnginePluginPCH.h>
#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokCharacterControllerHelpers.hpp>
#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokCharacterController.hpp>

#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokPhysicsModule.hpp>
#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokConversionUtils.hpp>
#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokUserData.hpp>

#include <Physics2012/Collide/Query/CastUtil/hkpLinearCastInput.h>

#if defined(HAVOK_SDK_VERSION_MAJOR) && (HAVOK_SDK_VERSION_MAJOR >= 2012)
#include <Common/Internal/SimplexSolver/hkSimplexSolver.h>
#else
#include <Physics/ConstraintSolver/Simplex/hkpSimplexSolver.h>
#endif

#define DEBUG_HAVOK_CHARACTER_CONTROLLER
#undef DEBUG_HAVOK_CHARACTER_CONTROLLER

//-------------------------------------------------------------------------
// CharacterBestStepCollector

class CharacterBestStepCollector : public hkpCdPointCollector
{
public:

  HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_AGENT, CharacterBestStepCollector);

  CharacterBestStepCollector( )
    : hkpCdPointCollector()
    , m_matchPos(hkVector4::getZero())
    , m_matchDir(hkVector4::getZero())
    , m_useMatch(false)
  {
    reset();
  }

  CharacterBestStepCollector( hkVector4Parameter matchPos, hkVector4Parameter matchDir )
    : hkpCdPointCollector()
    , m_matchPos(matchPos)
    , m_matchDir(matchDir)
    , m_useMatch(true)
  {
    HK_ASSERT2(0x13372008, m_matchDir.isNormalized<3>(), "The match vector must be normalized!");
    reset();
  }

  virtual ~CharacterBestStepCollector( )
  {}

  virtual inline void reset()
  {
    m_hitPoint.m_rootCollidableA = HK_NULL;
    m_hitPoint.m_contact.setDistance( HK_REAL_MAX );
    m_best = m_useMatch ? -1.f : HK_REAL_MAX;
    hkpCdPointCollector::reset();
  }

  hkBool hasHit( ) const
  {
    return m_hitPoint.m_rootCollidableA != HK_NULL;
  }

  const hkpRootCdPoint& getHit() const
  {
    return m_hitPoint;
  }

  const hkContactPoint& getHitContact() const
  {
    return m_hitPoint.m_contact;
  }

protected:

  virtual void addCdPoint( const hkpCdPoint& event )
  {
    // We ignore dynamic bodies all along.
    const hkpRigidBody* body = hkpGetRigidBody( event.m_cdBodyB.getRootCollidable() );
    const bool isDynamic = body && !body->isFixedOrKeyframed();
    if(isDynamic) return;

    // Use special step match method which selects the cp which is on the same half space as (m_matchPos, m_matchDir) and has its normal closest to m_matchDir.
    const hkSimdReal currentBest = hkSimdReal::fromFloat(m_best);
    if(m_useMatch)
    {
      hkVector4 dir; dir.setSub(event.getContact().getPosition(), m_matchPos);
      const hkSimdReal halfSpace = dir.dot<3>(m_matchDir);
      const hkSimdReal cosAng = event.getContact().getNormal().dot<3>(m_matchDir);
      if ( halfSpace.isLessZero() | cosAng.isLessEqual(currentBest) ) return;
      cosAng.store<1>(&m_best);
    }
    // Defaults to closest point wrt distance.
    else
    {
      if(event.getContact().getDistanceSimdReal().isGreaterEqual(currentBest)) return;
      event.getContact().getDistanceSimdReal().store<1>(&m_best);
    }

    // Update the contact point.
    {
      m_hitPoint.m_contact = event.getContact();
      m_hitPoint.m_rootCollidableA = event.m_cdBodyA.getRootCollidable();
      m_hitPoint.m_shapeKeyA = event.m_cdBodyA.getShapeKey();

      m_hitPoint.m_rootCollidableB = event.m_cdBodyB.getRootCollidable();
      m_hitPoint.m_shapeKeyB = event.m_cdBodyB.getShapeKey();
      event.getContact().getDistanceSimdReal().store<1>(&m_earlyOutDistance);
    }
  }

protected:

  const hkVector4& m_matchPos;
  const hkVector4& m_matchDir;
  hkpRootCdPoint m_hitPoint;
  hkReal m_best;
  const bool m_useMatch;
};

//-------------------------------------------------------------------------
// DebugHavokCharacterController

#ifdef DEBUG_HAVOK_CHARACTER_CONTROLLER

#define DEBUG_HAVOK_CHARACTER_CONTROLLER_MAX_NUM_LINES     1024
#define DEBUG_HAVOK_CHARACTER_CONTROLLER_MAX_NUM_CAPSULES  16

class DebugHavokCharacterController
{
public:

  HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_AGENT, DebugHavokCharacterController);

  DebugHavokCharacterController() {}
  virtual ~DebugHavokCharacterController()
  {
    cleanUp();
  }

  void cleanUp()
  {
    m_lines.clear();
    m_capsules.clear();
  }

public:
  struct DebugLine
  {
    DebugLine(hkVector4Parameter pos, hkVector4Parameter vec, bool hit, bool line = false) : m_pos(pos), m_vec(vec), m_hit(hit), m_line(line) {}
    hkVector4 m_pos;
    hkVector4 m_vec;
    bool m_hit;
    bool m_line;
  };
  hkArray<DebugLine> m_lines;

  struct DebugCapsule
  {
    DebugCapsule(hkVector4Parameter pos, bool hit) : m_pos(pos), m_hit(hit) {}
    hkVector4 m_pos;
    bool m_hit;
  };
  hkArray<DebugCapsule> m_capsules;

  void drawDebugLines()
  {
    for(int i = 0; i < m_lines.getSize(); ++i)
    {
      if (i >= DEBUG_HAVOK_CHARACTER_CONTROLLER_MAX_NUM_LINES)
      {
        m_lines.clear();
        break;
      }

      const DebugLine& line = m_lines[i];

      hkvVec3 beg = HK2VIS_VECTOR_SCALED(line.m_pos);
      hkvVec3 end;
      VColorRef col;
      if(line.m_line)
      {
        end = HK2VIS_VECTOR_SCALED(line.m_vec);
        col = line.m_hit ? VColorRef(255, 0, 255) : VColorRef(0, 0, 255);
      }
      else
      {
        Vision::Game.DrawCube(beg, 5.f, VColorRef(225, 225, 225));
        end = beg + HK2VIS_VECTOR_SCALED(line.m_vec);
        col = line.m_hit ? VColorRef(0, 255, 0) : VColorRef(255, 0, 0);
      }
      Vision::Game.DrawSingleLine(beg, end, col);
    }
  }

  void drawDebugCapsules(const vHavokCharacterController* character) 
  {
    for(int i = 0; i < m_capsules.getSize(); ++i)
    {
      if (i >= DEBUG_HAVOK_CHARACTER_CONTROLLER_MAX_NUM_CAPSULES)
      {
        m_capsules.clear();
        break;
      }

      const DebugCapsule& cap = m_capsules[i];

      hkGeometry geom;
      hkTransform t; t.setIdentity();
      t.setTranslation(cap.m_pos);
      hkGeometryUtils::createCapsuleGeometry(VIS2HK_VECTOR_SCALED(character->Character_Top), VIS2HK_VECTOR_SCALED(character->Character_Bottom), VIS2HK_FLOAT_SCALED(character->Capsule_Radius), 4, 4, t, geom);

      //VColorRef color(128, 128, 128);
      VColorRef color = cap.m_hit ? VColorRef(128, 228, 128) : VColorRef(228, 128, 128);
      for(int t = 0; t < geom.m_triangles.getSize(); ++t)
      {
        hkvVec3 v0 = HK2VIS_VECTOR_SCALED(geom.getVertex(t, 0));
        hkvVec3 v1 = HK2VIS_VECTOR_SCALED(geom.getVertex(t, 1));
        hkvVec3 v2 = HK2VIS_VECTOR_SCALED(geom.getVertex(t, 2));

        Vision::Game.DrawSingleLine(v0, v1, color);
        Vision::Game.DrawSingleLine(v1, v2, color);
        Vision::Game.DrawSingleLine(v2, v0, color);
      }
    }
  }
};

//-------------------------------------------------------------------------
// vHavokCharacterPushableProxy

#define DEBUG_ADD_VISION_LINE(pos, vec, hit, line) //m_visDebug->m_lines.pushBack(DebugHavokCharacterController::DebugLine(pos, vec, hit, line))
#define DEBUG_ADD_VISION_LINE2(pos, vec, hit, line) m_visDebug->m_lines.pushBack(DebugHavokCharacterController::DebugLine(pos, vec, hit, line))
#define DEBUG_ADD_VISION_CAPSULE(pos, hit) //m_visDebug->m_capsules.pushBack(DebugHavokCharacterController::DebugCapsule(pos, hit))
#define DEBUG_DRAW_VISION_LINES() m_visDebug->drawDebugLines()
#define DEBUG_DRAW_VISION_CAPSULES(character) m_visDebug->drawDebugCapsules(character)
#define DEBUG_DRAW_CREATE() m_visDebug = new DebugHavokCharacterController()
#define DEBUG_DRAW_DESTROY() delete m_visDebug; m_visDebug = HK_NULL
#else
#define DEBUG_ADD_VISION_LINE(pos, vec, hit, line)
#define DEBUG_ADD_VISION_LINE2(pos, vec, hit, line) 
#define DEBUG_ADD_VISION_CAPSULE(pos, hit) 
#define DEBUG_DRAW_VISION_LINES() 
#define DEBUG_DRAW_VISION_CAPSULES(character) 
#define DEBUG_DRAW_CREATE() 
#define DEBUG_DRAW_DESTROY() 
#endif

vHavokCharacterPushableProxy::vHavokCharacterPushableProxy( const hkpCharacterProxyCinfo& info, const vHavokCharacterController *pOwner, hkReal strength )
  : hkpCharacterProxy(info)
  , m_strength(strength)
  , m_onStepCnt(0)
  , m_visDebug(HK_NULL)
  , m_fCurrentTime(0.0f)
  , m_pushedRigidBodies()
{
  // Set user data to identify this Havok component during collision detection
  info.m_shapePhantom->setUserData((hkUlong)vHavokUserDataPointerPair_t::CombineTypeAndPointer((void*)pOwner, V_USERDATA_OBJECT));

  this->addCharacterProxyListener(this);
  DEBUG_DRAW_CREATE();
}

vHavokCharacterPushableProxy::~vHavokCharacterPushableProxy()
{
  DEBUG_DRAW_DESTROY();
}

void vHavokCharacterPushableProxy::drawDebug(const vHavokCharacterController* character) const
{
  DEBUG_DRAW_VISION_LINES();
  DEBUG_DRAW_VISION_CAPSULES(character);
#ifdef DEBUG_HAVOK_CHARACTER_CONTROLLER
  // Draw capsule
  if(1)
  {
    hkGeometry geom;
    hkTransform t; t.setIdentity();
    t.setTranslation(VIS2HK_VECTOR_SCALED(character->GetPosition()));
    hkGeometryUtils::createCapsuleGeometry(VIS2HK_VECTOR_SCALED(character->Character_Top), VIS2HK_VECTOR_SCALED(character->Character_Bottom), VIS2HK_FLOAT_SCALED(character->Capsule_Radius), 6, 6, t, geom);

    //VColorRef color(128, 128, 128);
    VColorRef color = isOnStep() ? VColorRef(128, 228, 228) : VColorRef(228, 228, 228);
    for(int t = 0; t < geom.m_triangles.getSize(); ++t)
    {
      hkvVec3 v0 = HK2VIS_VECTOR_SCALED(geom.getVertex(t, 0));
      hkvVec3 v1 = HK2VIS_VECTOR_SCALED(geom.getVertex(t, 1));
      hkvVec3 v2 = HK2VIS_VECTOR_SCALED(geom.getVertex(t, 2));

      Vision::Game.DrawSingleLine(v0, v1, color);
      Vision::Game.DrawSingleLine(v1, v2, color);
      Vision::Game.DrawSingleLine(v2, v0, color);
    }
  }
#endif
}

bool vHavokCharacterPushableProxy::handleSteps( const vHavokCharacterController* character, const hkpCharacterInput& input, hkpCharacterOutput& output )
{
  // Verify that any change may take place
  if( character->GetCharacterContext()->getState() != HK_CHARACTER_ON_GROUND || output.m_velocity.lengthSquared3() <= hkSimdReal_EpsSqrd )
  {
    m_onStepCnt = 0;
    return false;
  }

  // Simple onStep delay
  if(m_onStepCnt > 0)
  {
    --m_onStepCnt;
  }

  hkSimdReal threshold = hkSimdReal::fromFloat( hkvMath::cosRad ( hkvMath::Deg2Rad(character->Step_Threshold) ) );
  threshold.setClamped(threshold, hkSimdReal_0, hkSimdReal_1);

  // 1) Detect step candidates
  // 2) Find the closest non-penetrating contact manifold with a normal opposite to the velocity
  int index = -1;
  {
    hkSimdReal dist = hkSimdReal_Max;
    for(int i = 0; i < m_manifold.getSize(); ++i)
    {
      const hkpRootCdPoint& manifold = m_manifold[i];
      bool isCharacterA;
      {
        const hkpPhantom* test = hkpGetPhantom( manifold.m_rootCollidableA );
        isCharacterA = test == m_shapePhantom;
      }
      const hkpRigidBody* body = hkpGetRigidBody( isCharacterA ? manifold.m_rootCollidableB : manifold.m_rootCollidableA );
      const bool isDynamic = body && !body->isFixedOrKeyframed();
      // Consider only static collision objects
      if(!isDynamic)
      {
        const hkSimdReal vert = manifold.m_contact.getNormal().dot<3>(m_up);
        //const bool vertical = manifold.m_contact.getDistance() > 0 && vert >= hkSimdReal_0 && vert < threshold;
        const hkBool32 vertical = vert.isGreaterEqualZero() && vert.isLess(threshold);
        // Step candidate found
        if(vertical)
        {
          if( manifold.getContact().getDistanceSimdReal().isLess(dist) && manifold.getContact().getNormal().dot<3>(output.m_velocity).isLessZero() )
          {
            index = i;
            dist = manifold.getContact().getDistanceSimdReal();
          }
        }

        // Debug
        if(0)
        {
          hkvVec3 beg; vHavokConversionUtils::PhysVecToVisVecWorld(manifold.m_contact.getPosition(),beg);
          hkvVec3 end; vHavokConversionUtils::PhysVecToVisVecLocal(manifold.m_contact.getNormal(),end); end += beg;
          Vision::Game.DrawSingleLine(beg, end, VColorRef((vertical ? 255 : 0), (vertical ? 0 : 255), 0), 10.f);
          Vision::Game.DrawCube(beg, 5.f, VColorRef((vertical ? 255 : 50), (vertical ? 50 : 255), 50));
        }
      }
    }
  }
  // If no step candidate that met the requirements was found then we're done!
  if(index < 0)
  {
    return false;
  }

  const hkpRootCdPoint& cp = m_manifold[index];

  const hkTransform& trans = m_shapePhantom->getTransform();

  DEBUG_ADD_VISION_LINE(cp.getContact().getPosition(), cp.getContact().getNormal(), false, false);

  // We backup the current position as we need to put back the character to this place if the step candidate was unsuccessful.
  const hkVector4 currentPosition = trans.getTranslation();

  hkSimdReal slope = hkSimdReal::fromFloat( hkvMath::cosRad ( hkvMath::Deg2Rad(character->Max_Slope) ) ); slope.setClamped(slope,hkSimdReal_0,hkSimdReal_1);
  hkSimdReal stepHeight = hkSimdReal::fromFloat( character->Step_Height ); stepHeight.mul(vHavokConversionUtils::GetVision2HavokScaleSIMD());
  hkSimdReal radius = hkSimdReal::fromFloat( character->Capsule_Radius ); radius.mul(vHavokConversionUtils::GetVision2HavokScaleSIMD());

  // Extra contact point validations checks.
  {
    hkVector4 ground;
    vHavokConversionUtils::VisVecToPhysVecLocal(character->Character_Bottom, ground);
    ground.add(currentPosition);
    ground.subMul(m_up, radius);

    hkVector4 diff; diff.setSub(cp.getContact().getPosition(), ground);
    const hkSimdReal height = m_up.dot<3>(diff);

    // Slope validation
    {
      // Get the true surface normal from the contact point to determine if this is really a slope.
      {
        hkpWorldRayCastInput input;
        {
          // From is inside the capsule at the same hight as the contact point
          input.m_from.setAddMul(ground, m_up, height);
          // To is inside the hit body 5 % (of the distance) along the hit reverse normal.
          input.m_to.setSubMul(cp.getContact().getPosition(), cp.getContact().getNormal(), hkSimdReal::fromFloat(0.05f)*cp.getContact().getDistanceSimdReal());
        }

        hkpWorldRayCastOutput hit;
        vHavokPhysicsModule::GetInstance()->GetPhysicsWorld()->castRay(input, hit);
        if(hit.hasHit())
        {
          const hkSimdReal angle = hit.m_normal.dot<3>(m_up);
          // Detect if we are on a slope and do not need to reduce the velocity
          if(angle >= slope)
          {
            m_onStepCnt = 0;
            return false;
          }
        }
      }
    }

    // Step_Height validation test, we need to make sure we climb at maximum Step_Height.
    // Note: this step will often remove false positives such as walls, and thus speed up performance.
    {
      // If not climbable then adjust velocity to prevent the step climbing.
      if(height > stepHeight)
      {
        // Project cp normal to the plane perpendicular to the up direction.
        hkVector4 normal = cp.getContact().getNormal();
        normal.subMul(m_up, m_up.dot<3>(normal));
        normal.normalize<3>();

        DEBUG_ADD_VISION_LINE(ground, m_up, false, false);

        // Cancel velocity in the forward normal direction
        output.m_velocity.subMul(normal, output.m_velocity.dot<3>(normal));
        // Cancel velocity in the upward direction
        output.m_velocity.subMul(m_up, output.m_velocity.dot<3>(m_up));

        return false;
      }
    }
  }

  // Start position is lifted Step_Height upwards.
  hkVector4 from; from.setAddMul(currentPosition, m_up, stepHeight);
  // Forward sweep magnitude depends on velocity and timestep, and the predefined keepDistance controller parameter.
  hkSimdReal keepDistance = hkSimdReal::fromFloat( m_keepDistance );
  //horiHelp.setZero();
  hkVector4 forward; forward.setMul(input.m_forward, keepDistance + output.m_velocity.length<3>()*hkSimdReal::fromFloat(input.m_stepInfo.m_deltaTime));
  hkVector4 to; to.setAdd(from, forward);

  hkpLinearCastInput lci;
  {
    lci.m_startPointTolerance = m_keepDistance + m_keepContactTolerance;
    lci.m_maxExtraPenetration = 0.01f;
    lci.m_to = to;
  }

  // 1. FORWARD SWEEP TEST (this will eliminate all walls, etc.)
  // If something is hit during the forward sweep we cannot climb the step (or it might not be a step at all).
  {
    CharacterBestStepCollector hit;
    m_shapePhantom->setPositionAndLinearCast(from, lci, hit, HK_NULL);
    if(hit.hasHit())
    {
      m_shapePhantom->setPosition(currentPosition);
      DEBUG_ADD_VISION_CAPSULE(from, false);
      DEBUG_ADD_VISION_LINE(hit.getHitContact().getPosition(), hit.getHitContact().getNormal(), false, false);
      return false;
    }
    DEBUG_ADD_VISION_LINE(from, to, false, true);
  }

  // 2. UPWARD SWEEP TEST (this is merely used to prevent escaping small shaft, etc.)
  // If something is hit during the upward sweep we cannot reach the step.
  {
    CharacterBestStepCollector hit;
    lci.m_to = from;
    m_shapePhantom->setPositionAndLinearCast(currentPosition, lci, hit, HK_NULL);
    if(hit.hasHit())
    {
      DEBUG_ADD_VISION_LINE(hit.getHitContact().getPosition(), hit.getHitContact().getNormal(), false, false);
      return false;
    }
  }

  // 3. DOWNWARD SWEEP TEST
  hkVector4 newPos = currentPosition;
  bool handledStep = false;
  {
    // end position is lowered Step_Height downwards.
    from = to;
    to.setSubMul(from, m_up, stepHeight);
    lci.m_to = to;
    // The match point should be a below the actual step candidate to cater for new cp adjacent to the candidate.
    //hkVector4 match; match.setSubMul(cp.getContact().getPosition(), m_up, 0.1f*stepHeight);
    //CharacterBestStepCollector hit(match, m_up);
    CharacterBestStepCollector hit;
    m_shapePhantom->setPositionAndLinearCast(from, lci, hit, HK_NULL);

    DEBUG_ADD_VISION_LINE(from, to, false, true);

    if(hit.hasHit())
    {
      // New additional ray check to identify if the character is allowed to be on step using the true surface normal.
      bool horizontal = false;
      {
        hkpWorldRayCastInput input;
        {
          input.m_from.setAddMul(hit.getHitContact().getPosition(), m_up, keepDistance);
          input.m_to.setSubMul(hit.getHitContact().getPosition(), hit.getHitContact().getNormal(), keepDistance);
        }

        hkpWorldRayCastOutput slopeHit;
        vHavokPhysicsModule::GetInstance()->GetPhysicsWorld()->castRay(input, slopeHit);
        if(slopeHit.hasHit())
        {
          // Detect if we are on a slope and not a step (if contact normal is the same as the hit normal!)
          const hkSimdReal angle = slopeHit.m_normal.dot<3>(m_up);
          if(angle > slope)
          {
            horizontal = true;
          }
        }
        DEBUG_ADD_VISION_LINE2(input.m_from, input.m_to, horizontal, true);
      }

      if(0)	// old check
      {
        hkSimdReal hori = hkSimdReal_1 - hit.getHitContact().getNormal().dot<3>(m_up);
        horizontal = hit.getHitContact().getDistance() > 0 && hori >= hkSimdReal_0 && hori < threshold;
        DEBUG_ADD_VISION_LINE(hit.getHitContact().getPosition(), hit.getHitContact().getNormal(), horizontal, false);
      }
      DEBUG_ADD_VISION_LINE(hit.getHitContact().getPosition(), hit.getHitContact().getNormal(), horizontal, false);

      const hkSimdReal s = hit.getHitContact().getDistanceSimdReal();
      hkVector4 tmpPos; tmpPos.setMul(from, hkSimdReal_1-s);
      tmpPos.addMul(to, s);

      if(horizontal)
      {
        newPos = tmpPos;
        // Correct for the previously added keepDistance
        newPos.subMul(input.m_forward, keepDistance);
        // The velocity is reduced according to the surface normal even though the projection does cover the entire position time integration.
        output.m_velocity.setZero();
        //output.m_velocity.mul(hori);
        // We force the character to be on a step for 10 updates, this value is chosen experimentally and is bound to change.
        m_onStepCnt = 10;
        handledStep = true;
      }
      else
      {
        DEBUG_ADD_VISION_CAPSULE(tmpPos, horizontal);
      }
    }
  }

  m_shapePhantom->setPosition(newPos);
  return handledStep;
}

void vHavokCharacterPushableProxy::processConstraintsCallback(const hkpCharacterProxy* proxy, 
  const hkArray<hkpRootCdPoint>& manifold, hkpSimplexSolverInput& input)
{
  VASSERT(this == proxy);
  vHavokUserDataType_e type;
  vHavokCharacterController* pOwner = static_cast<vHavokCharacterController*>(
    vHavokUserDataPointerPair_t::ExtractTypeAndPointer((void*)m_shapePhantom->getUserData(), type));
  VASSERT(pOwner != NULL);

  for (int i = 0; i < manifold.getSize(); i++)
  {
    // Ground support on other character controllers.
    hkpPhantom* otherPhantom = hkpGetPhantom(manifold[i].m_rootCollidableB);
    if (otherPhantom != NULL)
    {
      vHavokUserDataType_e eType = V_USERDATA_UNDEFINED;
      void *pObject = vHavokUserDataPointerPair_t::ExtractTypeAndPointer((void*)otherPhantom->getUserData(), eType);
      if (pObject && eType == V_USERDATA_OBJECT)
      {
        const vHavokCharacterPushableProxy* otherCharacter = ((vHavokCharacterController*)pObject)->GetCharacterProxy();
        if (otherCharacter != NULL)
        {
          const hkSimdReal strength = hkSimdReal::fromFloat(m_strength);
          const hkSimdReal relativeStrength = strength / (strength + hkSimdReal::fromFloat(otherCharacter->m_strength));
          input.m_constraints[i].m_velocity.addMul(relativeStrength, getLinearVelocity());
          input.m_constraints[i].m_velocity.addMul((hkSimdReal_1 - relativeStrength), otherCharacter->getLinearVelocity());
        }
      }
    }
    // We don't want to get back the velocity induced by an object pushed away.
    // The following code reduces this effect.
    else if (!m_pushedRigidBodies.isEmpty())
    {
      // Ground support with rigid bodies.
      hkpRigidBody* pRigidBody = hkpGetRigidBody(manifold[i].m_rootCollidableB);
      if (pRigidBody != NULL)
      {
        int iIndex = getIndexOfPushedRigidBody(pRigidBody);
        if (iIndex != -1)
        {
          const PushedRigidBody& pushInfo = m_pushedRigidBodies[iIndex];

          // Make sure only the original velocity (not affected by the object interaction)
          // is taken into account by maintaining the direction of the velocity used for the ground support.
          hkSimdReal d = input.m_constraints[i].m_velocity.dot3(pushInfo.m_orgVelocityDir);
          d.setClamped(d, hkSimdReal::fromFloat(0.0f), pushInfo.m_orgVelocityLen);
          input.m_constraints[i].m_velocity.setMul(d, pushInfo.m_orgVelocityDir);

          // Smooth out the feedback further by incorporating the mass ratio.
          VASSERT(pRigidBody->getMass() > 0.0f);
          const float fMassRatioFactor = 4.0f;
          float fMassRatio = hkvMath::saturate(fMassRatioFactor * pRigidBody->getMass() / pOwner->Character_Mass);
          fMassRatio *= fMassRatio;
          input.m_constraints[i].m_velocity.mul4(hkSimdReal::fromFloat(fMassRatio));
        }
      }
    }
  }
}

void vHavokCharacterPushableProxy::objectInteractionCallback(hkpCharacterProxy* proxy, 
  const hkpCharacterObjectInteractionEvent& input, hkpCharacterObjectInteractionResult& output)
{
  if (input.m_body == HK_NULL || hkvMath::Abs(input.m_objectImpulse) < HKVMATH_DEFAULT_EPSILON)
    return;
  
  // Do not add the entry if it is present already. 
  // Otherwise we might incorporate the velocity induced by the last interaction.
  if (getIndexOfPushedRigidBody(input.m_body) == -1)
  {
    PushedRigidBody pushInfo;
    pushInfo.m_pRigidBody = input.m_body;
    pushInfo.m_fTimeStamp = m_fCurrentTime;
    pushInfo.m_orgVelocityDir = input.m_body->getLinearVelocity();
    pushInfo.m_orgVelocityLen = pushInfo.m_orgVelocityDir.normalizeWithLength3();
    m_pushedRigidBodies.pushBack(pushInfo);
  }
}

void vHavokCharacterPushableProxy::clearPushedRigidBodies()
{
  const float fTimeOut = 0.1f;

  // We need to keep the pushed rigid bodies for some time since there might be a chain of
  // object interaction events with the same object in consecutive simulation steps.
  for (int i = 0; i < m_pushedRigidBodies.getSize(); i++)
  {
    if (m_pushedRigidBodies[i].m_fTimeStamp + fTimeOut < m_fCurrentTime)
    {
      m_pushedRigidBodies.removeAt(i);
      i--;
    }
  }
}

int vHavokCharacterPushableProxy::getIndexOfPushedRigidBody(const hkpRigidBody* pRigidBody) const
{
  for (int i = 0; i < m_pushedRigidBodies.getSize(); i++)
  {
    if (m_pushedRigidBodies[i].m_pRigidBody == pRigidBody)
      return i;
  }

  return -1;
}

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
