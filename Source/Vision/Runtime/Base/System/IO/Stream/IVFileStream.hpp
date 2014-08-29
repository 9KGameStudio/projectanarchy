/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file IVFileStream.hpp

#ifndef DC_IVFILESTREAM_HPP
#define DC_IVFILESTREAM_HPP

#include <Vision/Runtime/Base/System/VRefCounter.hpp>
#include <Vision/Runtime/Base/System/IO/Asset/hkvAssetLookUpTable.hpp>
#include <Vision/Runtime/Base/System/IO/System/VFileAccessManager.hpp>
#include <Vision/Runtime/Base/System/Threading/SyncPrimitive/VMutex.hpp>
#include <Vision/Runtime/Base/System/VDateTime.hpp>

#include <Vision/Runtime/Base/Math/hkvMath.h> //for hkvResult
#include <Vision/Runtime/Base/Math/Type/VFloat16.hpp>
#include <Vision/Runtime/Base/System/VNameValueListParser.hpp>


class IVFileInStream;
class IVFileOutStream;
class VFileAccessManager;
struct VFileHandle;

/// \brief
///   Parser that can be used to parse asset metadata strings
typedef VNameValueListParser<',', '=', 1024> VAssetMetadataParser;


/// \brief
///   struct that contains the information attached to an asset
struct VAssetInfo
{
  const char* m_pszRelativePath; ///< relative path to the asset inside the library
  const char* m_pszMetaInfo;     ///< meta information for the asset
  unsigned int m_uiHash;         ///< hash value of path and meta information
};


/// \brief
///   Base class for all input and output streams.
class IVFileStreamBase
{
public:
  /// \brief
  ///   Initializes this file stream, optionally passing the file system that created this stream.
  IVFileStreamBase(IVFileSystem* pFileSystem = NULL)
  : m_spFileSystem(pFileSystem)
  {
  }
private:
  IVFileStreamBase(const IVFileStreamBase&);
  IVFileStreamBase& operator=(const IVFileStreamBase&);
public:
  virtual ~IVFileStreamBase()
  {
  }

public:
  /// \brief
  ///   Returns the file system that created this stream (may be NULL).
  inline IVFileSystem* GetFileSystem() const { return m_spFileSystem; }

  /// \brief
  ///   Returns the name of the file.
  virtual const char* GetFileName() = 0;

  /// \brief
  ///   Sets the absolute path within the VFileAccessManager by which this stream has been opened.
  /// \param szAbsolutePath
  ///   the path to set
  VBASE_IMPEXP void SetAbsolutePath(const char* szAbsolutePath);

  /// \brief
  ///   Sets the absolute path within the VFileAccessManager by which this stream has been opened.
  /// \param szRoot
  ///   the root of path to set
  /// \param szPathBelowRoot
  ///   the part below the root of the path to set
  VBASE_IMPEXP void SetAbsolutePath(const char* szRoot, const char* szPathBelowRoot);

  /// \brief
  ///   Returns the absolute path within the VFileAccessManager by which this stream has been opened.
  /// \return
  ///   the absolute path. This string may be empty if the stream has not been opened via the
  ///   VFileAccessManager.
  VBASE_IMPEXP const char* GetAbsolutePath() const;

  /// \brief
  ///   This function closes the stream and deallocates its data. The object can not be used anymore after this call.
  virtual void Close() = 0;

private:

protected:
  VSmartPtr<IVFileSystem> m_spFileSystem;
  VStaticString<FS_MAX_PATH> m_sAbsolutePath;
};


/// \brief
///   This is the base class for all 'in streams', ie. streams that are used for reading from a data source (e.g. a file or a memory stream).
class IVFileInStream : public IVFileStreamBase
{
public:
  /// \brief
  ///   Initializes this file stream, optionally passing the file system that created this stream.
  IVFileInStream(IVFileSystem* pFileSystem = NULL)
  : IVFileStreamBase(pFileSystem), m_bEOF(true), m_uiLookupHash(0)
  {
  }

  /// \brief
  ///   Destructor
  virtual ~IVFileInStream() {}

  /// \brief
  ///   Reads up to iLen number of bytes to pBuffer. Returns the exact number of bytes that was read (may be smaller than iLen).
  /// 
  /// \param pBuffer
  ///   The destination buffer.
  /// 
  /// \param iLen
  ///   Number of bytes to read.
  /// 
  /// \return
  ///   size_t iRead : Number of actual bytes read.
  virtual size_t Read(void* pBuffer,int iLen) = 0;

