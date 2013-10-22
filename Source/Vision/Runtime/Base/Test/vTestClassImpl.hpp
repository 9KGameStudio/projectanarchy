/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file vTestClass.hpp

#ifndef H_TEST_BASE_CLASS
#define H_TEST_BASE_CLASS


#include <Vision/Runtime/Base/Types/VType.hpp>

// forward declarations
class VTestUnit;
class IVFileStreamManager;
class VStrList;

#define MAX_SUBTESTS 4096 ///< maximun number of subtests

/// \brief
///   Tests a condition
/// 
/// Tests the condition, if it evaluates to FALSE then the test has failed. An error message is
/// added to the log output
/// 
/// \param condition
///   your expression which you want to test.
/// 
/// \example
///   \code
///   VisEntity_cl *cam = Vision::Game.CreateEntity("VisEntity_cl", VisVector_cl(0,0,0)); 
///   VTEST(cam!=NULL);
///   \endcode
#define VTEST(condition)              { if(AssertTest((condition)!=0,__FILE__,__LINE__)) VDBGBREAK; ANALYSIS_ASSUME(condition) }

/// \brief
///   Tests a float within a certain epsilon
/// 
/// \param value
///   the float that you want to test
/// 
/// \param expected
///   the float that value should be
/// 
/// \param epsilon
///   the allowed epsilon for 'value' to deviate from 'expected'
/// 
/// \example
///   \code
///   VTEST(1.0f, 10.0f / 10.0f, 0.0001f);
///   \endcode
#define VTEST_FLOAT(value, expected, epsilon)              { const float VTEST_FLOAT_f = value; if(AssertTest(((VTEST_FLOAT_f <= (expected) + (epsilon)) && (VTEST_FLOAT_f >= (expected) - (epsilon)))!=0,__FILE__,__LINE__)) VDBGBREAK; ANALYSIS_ASSUME(((VTEST_FLOAT_f <= (expected) + (epsilon)) && (VTEST_FLOAT_f >= (expected) - (epsilon)))!=0) }

/// \brief
///   Tests a condition and returns on failure
/// 
/// Tests the condition, if it evaluates to FALSE then the test has failed. An error message is
/// added to the log output. If the test has failed, \c retval is returned; otherwise, excecution
/// continues.
/// 
/// \param condition
///   your expression which you want to test.
/// \param retval
///   the value to return if the test fails
/// 
/// \example
///   \code
///   VisEntity_cl *cam = Vision::Game.CreateEntity("VisEntity_cl", VisVector_cl(0,0,0)); 
///   VTEST_RETURN(cam!=NULL, FALSE);
///   \endcode
#define VTEST_RETURN(condition, retval) { VBool condEval = ((condition)!= 0); if(AssertTest(condEval,__FILE__,__LINE__)) VDBGBREAK; if (!condEval) return (retval);}

