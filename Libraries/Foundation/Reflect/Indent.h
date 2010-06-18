#pragma once

#include "API.h"
#include "Stream.h" 


namespace Reflect
{
  class FOUNDATION_API Indent
  {
  private:
    // Indent spacing
    unsigned int m_Indent;

  public:
    Indent()
      : m_Indent (0)
    {

    }

    // Push indenting state
    void Push();

    // Pop the indenting state
    void Pop();

    // Method to actually send the indent to the stream.
    void Get(Stream& stream);

    // Method to actually send the indent to a file handle.
    void Get(FILE* file);
  };
}