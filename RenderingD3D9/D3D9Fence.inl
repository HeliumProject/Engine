namespace Helium
{
    /// Get the Direct3D event query instance associated with this fence.
    ///
    /// @return  Direct3D event query instance.
    IDirect3DQuery9* D3D9Fence::GetQuery() const
    {
        return m_pQuery;
    }
}
