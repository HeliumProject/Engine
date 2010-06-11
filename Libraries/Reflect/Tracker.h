#pragma once

#include "Platform/Types.h"
#include "Foundation/Memory/SmartPtr.h"
#include "Debug/Symbols.h"

#include "API.h"

#ifdef REFLECT_OBJECT_TRACKING

namespace Reflect
{
  //
  // Stack record captures stack addresses
  //

  class StackRecord : public Nocturnal::RefCountBase<StackRecord>
  {
  public:
    Debug::V_DWORD64    m_Stack;
    std::string         m_String;
    bool                m_Converted;

    StackRecord()
      : m_Converted ( false )
    {
      m_Stack.reserve( 30 );
    }

    const std::string& Convert();  
  };

  typedef Nocturnal::SmartPtr< StackRecord > StackRecordPtr;
  typedef std::vector< StackRecordPtr > V_StackRecordPtr;
  typedef std::map< Debug::V_DWORD64, StackRecordPtr > M_StackRecord;


  //
  // Creation record stores object information
  //

  class CreationRecord
  {
  public:
    PointerSizedUInt    m_Address;
    std::string         m_ShortName;
    int                 m_Type;

    StackRecordPtr m_CreateStack;
    StackRecordPtr m_DeleteStack;

    CreationRecord();

    CreationRecord(PointerSizedUInt ptr);

    void Dump(FILE* f);
  };

  typedef std::map<PointerSizedUInt, CreationRecord> M_CreationRecord;


  //
  // Tracker object
  //

  class Tracker
  {
  public:
    M_CreationRecord m_CreatedObjects;
    M_CreationRecord m_DeletedObjects;
    M_StackRecord    m_Stacks;

    Tracker();
    virtual ~Tracker();

    // make a stack record
    StackRecordPtr GetStack();

    // save debug info during creation
    void Create(PointerSizedUInt ptr);

    // callback on object delete
    void Delete(PointerSizedUInt ptr);

    // validate a pointer
    void Check(PointerSizedUInt ptr);

    // dump all debug info
    void Dump();
  };
}

#endif
