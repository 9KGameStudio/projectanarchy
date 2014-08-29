/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef H_TEST_AUTOMATION_CLASS
#define H_TEST_AUTOMATION_CLASS

#include <Vision/Runtime/Base/System/IO/Stream/VDiskFileStream.hpp>

#include <Vision/Runtime/Base/Test/vTestClassImpl.hpp>

#define MAX_TESTS 256
#define MAX_CONFIGS 256

#define TEST_ROOT_NAME_READ_BASE "test_read"
#define TEST_ROOT_NAME_WRITE_BASE "test_write"

// forward declarations
class VTestClass;
class VTypeManager;
class VStrList;

/// \brief
///   This class wraps test automation completly
/// 
/// This is the test unit to add, configure and run your tests.
/// 
/// You need one instance in your test project to add your own VTestClass derives classes.
/// 
/// After the adding you can display a nice Windows UI to configure the tests. Afterwards you have
/// to update the object state and then you can run all the activated tests with  the subtests. 
/// 
/// \example
///   \code
///   VTestUnit *tests = new VTestUnit(); // create VTestUnit object
///   tests->SetOutput ( AOUT_WINCONSOLE ); // define where the log messages are displayed
///   tests->RegisterTest ( new VLightTest ); // add first test
///   tests->RegisterTest ( new LoadTextureTest ); // add second test
///   if(SetupDialog(tests)) // display the user interface
///   {
///     tests->Printf(0, AIS_MAINTITLE, "Vision Engine Tests"); // Print out a heading into the log
///     tests->RunAll(); // run all tests
///     tests->OutputStatistics(); // print out some satistics and version nubmers
///   }
///   delete tests; // cleans up everything (ATTENTION: your project must use the same RTL as the VTestUnit module)
///   \endcode
class VTestUnit 
{
public:

  ///
  /// @name Constructor/destructor
  /// @{
  ///
  
  /// \brief
  ///   Constructor, sets the default values for the test uint settings
  VBASE_IMPEXP VTestUnit(const char* szTestDirectory_read, const char* szTestDirectory_write, int partIndex = 1, int partCount = 1, bool bSetAsActive = true);

  /// \brief
  ///   Destructor which deletes all registered VTestClasses.
  VBASE_IMPEXP virtual ~VTestUnit();

  VBASE_IMPEXP static VTestUnit* GetCurrentTestUnit();

  ///
  /// @}
  ///

  ///
  /// @name Important Functions
  /// @{
  ///

  /// \brief
  ///   Adds a new VTestClass derived object to the test suite.
  /// 
  /// The objects is deleted in the destructor of VTestUnit!
  /// 
  /// \param pTest
  ///   pointer to the new VTestClass object
  /// 
  /// \example
  ///   \code
  ///   tests->RegistersTest( new YourTest );
  ///   \endcode
  VBASE_IMPEXP void RegisterTest( VTestClass *pTest );

  /// \brief
  ///   Registers all test classes which are derived from a specified base class
  /// 
  /// \param typeManager
  ///   your typemanager
  /// 
  /// \param pBaseType
  ///   all tests derived from this class will be added
  /// 
  /// \return
  ///   int: number of tests registered
  /// 
  /// \example
  ///   \code
  ///   VTypeManager typeManager;
  ///   typeManager.RegisterModule(&g_baseTestModule);  
  ///   VVERIFY( testUnit.RegisterTestsFromModule( typeManager, V_RUNTIME_CLASS(VTestClass) ) );
  ///   \endcode
  VBASE_IMPEXP int RegisterTestsFromModule(VTypeManager &typeManager, VType *pBaseType);

  /// \brief
  ///   Runs the tests and returns the number of failed tests
  VBASE_IMPEXP int RunAll();
  
  /// \brief
  ///    Advances the tests by one frame, returns false if finished
  VBASE_IMPEXP bool RunTestCallback();

  
  ///
  /// @}
  ///

  ///
  /// @name Logging
  /// @{
  ///



  VBASE_IMPEXP void SetTestSuiteName(const char* szName);

