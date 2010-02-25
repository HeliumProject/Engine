#pragma once

#include "Archive.h"

#define XML_STATIC
#include "expat/expatimpl.h"

namespace Reflect
{
  //
  // XML Archive Class
  //

  class REFLECT_API ArchiveXML : public Archive, public CExpatImpl <ArchiveXML>
  {
  public: 
    static const u32 CURRENT_VERSION; 
    static const u32 FIRST_VERSION_WITH_POINTER_SERIALIZER; 
    static const u32 FIRST_VERSION_WITH_NAMESPACE_SUPPORT;

  private:
    template <class _T>
    friend class CExpatImpl;
    friend class Archive;

    class ParsingState : public Nocturnal::RefCountBase<ParsingState>
    {
    public:
      // the name of the short name being processed
      std::string m_ShortName;

      // the cdata section for xml files
      std::string m_Buffer;

      // the current serializing field
      const Field* m_Field;

      // the item being processed
      ElementPtr m_Element;

      // The collected components
      V_Element m_Components;

      // flags, as specified below
      unsigned int m_Flags;

      enum ProcessFlag
      {
        kField  = 0x1 << 0,
      };

      ParsingState(const char* shortName)
      : m_ShortName (shortName)
      , m_Field (NULL) 
      , m_Flags (0)
      {

      }

      void SetFlag( ProcessFlag flag, bool state )
      {
        if ( state )
          m_Flags |= flag;
        else
          m_Flags &= ~flag;
      }

      bool GetFlag( ProcessFlag flag )
      {
        return ((m_Flags & flag) != 0x0);
      }
    };

    typedef Nocturnal::SmartPtr<ParsingState> ParsingStatePtr;

    // File format version
    u32 m_Version;

    // The nesting stack of parsing state
    std::stack<ParsingStatePtr> m_StateStack;

    // The current name of the serializing field
    std::stack<std::string> m_FieldNames;

    // The current collection of components
    V_Element m_Components;

    // The append elements
    V_Element m_Append;

    // The container to decode elements to
    V_Element* m_Target;

  private:
    ArchiveXML (StatusHandler* status = NULL);

  protected:
    // The type
    virtual ArchiveType GetType() const { return ArchiveTypes::XML; }

    // Begins parsing the InputStream
    virtual void Read();

    // Write to the OutputStream
    virtual void Write();

    // Write the file header
    virtual void Start();

    // Write the file footer
    virtual void Finish();

  public:
    // Access indentation
    Indent& GetIndent()
    {
      return m_Indent;
    }

  public:
    // Serialize
    virtual void Serialize(const ElementPtr& element);
    virtual void Serialize(const V_Element& elements, u32 flags = 0);

  protected:
    // Helpers
    void SerializeFields(const ElementPtr& element);
    void SerializeField(const ElementPtr& element, const Field* field);

    // <Element> and </Element>
    void SerializeHeader(const ElementPtr& element);
    void SerializeFooter(const ElementPtr& element);

  public:
    // For handling components
    virtual void Deserialize(ElementPtr& element);
    virtual void Deserialize(V_Element& elements, u32 flags = 0);

  private:
    // Registers required callbacks with ExpatImpl
    void OnPostCreate();

    // Called on <element>
    void OnStartElement(const XML_Char *pszName, const XML_Char **papszAttrs);

    // Called between <element> and </element>
    void OnCharacterData(const XML_Char *pszData, int nLength);

    // Called after </element>
    void OnEndElement(const XML_Char *pszName);
  };
}