  /// \brief
  ///   Read function that performs endianness conversion on the read buffer.
  /// 
  /// This version of the read function calls the virtual Read function and performs the conversion
  /// on the read block.
  /// 
  /// The expected format is passed in the format string.
  /// 
  /// Each character in the format string represents a data type: i=int, s=short, c=char, f=float, q=quad word (8bytes)
  /// For full documentation of the format string see the EndianSwitchWorker() documentation.
  /// 
  /// \param pBuffer
  ///   The destination buffer.
  /// 
  /// \param iLen
  ///   Length of the buffer in bytes, must be large enough to hold the data that the format string specifies (ie. if "f" is used to read a float, the buffer must be at least 4 bytes large).
  /// 
  /// \param pFormat
  ///   Format specification, see EndianSwitchWorker() for more details.
  /// 
  /// \param iRepetitions
  ///   Number of repetitions of the format string.
  /// 
  /// \return
  ///   size_t iRead : Number of actual bytes read.
  /// 
  /// \example
  ///   Reads two integers into a structure:
  ///   \code
  ///   pFile->Read(&header, sizeof(header),"ii", 1);
  ///   \endcode
  VBASE_IMPEXP size_t Read(void* pBuffer, int iLen, const char *pFormat, unsigned int iRepetitions = 1);


  /// \brief
  ///   Sets the byte position where to read/write the file. This function will be removed in the future.
  /// 
  /// \param iPos
  ///   Position relative to base position (iMode)
  /// 
  /// \param iMode
  ///   Base position. Must be VFS_SETPOS_SET, VFS_SETPOS_CURRENT or VFS_SETPOS_END (from the VFilePositionMode enum).
  virtual BOOL SetPos(LONG iPos, int iMode) = 0;

  /// \brief
  ///   Returns the current file position.
  virtual LONG GetPos() = 0;

  /// \brief
  ///   Returns the size of the file in bytes.
  virtual LONG GetSize() = 0;

  /// \brief
  ///   Indicates whether the end of file has been reached.
  bool IsEOF()
  {
    return m_bEOF;
  }

  /// \brief
  ///  Sets the modification date of the file the stream points to.
  VBASE_IMPEXP void SetTimeStamp(const VDateTime& timeStamp);

  /// \brief
  ///  Returns the modification date of the file the stream points to.
  VBASE_IMPEXP bool GetTimeStamp(VDateTime& result) const;

  /// \brief
  ///   Sets the search path in which the asset for this stream was first successfully looked up.
  /// \param szSearchPath
  ///   The search path to set.
  VBASE_IMPEXP void SetInitialSearchPath(const char* szSearchPath);

  /// \brief
  ///   Gets the search path in which the asset for this stream was first
  ///   successfully looked up. If the stream was not opened using a relative path,
  ///   this function returns an empty string, as no look-up has taken place.
  /// \return
  ///   The initial search path, or \c NULL if this stream was not opened without look-up.
  const char* GetInitialSearchPath() const
  {
    return m_sInitialSearchPath;
  }

  /// \brief
  ///   Sets the metadata string to go with this stream.
  /// \param szMetadata
  ///   The metadata string. Can be NULL.
  VBASE_IMPEXP void SetMetadata(const char* szMetadata);

  /// \brief
  ///   Returns the metadata string set for this stream.
  /// \return
  ///   The metadata set for this stream. May be empty, but never \c NULL.
  const char* GetMetadata() const
  {
    return m_sMetaData;
  }

  /// \brief
  ///   Sets the hash of the asset lookup entry that identified the underlying file in the relevant asset lookup table.
  /// \param uiHash
  ///   the hash value to set
  void SetLookupHash(unsigned int uiHash)
  {
    m_uiLookupHash = uiHash;
  }

  /// \brief
  ///   Returns the hash of the asset lookup entry that identified the underlying file in the relevant asset lookup table.
  /// \return
  ///   the hash value (0 if not explicitly set)
  unsigned int GetLookupHash() const
  {
    return m_uiLookupHash;
  }

  ///
  /// @name Read Operators
  /// @{
  ///

  /// \brief
  ///   Reads a char from the stream
  inline IVFileInStream& operator>>(char& ch);

  /// \brief
  ///   Reads a signed char from the stream
  inline IVFileInStream& operator>>(signed char& ch);