  /// \brief
  ///   Starts the statistic output, calls OutputStatistics, closes the HTML and XML document and
  ///   opens the HTML document (if m_bShowHTML is set)
  VBASE_IMPEXP void FinishOutputDocuments();



public:

  ///
  /// @}
  ///

  ///
  /// @name Enable/disable, Select
  /// @{
  ///
  
  // set all test state (and optionally the subtests states) as enabled
  VBASE_IMPEXP void EnableAll(VBool bSubtestsToo = FALSE);    
  // set all test state (and optionally the subtests states) as disabled
  VBASE_IMPEXP void DisableAll(VBool bSubtestsToo = FALSE);
  // toggle all test state (and optionally the subtest states)
  VBASE_IMPEXP void ReverseState(VBool bSubtestsToo = FALSE);

  // fill the m_iTestOrder array with a growing index
  VBASE_IMPEXP void OrderReset();   
  // reset it and sort the array using the test name
  VBASE_IMPEXP void OrderAlphabetical(VBool bAZ = TRUE); 
  
  ///
  /// @}
  ///

  ///
  /// @name Serialization
  /// @{
  ///

  // Sets the base path for the tests. All files of the tests should uses this directory.
  // eg. the settings file, the html,xml log file are created in that directory!
  VBASE_IMPEXP void SetTestBasePath(bool forRead, const char* szPath);

  // Gets the base directory for the tests. 
  VBASE_IMPEXP const char* GetTestDirectory(bool readDir) const;


  // Builds an absolute, platform-independent path for accessing a file in the test directory for
  // reading/writing.
  VBASE_IMPEXP const char* BuildTestFilePath(bool useReadDir, const char* szFilename) const;

  // Helper to build a native file/folder name for the specified file/folder within the test's read
  // or write file system.
  VBASE_IMPEXP const char* BuildNativeFilePath(bool useReadDir, bool isFolder, const char* szName) const;

  //the framework can save and load the status of each test and subtest and the order of the tests
  //the file is saved into the TestDirectory!
  VBASE_IMPEXP VBool LoadTests(const char *szFileName, bool bUseReadDir=true);
  VBASE_IMPEXP VBool SaveTests(const char *szFileName) const;
  
  //the framework has also some settings like output format and destination, assertions, special engine settings,
  // these settings can be also serialized.
  //the file is saved into the TestDirectory!
  VBASE_IMPEXP VBool LoadSettings(const char *szFileName, bool bUseReadDir=true);
  VBASE_IMPEXP VBool SaveSettings(const char *szFileName) const;

  ///
  /// @}
  ///

  
  // return test by index (the index is for the test-array) (m_pTests[iTestIndex])
  VBASE_IMPEXP VTestClass *GetTest(int iTestIndex) const;
  // return test count (number of elements in the test-array)
  inline int GetTestCount() const { return m_iTests; }

  // return the test index found at specified position in the order-array (m_iTestOrder[iPos])
  VBASE_IMPEXP int GetTestIndexAt(int iPos) const;

  // get the position in the order-array of the testcall pointer 
  VBASE_IMPEXP int GetOrderArrayIndex(VTestClass *pTestClass);
  
  // return -1 if test name not found, else an index into the test array
  VBASE_IMPEXP int FindTestByName(const char* szTestName);
   
  VBASE_IMPEXP VBool MoveTest(int iOldPos, int iNewPos); ///< Moves a test between two position in the order-array and update the other test positions


  VBASE_IMPEXP IVFileOutStream* GetLogFileHTML() 
  { 
    return m_pLogFileHTML; 
  }

  // \brief
  //   
  enum FeatureLevel
  {
    FeatureLevel_Default,   // don't modify any feature level specifiers
    FeatureLevel_11_0,
    FeatureLevel_10_1,
    FeatureLevel_10_0,
    FeatureLevel_9_3,
    FeatureLevel_9_2,
    FeatureLevel_9_1,

    FeatureLevel_Count
  };

  // \brief
  //   Set the feature level to be used for DX11 tests - this is ignored for everything else.
  VBASE_IMPEXP void SetFeatureLevel( FeatureLevel featureLevel );