/// \brief
///   Tests the condition and prints out the message if it fails
/// 
/// Tests also the condition, like the VTEST macro, but if it evaluates to FALSE,  the error
/// message AND your message are written into the log.
/// 
/// \param condition
///   your expression
/// 
/// \param message
///   your message for the log 
/// 
/// \example
///   \code
///   VisEntity_cl *scientist = Vision::Game.SearchEntity("Scientist");
///   VTESTM(scientist!=NULL, "Couldn't find Scientist entity!");
///   \endcode
#define VTESTM(condition,message,...) { \
  const bool cond = !!(condition); \
  if(!cond) \
  { \
    /* Ensure that inside the macro a unique name is used, which does not */ \
    /* collide with a variable name used in the scope the macro is used */   \
    VString vTestMessage_##__LINE__; \
    vTestMessage_##__LINE__.Format(message, ##__VA_ARGS__); \
    if(AssertTest((VBool)(cond), __FILE__, __LINE__, vTestMessage_##__LINE__.AsChar())) VDBGBREAK; \
  } }

// general equality test. This macro can be used to compare any two "reasonable" 
// expressions of the same standard types (floats, integers, pointers etc.) or of any
// objects which have the operators == and >> defined. Use this macro only inside the
// VTestClass derived classes.

//You have place the next line in your source if you use the VTESTEQUAL macro below
//#include <sstream>
//This is because of linking conflicts in vMaya

#define VTESTEQUAL( value, expected) \
  { \
  if( !(value == expected) ) \
    { \
    std::ostringstream oss; \
    oss << "Test failed: " << value << " is not equal to " << expected << std::ends; \
    VTESTM( FALSE, oss.str().c_str() ); \
    } \
  }

#define VTESTEQUALSTR( value, expected) \
  { \
    if( !(value == expected) ) \
    { \
      std::ostringstream oss; \
      oss << "Test failed: " << value.AsChar() << " is not equal to " << expected << std::ends; \
      VTESTM( FALSE, oss.str().c_str() ); \
    } \
  }

/// \brief
///   Abstract Test class for single tests containing many subtests
/// 
/// You have to implement the following abstract functions in your class:
/// \li DescribeTest
/// 
/// \li Init
/// 
/// \li InitSubTest
/// 
/// \li RunSubTest
/// 
/// \li DeInitSubTest
/// 
/// \li DeInit
class VTestClass : public VTypedObject
{
  V_DECLARE_DYNCREATE_DLLEXP( VTestClass,VBASE_IMPEXP )
public:   
  
  ///
  /// @name Consturctor/destructor
  /// @{
  ///

  /// \brief
  ///   Inits all member data
  VBASE_IMPEXP VTestClass ();

  /// \brief
  ///   Deletes allocated member data (eg. the subtest strings)
  VBASE_IMPEXP virtual ~VTestClass ();

  ///
  /// @}
  ///

  ///
  /// @name Pure Virtual Functions
  /// @{
  ///

  /// \brief
  ///   Abstract function which describes your tests
  /// 
  /// Override this function to describe this test and set your settings.
  /// 
  /// You implementation must set the test flags for this and  the name of this tests and add sub.
  /// test names.
  /// 
  /// \example
  ///   \code
  ///   void BoneTest::DescribeTest()
  ///   {
  ///     SetTestFlags(1000, TRUE, TRUE); 
  ///     SetTestName ( "Bone test");
  ///     AddSubTest ( "Forward cinematics local rotations");  
  ///     AddSubTest ( "Hide the left upper arm");
  ///     AddSubTest ( "Hide the left upper arm an all its children");
  ///     AddSubTest ( "Rotate the head bone in all Directions -> IncJointRotation");
  ///     ...
  ///   \endcode
  VBASE_IMPEXP virtual void DescribeTest() = 0; 
  
  /// \brief
  ///   Abstract Init function
  /// 
  /// Global init for this tests. You implementation could for example load a map for all the
  /// subtests.
  /// 
  /// \return
  ///   virtual VBool: If the initialization failed, you have to return FALSE
  /// 
  /// \note
  ///   This function is called as the first function. That means before 
  ///   \li InitSubTest()
  /// 
  ///   \li RunSubTest()
  /// 
  ///   \li DeInitSubTest()
  /// 
  ///   \li DeInit()
  /// 
  /// \example
  ///   \code
  ///   VBool BoneTest::Init()
  ///   {
  ///     // Init Engine
  ///     simpleApp = new VisSimpleApp_cl ( m_pConfig->m_iXRes, m_pConfig->m_iYRes, m_pConfig->m_bFullscreen, FALSE, &(m_pConfig->m_tConfig));            
  ///     Vision::SetDataDirectory("Data\\Bones","Data\\globalData");
  ///     Vision::LoadWorld ( "viewermap" );
  ///     Vision::Game.CreateEntity("VisEntity_cl", ...
  ///     ...
  ///     return TRUE;
  ///   }
  ///   \endcode
  VBASE_IMPEXP virtual VBool Init() = 0; 

  /// \brief
  ///   Abstract init function for subtests
  /// 
  /// This functions is called once for each subtest to init subtest data.
  /// 
  /// You can use the VTEST macros to check your initialisation.
  /// 
  /// \param iTest
  ///   Index of the subtest
  /// 
  /// \note
  ///   If you want to report an error in this function use the VTEST macros!
  /// 
  /// \note
  ///   This function is called for each subtest after the function
  ///   \li Init()
  /// 
  /// \note
  ///   But SubTestInit is called before that functions
  ///   \li RunSubTest()
  /// 
  ///   \li DeInitSubTest()
  /// 
  ///   \li DeInit()
  /// 
  /// \example
  ///   \code
  ///   void BumpmapTest::InitSubTest(int iTest)
  ///   {
  ///     ...
  ///     switch (iTest)
  ///     {
  ///       case 0:
  ///       case 2:
  ///         light[0]->SetIntensity(2000); // point light
  ///         cam->RotateToAngle(90, 0, 0);
  ///         break;
  ///       case 1:
  ///         light[1]->SetIntensity(2000); // spotlight
  ///         cam->RotateToAngle(90, 0, 0);
  ///         break;
  ///   ...
  ///   \endcode
  VBASE_IMPEXP virtual void InitSubTest ( int iTest ) = 0;
  
  /// \brief
  ///   Abstract function which executes each sub test.
  /// 
  /// Your implementation has to execute the subtest. Normally you have a if/else or switch/case as
  /// the implementation.
  /// 
  /// Use the VTEST macros to check your test code.
  /// 
  /// Your test has to return FALSE if it is finished. For the return value TRUE the test framework
  /// calls the RunSubTest function again with the same index (this behaviour is used in the engine
  /// test to render more than one frame  for each test).
  /// 
  /// \param iTest
  ///   Index of the subtest
  /// 
  /// \return
  ///   virtual VBool: True if the RunSubTest should be called with the SAME index False if the
  ///   RunSubTest function is finshed with the current subtest
  /// 
  /// \note
  ///   This function is called for each subtest after the function
  ///   \li Init()
  /// 
  ///   \li SubTestInit()
  /// 
  /// \note
  ///   But RunSubTest() is called before that functions
  ///   \li DeInitSubTest()
  /// 
  ///   \li DeInit()
  /// 
  /// \example
  ///   \code
  ///   VBool BumpmapTest::RunSubTest(int iTest)
  ///   {
  ///     if (!simpleApp->Run()) return FALSE;
  ///     if (iTest==0 || iTest==2) // brushes / s3tc
  ///     {
  ///       ...
  ///     }
  ///     if (iTest==1) // spotlight
  ///     {
  ///       ...
  ///     }
  ///     if (iTest==3) // models
  ///     {
  ///   ...
  ///   \endcode
  VBASE_IMPEXP virtual VBool RunSubTest( int iTest ) = 0;

  /// \brief
  ///   Abstract deinit function for each subtest
  /// 
  /// This functions is called once for each subtest to deinit subtest data.
  /// 
  /// You can use the VTEST macros to check your deinitialisation.
  /// 
  /// \param iTest
  ///   Index of the subtest
  /// 
  /// \note
  ///   If you want to report an error in this function use the VTEST macros!
  /// 
  /// \note
  ///   This function is called for each subtest after the function
  ///   \li Init()
  /// 
  ///   \li SubTestInit()
  /// 
  ///   \li RunSubTest()
  /// 
  /// \note
  ///   But DeInitSubTest() is called before that functions
  ///   \li DeInit()
  /// 
  /// \example
  ///   \code
  ///   void BumpmapTest::DeInitSubTest(int iTest)
  ///   {
  ///     if(entity) 
  ///       entity->Remove();
  ///   }
  ///   \endcode
  VBASE_IMPEXP virtual void DeInitSubTest( int iTest ) = 0;

  /// \brief
  ///   Abstract function for the deinitialisation of the whole test
  /// 
  /// Override the function to clean up all your test data.
  /// 
  /// \return
  ///   virtual VBool: FALSE stands for a failed deinit
  /// 
  /// \note
  ///   This function is called after all the functions
  ///   \li Init()
  /// 
  ///   \li SubTestInit()
  /// 
  ///   \li RunSubTest()
  /// 
  ///   \li DeInitSubTest()
  /// 
  /// \example
  ///   \code
  ///   VBool BoneTest::DeInit()
  ///   {
  ///     if (simpleApp) 
  ///       delete simpleApp;
  ///     simpleApp = NULL;
  ///     return TRUE;
  ///   }
  ///   \endcode
  VBASE_IMPEXP virtual VBool DeInit() = 0;

  /// \brief
  ///   Virtual function for checking whether a test is to be skipped if the
  ///   given list of tags is set.
  /// 
  /// Override this function to control test execution based on set tags.
  /// 
  /// \param pTagList
  ///   List of tags that are set for the current test run.
  /// 
  /// \return
  ///   virtual VBool: TRUE if this test should not be executed when the given tags are set.
  /// 
  /// \example
  ///   \code
  ///   VBool DeferredTest::SkipTest( const VStrList* pTagList )
  ///   {
  ///     if ( pTagList->FindString( "FeatureLevel_9_3" ) )
  ///       return TRUE;
  ///     return FALSE;
  ///   }
  ///   \endcode
  VBASE_IMPEXP virtual VBool SkipTest( const VStrList* pTagList ) const;

  /// \brief
  ///   Virtual function for checking whether a subtest is to be skipped if the
  ///   given list of tags is set.
  /// 
  /// Override this function to control subtest execution based on set tags.
  /// 
  /// \param iTestIndex
  ///   Index of the test to be checked for execution.
  ///
  /// \param pTagList
  ///   List of tags that are set for the current test run.
  /// 
  /// \return
  ///   virtual VBool: TRUE if this subtest should not be executed when the given tags are set.
  /// 
  /// \example
  ///   \code
  ///   VBool ShadowMappingTest::SkipSubTest( int iTestIndex, const VStrList* pTagList )
  ///   {
  ///     if ( pTagList->FindString( "FeatureLevel_9_3" ) )
  ///     {
  ///       if ( iTestIndex >= TEST_DEFERRED )
  ///         return TRUE;
  ///     }
  ///     return FALSE;
  ///   }
  ///   \endcode
  VBASE_IMPEXP virtual VBool SkipSubTest( int iTestIndex, const VStrList* pTagList ) const;

  ///
  /// @}
  ///

  ///
  /// @name Helper Functions
  /// @{
  ///

  /// \brief
  ///   Sets the name for this test
  /// 
  /// Call this functino in Init() to give your test a name.
  /// 
  /// \param szName
  ///   name string
  /// 
  /// \example
  ///   \code
  ///   void BoneTest::DescribeTest()
  ///   {
  ///     SetTestName ( "Bone test");
  ///     AddSubTest ( "Forward cinematics local rotations");  
  ///   \endcode
  VBASE_IMPEXP void SetTestName( const char *szName );
  
  /// \brief
  ///   Adds new subtest to your test
  /// 
  /// Pass the name of your subtest to that function to 'create' a new subtest in your test-
  /// 
  /// \param szSubTestName
  ///   string with the name of the subtest
  /// 
  /// \example
  ///   \code
  ///   void BoneTest::DescribeTest()
  ///   {
  ///     SetTestName ( "Bone test");
  ///     AddSubTest ( "Forward cinematics local rotations");  
  ///     AddSubTest ( "Hide the left upper arm");
  ///     ...
  ///   \endcode
  VBASE_IMPEXP void AddSubTest ( const char *szSubTestName );

  VBASE_IMPEXP void AddSubTest ( int testIndex, const char *szSubTestName );
  
  /// \brief
  ///   Sets the boolean for the toggle of subtests (if you pass false subtest can't be disabled)
  VBASE_IMPEXP void SetAllowSubTestToggle( VBool bAllowSubTestToggle = TRUE );
  
  /// \brief
  ///   Returns the VTestUnit in which this test class is registered
  VBASE_IMPEXP VTestUnit* GetTestUnit() const;

  /// \brief
  ///   Returns the test directory for the VTestUnit set with VTestUnit::SetTestDirectory()
  VBASE_IMPEXP const char* GetTestDirectory( bool readDir ) const;

  /// \brief
  ///   the passed filename string and returns the TestDirectory + \\ + filename
  VBASE_IMPEXP const char* BuildTestFilePath( bool useReadDir, const char* szFilename ) const;

  /// \brief
  ///   Asserts the condition.
  /// 
  /// If the condition evaluates to FALSE the file, line  and the message string are printed with
  /// the Printf function.
  /// 
  /// \param condition
  ///   evaluated Condition 
  /// 
  /// \param szFile
  ///   filename (normally __FILE__ is passed)
  /// 
  /// \param iLine
  ///   line number (normally __LINE__ is passed)
  /// 
  /// \param pszMessage
  ///   optional message string
  /// 
  /// \return
  ///   VBool: is TRUE if the test failed AND if assert_on_break is enabled!
  /// 
  /// \note
  ///   Normally you will nopt uses this function directry. This function is used inside of the
  ///   VTEST macros.
  VBASE_IMPEXP VBool AssertTest( VBool condition, const char *szFile, const int iLine, const char *pszMessage=NULL );

  /// \brief
  ///   Like the normal printf, but prints out the string to the choosen output devices.
  /// 
  /// You can choose in the UI between stdout, console and HTML file
  /// 
  /// \param s
  ///   you string with format specifies in it
  /// 
  /// \param ...
  ///   other arguments used in the text
  VBASE_IMPEXP void Printf ( const char *s, ... );
  
  ///
  /// @}
  ///

  ///
  /// @name Functions and Settings describing the Status of the Subtests
  /// @{
  ///

  // set Booleans in m_bSubTestStatus to true
  VBASE_IMPEXP void EnableAllSubtests();
  // set Booleans to false
  VBASE_IMPEXP void DisableAllSubtests();
  // toggle booleans
  VBASE_IMPEXP void ReverseSubtestState();
  // set randomly subtests status
  VBASE_IMPEXP void RandomSubtestState(int iPercent = 50); 

  // reset the data memvers used for statistics
  VBASE_IMPEXP void ResetStatistics();

  // binary comparison of two files
  VBASE_IMPEXP VBool CompareFiles( const char *szName1, const char *szName2, IVFileStreamManager *pManager );

  // compare files, ignoring whitespace characters
  VBASE_IMPEXP VBool CompareFilesSkipWhitespace( const char *szName1, const char *szName2, IVFileStreamManager *pManager );

  // increases errors counter by one
  inline void IncErrors() { m_iErrors++; }

  // increases excpetion counter by one
  inline void IncExceptions() { m_iExceptions++; }

  // increases warning counter by one
  inline void IncWarnings() { m_iWarnings++; }

  // increases assert counter by one
  inline void IncAsserts() { m_iAsserts++; }

  // return test name
  inline const char* GetTestName() const { return m_szTestName; }

  // return sub test name
  inline const char* GetSubTestName(unsigned int uiIndex) const { return m_pszSubTestNames[uiIndex]; }

  // return number of sub tests
  inline int GetNumberOfSubTests() const { return m_iNumberOfTest; }

  // enable or disable test
  inline void SetEnabled( VBool bEnabled ) { m_bEnabled = bEnabled; }

  // returns if test is enabled or not
  inline VBool IsEnabled() const { return m_bEnabled; }

  // returns whether sub test toggling is allowed or not
  inline VBool IsSubTestToggleAllowed() const { return m_bAllowSubTestToggle; }

  // returns the current test
  inline int GetCurrentSubTest() const { return m_iCurrentTest; }

  // enable or disable sub test
  inline void SetSubTestEnabled( unsigned int uiIndex, VBool bEnabled ) { m_bSubTestStatus[uiIndex] = bEnabled; }

  // returns whether the sub test for the given index is enabled or not
  inline VBool IsSubTestEnabled( unsigned int uiIndex ) const { return m_bSubTestStatus[uiIndex]; }

protected:
  int m_iErrors;                         ///< counts total number of errors
  int m_iExceptions;                     ///< counts total number of exceptions that happend
  int m_iWarnings;                       ///< counts total number of warnings that happend
  int m_iAsserts;                        ///< counts total number of assertions that were _tested_ (number of VTEST.. calls)

private:
  // * common members - DescribeTest should fill them in. It is best to use auxiliary functions to do so
  char m_szTestName[256];                ///< name of this test
  char* m_pszSubTestNames[MAX_SUBTESTS]; ///< names of the subtests
  int m_iNumberOfTest;                   ///< number of subtests in test
  VBool m_bRandomOrder;                  ///< sub-tests executed in-order or in random order 
  VBool m_bEnabled;                      ///< this test is completly disabled
  VBool m_bAllowSubTestToggle;           ///< (Deprecated) test allows subtests to be on/off - should be set true. This is just left for compatibility reasons

  // * values that are set for the test by the AutomatedTest function, and taken care of by auxiliary functions. maybe useful for reading
  int m_iCurrentTest;                    ///< the value last passed to iTest

  VBool m_bSubTestStatus[MAX_SUBTESTS];  ///< subtest is on or off

  int m_iExtraInfo;                      ///< added for use in the serializing functions

  VTestUnit* m_pTestUnit;                ///< pointer to the test unit which contains this test

  friend class VTestUnit;

  ///
  /// @}
  ///

};



#endif

/*
 * Havok SDK - Base file, BUILD(#20131019)
 * 
 * Confidential Information of Havok.  (C) Copyright 1999-2013
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