  /// \brief
  ///   Reads a bool from the stream
  inline IVFileInStream& operator>>(bool &b);


  /// \brief
  ///   Reads a BYTE from the stream
  VBASE_IMPEXP IVFileInStream& operator>>(BYTE& by);

  /// \brief
  ///   Reads a WORD from the stream
  VBASE_IMPEXP IVFileInStream& operator>>(WORD& w);

  /// \brief
  ///   Reads a DWORD from the stream
  VBASE_IMPEXP IVFileInStream& operator>>(DWORD& dw);

  /// \brief
  ///   Reads a LONG from the stream
  VBASE_IMPEXP IVFileInStream& operator>>(LONG& l);

  /// \brief
  ///   Reads an __int64 from the stream
  VBASE_IMPEXP IVFileInStream& operator>>(__int64& i);

  /// \brief
  ///   Reads an uint64 from the stream
  VBASE_IMPEXP IVFileInStream& operator>>(uint64& i);

  /// \brief
  ///   Reads a VFloat16 from the stream
  VBASE_IMPEXP IVFileInStream& operator>>(VFloat16& h);

  /// \brief
  ///   Reads a float from the stream
  VBASE_IMPEXP IVFileInStream& operator>>(float& f);

  /// \brief
  ///   Reads a double from the stream
  VBASE_IMPEXP IVFileInStream& operator>>(double& d);


  /// \brief
  ///   Reads a short from the stream
  inline IVFileInStream& operator>>(short& w);

  /// \brief
  ///   Reads an int from the stream
  inline IVFileInStream& operator>>(int& i);

  /// \brief
  ///   Reads an unsigned int from the stream
  inline IVFileInStream& operator>>(unsigned int& u);

  ///
  /// @}
  ///

protected:
  VDateTime m_timeStamp;
  bool m_bEOF;
  VString m_sMetaData;
  VString m_sInitialSearchPath;
  unsigned int m_uiLookupHash;
};

inline IVFileInStream& IVFileInStream::operator>>(char& ch)
{ return IVFileInStream::operator>>((BYTE&)ch); }
inline IVFileInStream& IVFileInStream::operator>>(signed char& ch)
{ return IVFileInStream::operator>>((BYTE&)ch); }
inline IVFileInStream& IVFileInStream::operator>>(bool &b)
{ return IVFileInStream::operator>>((BYTE&)b); }

inline IVFileInStream& IVFileInStream::operator>>(short& w)
{ return IVFileInStream::operator>>((WORD&)w); }
inline IVFileInStream& IVFileInStream::operator>>(int& i)
{ return IVFileInStream::operator>>((LONG&)i); }
inline IVFileInStream& IVFileInStream::operator>>(unsigned int& u)
{ return IVFileInStream::operator>>((LONG&)u); }

/// \brief
///   This is the base class for all 'out streams', ie. streams that are used for writing to a data stream (e.g. a file or a memory stream).
class IVFileOutStream : public IVFileStreamBase
{
public:
  IVFileOutStream(IVFileSystem* pFileSystem)
  : IVFileStreamBase(pFileSystem)
  {
  }

  /// \brief
  ///   Destructor
  virtual ~IVFileOutStream() {}

  /// \brief
  ///   Writes up to iLen number of bytes from pBuffer to the stream. Returns the exact number of bytes that were written.
  /// 
  /// \param pBuffer
  ///   The source buffer.
  /// 
  /// \param iLen
  ///   Number of bytes to write.
  /// 
  /// \return
  ///   size_t : Number of actual bytes written.
  virtual size_t Write(const void* pBuffer, size_t iLen) = 0;

  /// \brief
  ///   Write function that performs endianess conversion before writing the data to the stream
  /// 
  /// This version of the write function performs the conversion on a copy of the write block and
  /// then calls the virtual Write function.
  /// 
  /// The expected format is passed in the format string.
  /// 
  /// Each character in the format string represents a data type: i=int, s=short, c=char, f=float, q=quad word (8bytes)
  /// For full documentation of the format string see the EndianSwitchWorker() documentation.
  /// 
  /// \param pBuffer
  ///   Buffer to write (not modified by endian conversion)
  /// 
  /// \param iLen
  ///   Length of the buffer in bytes, must be large enough to hold the data that the format string specifies (ie. if "f" is used to write a float, the buffer must be at least 4 bytes large).
  /// 
  /// \param pFormat
  ///   Format specification, see EndianSwitchWorker() for more details.
  /// 
  /// \param iRepetitions
  ///   Number of repetitions of the format string.
  /// 
  /// \return
  ///   size_t : Number of actual bytes written.
  /// 
  /// \example
  ///   Writes two integers from a structure to the stream:
  ///   \code
  ///   pFile->Write(&header, sizeof(header),"ii", 1);
  ///   \endcode
  VBASE_IMPEXP size_t Write(const void* pBuffer, size_t iLen, const char *pFormat, unsigned int iRepetitions = 1);