  // \brief
  //   Retrieve the feature level to be used for DX11 tests. 
  //   On all other builds/platforms this returns FeatureLevel_Default.
  VBASE_IMPEXP FeatureLevel GetFeatureLevel() const;

  // \brief
  //   Should the DX11 feature level selector be displayed for this test unit?
  VBASE_IMPEXP virtual VBool DisplayFeatureLevelSelector() const;

  inline int GetFailedTestCount() const { return m_iFailed; }

  static VTestUnit* m_pActiveTestUnit;

protected:
  friend class CDialogSettings;
  friend class VTestClass;

  /// \brief
  ///   Initializes the test
  /// 
  /// This function calls the VTestClass::Init function and checks for an exception (if exception
  /// handling is enabled)
  /// 
  /// \param pTest
  ///   pointer to the test class
  /// 
  /// \return
  ///   VBool: returns FALSE if an exception was caught. Otherwise TRUE is returned
  virtual VBASE_IMPEXP VBool DoTestInit(VTestClass *pTest);

  /// \brief
  ///   Deinitializes the test
  /// 
  /// This function calls the VTestClass::DeInit function and checks for an exception (if exception
  /// handling is enabled)
  /// 
  /// \param pTest
  ///   pointer to the test class
  /// 
  /// \return
  ///   VBool: returns FALSE if an exception was caught. Otherwise TRUE is returned
  virtual VBASE_IMPEXP VBool DoTestDeInit(VTestClass *pTest);

  /// \brief
  ///   Initializes the subtest
  /// 
  /// This function calls the VTestClass::InitSubTest function and checks for an exception (if
  /// exception handling is enabled).
  /// 
  /// \param pTest
  ///   pointer to the test class
  /// 
  /// \param iSubTest
  ///   number of the subtest
  /// 
  /// \return
  ///   VBool: returns FALSE if an exception was caught. Otherwise TRUE is returned
  virtual VBASE_IMPEXP VBool DoSubTestInit(VTestClass *pTest, int iSubTest);

  /// \brief
  ///   Runs the subtest
  /// 
  /// This function calls the VTestClass::RunSubTest function and checks for an exception (if
  /// exception handling is enabled).
  /// 
  /// This function gets called within the VTestUnit::RunSubTest function until FALSE is returned.
  /// This is e.g. used by the engine tests to run the same subtest for multiple frames.
  /// 
  /// \param pTest
  ///   pointer to the test class
  /// 
  /// \param iSubTest
  ///   number of the subtest
  /// 
  /// \return
  ///   VBool: returns FALSE if an exception was caught. Otherwise the result of
  ///   VTestClass::RunSubTest is returned
  virtual VBASE_IMPEXP VBool DoSubTestRun(VTestClass *pTest, int iSubTest);

  /// \brief
  ///   Deinitializes the subtest
  /// 
  /// This function calls the VTestClass::DeInitSubTest function and checks for an exception (if
  /// exception handling is enabled).
  /// 
  /// \param pTest
  ///   pointer to the test class
  /// 
  /// \param iSubTest
  ///   number of the subtest
  /// 
  /// \return
  ///   VBool: returns FALSE if an exception was caught. Otherwise TRUE is returned
  virtual VBASE_IMPEXP VBool DoSubTestDeInit(VTestClass *pTest, int iSubTest);

  /// \brief
  ///   Adds some statistics to the log window. You can override this function if you want to add
  ///   your own statistics, don't forget to call the base class then ! You also get an overview of
  ///   you used DLL version if you use the engine.
  virtual VBASE_IMPEXP void DoOutputStatistics();

  // private helper functions
  void SwapTestOrder(int iPos1, int iPos2); ///< Swaps to tests in the order-array
  
  // used internally to set the tests position in the order-array
  VBool SetTestPos(int iTestArrayIndex, int iPositionInOrderArray); 

  void HTMLShowDoc ();

  // add automatically generated tags (for the desired feature level, for example)
  void GenerateTags();

  // Platform-independent time function.
  // Returns time in milliseconds.
  unsigned int GetTime() const;
  
