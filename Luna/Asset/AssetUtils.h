#pragma once

// Forwards
namespace Reflect
{
  class Element;
  class Field;
};

// Helper functions
namespace Luna
{
  bool IsFileReference( Reflect::Element* element, const Reflect::Field* field );
}