  /// \brief
  ///   Makes sure all data is written to disk (not necessarily supported by all file-systems).
  virtual void Flush() = 0;


  /// \brief
  ///   Helper function to write a string to the stream.
  /// 
  /// This function just writes the string content, not the length and also not the 0-termination.
  /// Thus this function is useful for pure text output rather than binary output.
  ///
  /// \param szString
  ///   The string which will be written to the file.
  /// 
  /// \return
  ///   size_t : Number of actual bytes written.
  size_t WriteText(const char* szString)
  {
    if (!szString)
      return 0;

    return Write(szString, strlen(szString));
  }

  ///
  /// @name Write Operators
  /// @{
  ///

public:
  /// \brief
  ///   Writes a char to the stream
  inline IVFileOutStream& operator<<(char ch);

  /// \brief
  ///   Writes a signed char to the stream
  inline IVFileOutStream& operator<<(signed char ch);

  /// \brief
  ///   Writes a bool to the stream
  inline IVFileOutStream& operator<<(bool b);


  /// \brief
  ///   Writes a BYTE to the stream
  VBASE_IMPEXP IVFileOutStream& operator<<(BYTE by);

  /// \brief
  ///   Writes a WORD to the stream
  VBASE_IMPEXP IVFileOutStream& operator<<(WORD w);

  /// \brief
  ///   Writes a DWORD to the stream
  VBASE_IMPEXP IVFileOutStream& operator<<(DWORD dw);

  /// \brief
  ///   Writes a LONG to the stream
  VBASE_IMPEXP IVFileOutStream& operator<<(LONG l);

  /// \brief
  ///   Writes an __int64 to the stream
  VBASE_IMPEXP IVFileOutStream& operator<<(__int64 i);

  /// \brief
  ///   Writes an uint64 to the stream
  VBASE_IMPEXP IVFileOutStream& operator<<(uint64 i);

  /// \brief
  ///   Writes a VFloat16 to the stream
  VBASE_IMPEXP IVFileOutStream& operator<<(const VFloat16& h);

  /// \brief
  ///   Writes a float to the stream
  VBASE_IMPEXP IVFileOutStream& operator<<(float f);

  /// \brief
  ///   Writes a double to the stream
  VBASE_IMPEXP IVFileOutStream& operator<<(double d);


  /// \brief
  ///   Writes a short to the stream
  inline IVFileOutStream& operator<<(short w);

  /// \brief
  ///   Writes an int to the stream
  inline IVFileOutStream& operator<<(int i);

  /// \brief
  ///   Writes an unsigned int to the stream
  inline IVFileOutStream& operator<<(unsigned int u);

  ///
  /// @}
  ///
};

inline IVFileOutStream& IVFileOutStream::operator<<(char ch)
{ return IVFileOutStream::operator<<((BYTE&)ch); }
inline IVFileOutStream& IVFileOutStream::operator<<(signed char ch)
{ return IVFileOutStream::operator<<((BYTE&)ch); }
inline IVFileOutStream& IVFileOutStream::operator<<(bool b)
{ return IVFileOutStream::operator<<((BYTE&)b); }

inline IVFileOutStream& IVFileOutStream::operator<<(short w)
{ return IVFileOutStream::operator<<((WORD&)w); }
inline IVFileOutStream& IVFileOutStream::operator<<(int i)
{ return IVFileOutStream::operator<<((LONG&)i); }
inline IVFileOutStream& IVFileOutStream::operator<<(unsigned int u)
{ return IVFileOutStream::operator<<((LONG&)u); }


/// \brief
///   Class containing helper functions for streams.
class VStreamHelper
{
private:

  /// \brief
  ///   Constructor
  VStreamHelper();

public:

  ///
  /// @name Write and Read String Helpers
  /// @{
  ///