  VTestClass *m_pCurrentTest;   ///< pointer to the current test

public:
  //   If skip skipOnFrameFail is enabled, then the test units stops the subtest as soon as the
  //   first error has occured. Usually the subtest is only stopped when RunSubTest returns FALSE.
  //   By default the test unit initializes bSkipOnFrameFail with FALSE.
  VBool m_bSkipOnFrameFail;     ///< skip current subtest as soon as the first frame in the subtest fails

  VBool m_bShowHTML;            ///< if true, the system will open the HTML output file in a window (requires HTML output)
  VBool m_bBreakOnAssert;
  VBool m_bSkipOnSubTestFail;   ///< skip all remaining \b subtests if one subtest failed
  VBool m_bSkipOnTestFail;      ///< skip all remaining \b tests if one subtest failed

  VBool m_bVsync;               ///< wait for vertical sync on each frame
  VBool m_bFastMode;            ///< faster execution of unit tests (if supported)
  VBool m_bHeadlessMode;        ///< execute the tests in headless mode
  VBool m_bGrabRefImages;       ///< grabs reference images for image comparisons

private:
  static bool hkvLogMessageFilter(hkvLogMsgType::Enum MsgType, const char* szText, const char* szTag, void* pPassThrough);

  static void hkvLogWriter_ErrorCounter(hkvLogMsgType::Enum MsgType, const char* szText, int iIndentation, const char* szTag, void* pPassThrough);
  static void hkvLogWriter_TestToHTML(hkvLogMsgType::Enum MsgType, const char* szText, int iIndentation, const char* szTag, void* pPassThrough);
  static void hkvLogWriter_TestToXML(hkvLogMsgType::Enum MsgType, const char* szText, int iIndentation, const char* szTag, void* pPassThrough);

  const VString GetTestSuiteLogName() const;

  void BeginTesting();
  void EndTesting();

  void BeginSuperTest(VTestClass* pTest);
  void EndSuperTest();

  void WriteToXML(bool bAllTests, const char* szText);

  VString m_sTestSuiteName;
  
  // Unfortunately, since we need to have one function call to render each frame (iOS / Android model),
  // we cannot use log blocks as they were meant to be used (by having them destroyed when exiting a scope)
  // instead we need to do this manually -> they are allocated on the heap and destroyed manually
  hkvLogBlock* m_pLastSuperBlock;
  hkvLogBlock* m_pLastSubBlock;

  

  IVFileOutStream* m_pLogFileXML_AllTests;
  IVFileOutStream* m_pLogFileXML_SuperTest;              
  IVFileOutStream* m_pLogFileHTML;


  int m_iTestOrderSeed;
  int m_iTestOrder[MAX_TESTS];
  int m_iTests;                 ///< number of registered tests
  VTestClass *m_pTests[MAX_TESTS]; ///< array of registered pointers to VTestClass objects
  
  int m_iCurrentTest;           ///< index of current executed test
  int m_iCurrentTestIter;       ///< iterator index
  int m_iCurrentSubTestNr;      ///< index of the current subtest (within a test)
  int m_iCurrentSubTestFrameNr; ///< frame number within the current subtest

  int m_iFailed;                ///< number of failed test

  VString m_sRootNameRead;
  VString m_sAbsTestPathRead;
  VString m_sRootNameWrite;
  VString m_sAbsTestPathWrite;

  unsigned int m_uiStartTime;
  unsigned int m_uiTimeTaken;

  // test part index/count (e.g., index=2, count=3 runs the second third of the tests)
  int m_iPartIndex, m_iPartCount;

  // downlevel support on DX11 - this is ignored on everything else
  FeatureLevel  m_featureLevel;


private:
  VStrList *m_pTagList;

  int m_iOverallErrorCount;
  int m_iOverallWarningCount;
  int m_iSuperTestErrorCount;
  int m_iSuperTestWarningCount;
  int m_iSubTestErrorCount;
  int m_iSubTestWarningCount;

  static VTestUnit* s_pCurrentTestUnit;
};

class VTestUtils
{
public:
  VBASE_IMPEXP static bool AddZipFileAsRoot(const char* szZipFileName, const char* szRootName, bool bAddSearchPath);


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
