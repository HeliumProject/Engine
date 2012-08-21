////----------------------------------------------------------------------------------------------------------------------
//// DirectDeserializer.inl
////
//// Copyright (C) 2010 WhiteMoon Dreams, Inc.
//// All Rights Reserved
////----------------------------------------------------------------------------------------------------------------------
//
//namespace Helium
//{
//    /// Get the current buffer byte offset.
//    ///
//    /// @return  Buffer byte offset.
//    size_t DirectDeserializer::GetOffset() const
//    {
//        return m_offset;
//    }
//
//    /// Get whether a read past the end of the buffer has been attempted.
//    ///
//    /// Note that this only returns true if the read could not be fulfilled.  If the last read reached the end of the
//    /// buffer, but was still able to be read completely (i.e. a uint32_t value was read with exactly 4 bytes
//    /// remaining), this will still return false.
//    ///
//    /// @return  True if a read past the end of the buffer was attempted, false if not.
//    bool DirectDeserializer::GetEndOfStream() const
//    {
//        return m_bEndOfStream;
//    }
//}