  /// \brief
  ///   Writes a string to file in binary format.
  /// 
  /// That is, writing the length as an int followed by the string characters (without terminating 0).
  /// 
  /// A length of -1 is written if szString==NULL.
  ///
  /// \param pOutStream
  ///   The stream to write to.
  /// 
  /// \param szString
  ///   The string to write.
  /// 
  /// \return
  ///   bool : true, if successful
  VBASE_IMPEXP static bool WriteStringBinary(IVFileOutStream *pOutStream, const char *szString);

  /// \brief
  ///   Reads a string from a binary file
  /// 
  /// The assumed format is the same as in WriteStringBinary.
  /// The returned string must be free'd with VBaseDealloc().
  ///
  /// \param pInStream
  ///   The stream to read from.
  /// 
  /// \return
  ///   char* szString : pointer to a new allocated string (or NULL if length was -1)
  ///
  /// \note
  ///   As all variants of the ReadStringBinary function, this function first reads a 32bit integer that defines the length of the string, where 0 defines an empty string and -1 a NULL string
  ///   Accordingly, all variants of ReadStringBinary are compatible.
  VBASE_IMPEXP static char* ReadStringBinary(IVFileInStream *pInStream);

  /// \brief
  ///   Reads a string from a binary file
  /// 
  /// The assumed format is the same as in WriteStringBinary.
  /// 
  /// The string is written to the destination buffer. A maximum size can be specified.
  /// 
  /// \param pInStream
  ///   The stream to read from.
  /// 
  /// \param szDestBuffer
  ///   The destination buffer to write the string into.
  /// 
  /// \param iMaxCount
  ///   The size of the destination buffer (i.e., the maximum number of bytes to read).
  /// 
  /// \return
  ///   int iLength : the length of the string (-1 if a NULL string was saved)
  ///
  /// \note
  ///   As all variants of the ReadStringBinary function, this function first reads a 32bit integer that defines the length of the string, where 0 defines and empty string and -1 a NULL string
  ///   Accordingly, all variants of ReadStringBinary are compatible.
  VBASE_IMPEXP static int ReadStringBinary(IVFileInStream *pInStream, char *szDestBuffer, int iMaxCount=-1);
  
  /// \brief
  ///   This version of ReadStringBinary reads all bytes and tries to use the passed buffer.
  /// 
  /// \param pInStream
  ///   The stream to read from.
  /// 
  /// \param szDestBuffer
  ///   The destination buffer to write the string into.
  /// 
  /// \param iMaxCount
  ///   The size of the destination buffer (i.e., the maximum number of bytes to read).
  ///
  /// \param bAllocated
  ///   If the bAllocated flag is set after the function returns, the return value has to be free'd with VBaseDeAlloc.
  ///
  /// \returns
  /// char *szRetString: If the string uses not more than iMaxCount bytes (i.e. it fits into szDestBuffer), then the
  /// return value matches the passed buffer.
  ///
  /// \note
  ///   As all variants of the ReadStringBinary function, this function first reads a 32bit integer that defines the length of the string, where 0 defines and empty string and -1 a NULL string
  ///   Accordingly, all variants of ReadStringBinary are compatible.
  VBASE_IMPEXP static char *ReadStringBinary(IVFileInStream *pInStream, char *szDestBuffer, int iMaxCount, bool &bAllocated);

  /// \brief
  ///   WriteLine writes szText into pOutStream and appends a '\n' character.
  /// 
  /// \param pOutStream
  ///   The stream to write to.
  /// 
  /// \param szText
  ///   The text to write to the stream.
  /// 
  /// \returns
  ///   Returns true if the operation was successful.
  VBASE_IMPEXP static bool WriteLine(IVFileOutStream *pOutStream, const char* szText);

  /// \brief
  ///   ReadLine reads characters from pInStream until the first occurrence of a '\\n' character, skipping '\\r' characters.
  ///
  /// \param pInStream
  ///   The stream to read from.
  ///
  /// \param szDestBuffer
  ///   The buffer the function can write to.
  ///
  /// \param iMaxCount
  ///   The size of szDestBuffer. Must be greater than 1 in order to be able to read any characters.
  ///
  /// \returns
  ///   Returns the number of characters written to szDestBuffer, excluding null termination.
  VBASE_IMPEXP static int ReadLine(IVFileInStream* pInStream, char* szDestBuffer, int iMaxCount);

  ///
  /// @}
  ///

};

