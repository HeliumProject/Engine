
template <class T>
class API_DEFINE TEMPLATE_NAME : public Data
{
public:
    typedef T DataType;
    Helium::Reflect::DataPointer<DataType> m_Data;

    typedef TEMPLATE_NAME<DataType> SimpleDataT;
    REFLECT_DECLARE_OBJECT( SimpleDataT, Helium::Reflect::Data );

    TEMPLATE_NAME ();
    ~TEMPLATE_NAME();

    virtual bool IsCompact() const HELIUM_OVERRIDE { return true; }

    virtual void ConnectData(void* data) HELIUM_OVERRIDE;

    virtual bool Set(Helium::Reflect::Data* src, uint32_t flags = 0) HELIUM_OVERRIDE;
    virtual bool Equals(Helium::Reflect::Object* object) HELIUM_OVERRIDE;

    virtual void Serialize(const Helium::BasicBufferPtr& buffer, const tchar_t* debugStr) const HELIUM_OVERRIDE;

    virtual void Serialize(Helium::Reflect::ArchiveBinary& archive) HELIUM_OVERRIDE;
    virtual void Deserialize(Helium::Reflect::ArchiveBinary& archive) HELIUM_OVERRIDE;

    virtual void Serialize(Helium::Reflect::ArchiveXML& archive) HELIUM_OVERRIDE;
    virtual void Deserialize(Helium::Reflect::ArchiveXML& archive) HELIUM_OVERRIDE;

    virtual tostream& operator>>(tostream& stream) const HELIUM_OVERRIDE;
    virtual tistream& operator<<(tistream& stream);
};