/// \brief Helper class that takes ownership of a file stream and automatically calls Close() on it as soon as it goes out of scope
template<class T>
class VScopedFileStream
{
public:
  /// \brief constructor
  ///
  /// \param pStream
  ///   The stream to take ownership of
  explicit VScopedFileStream(T* pStream) : 
    m_pStream(pStream)
  { }

  /// \brief default constructor
  VScopedFileStream() :
    m_pStream(NULL)
  { }

  /// \brief destructor, ensures that the given stream gets closed.
  ~VScopedFileStream()
  {
    if(m_pStream != NULL)
      m_pStream->Close();
  }

  /// \brief dereferencing operator
  T* operator -> () const
  {
    return m_pStream;
  }

  /// \brief cast to T*
  operator T*() const
  {
    return m_pStream;
  }

  /// \brief
  ///  Releases the ownership of the stream and returns it
  T* Release()
  {
    T* temp = m_pStream;
    m_pStream = NULL;
    return temp;
  }

private:

  //non-copyable
  void operator = (const VScopedFileStream<T>& rh);
  VScopedFileStream(const VScopedFileStream<T>& other);

  T* m_pStream;
};


#include <Vision/Runtime/Base/System/IO/System/VFileHelper.hpp>

// Initializes file access in VBase
VBASE_IMPEXP void VBase_InitFileManagement();

// Resets the file access manager and asset management data
VBASE_IMPEXP void VBase_DeInitFileManagement();

namespace AssetProfile
{
  /// \brief
  ///   Returns the asset profile name currently being used.
  /// \return
  ///   the name of the asset profile currently being used, or an empty string if no asset profile has been set
  VBASE_IMPEXP const char* GetProfileName();

  /// \brief
  ///   Sets the asset profile to use.
  ///
  /// Calls to this function set the asset profile name and the name of the asset look-up table to use.
  ///
  /// \param szProfileName
  ///   the name of the asset profile. If \c NULL or an empty string is being passed, and there is no profile currently
  ///   set, the profile name as well as the look-up table name are set to the default name for the current
  ///   platform. Otherwise, calls with empty profile names are ignored.
  /// \return
  ///   \c true if the name has been changed; \c false if not
  VBASE_IMPEXP bool SetProfileName(const char* szProfileName);

  /// \brief
  ///   Returns whether an asset profile name has been set.
  /// \return
  ///   \c true if a profile name has been set; \c false if not
  VBASE_IMPEXP bool IsProfileNameSet();

  /// \brief
  ///   Returns the name of the asset look-up table to load.
  ///
  /// If the name has not explicitly set to something else, this returns the same name as GetProfileName().
  ///
  /// \return
  ///   the name of the asset look-up table to load, or an empty string if no asset profile has been set
  VBASE_IMPEXP const char* GetLookUpTableName();

  /// \brief
  ///   Sets the name of the asset look-up table to load.
  ///
  /// Normally, you don't need to call this function, as SetProfileName() also set the name of the look-up
  /// table to the name of the profile. Only call this if you need to load a different look-up table.
  ///
  /// \param szLookUpTableName
  ///   the name of the look-up table to load. If the string is \c NULL or empty, the call to this function
  ///   has no effect.
  VBASE_IMPEXP void SetLookUpTableName(const char* szLookUpTableName);
}

namespace AssetVariantKeys
{
  /// \brief
  ///   Appends a variant key to the list of currently active variant keys.
  ///
  /// If the specified variant key is \c NULL, empty, or already present in the list of active variant
  /// keys, this call has no effect.
  ///
  /// \param key
  ///   the variant key to append.
  VBASE_IMPEXP void Add(const char* key);

  /// \brief
  ///   Removes a variant key of the list of currently active variant keys.
  ///
  /// If the specified variant key is \c NULL, empty, or not present in the list of active variant
  /// keys, this call has no effect.
  ///
  /// \param key
  ///   the variant key to remove.
  VBASE_IMPEXP void Remove(const char* key);

  /// \brief
  ///   Returns the number of asset variant keys currently set.
  /// \return
  ///   the number of asset variant keys currently set
  VBASE_IMPEXP int GetCount();
  
  /// \brief
  ///   Returns a specific asset variant key.
  /// \param iIndex
  ///   the key to return
  /// \return
  ///   the requested key, or \c NULL, if \c iIndex was not valid.
  VBASE_IMPEXP const char* Get(int iIndex);

  /// \brief 
  ///   Clears all set asset variant keys.
  VBASE_IMPEXP void Clear();
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
