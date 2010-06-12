#include "UniqueID.h"

#include "Platform/Assert.h"
#include "UID/GUID.h"
#include "UID/TUID.h"

#include <maya/MGlobal.h>
#include <maya/MFnDependencyNode.h>
#include <maya/MFnTypedAttribute.h>
#include <maya/MPlug.h>

using namespace Maya;

const char* s_GUIDAttributeName = "GUID";
const char* s_TUIDAttributeName = "TUID";

Nocturnal::UID::TUID Maya::GetNodeID( const MObject& node, bool create )
{
  if (node == MObject::kNullObj)
  {
    return Nocturnal::UID::TUID::Null;
  }

  MObject attr = MObject::kNullObj;
  MStatus status = MS::kFailure;
  MFnDependencyNode nodeFn (node);

  if ( create )
  {
    //
    // GUID->TUID legacy handling
    //

    // look for the old GUID attribute
    attr = nodeFn.attribute(MString (s_GUIDAttributeName), &status);

    // if we found it
    if ( status == MS::kSuccess && !attr.isNull() )
    {
      // get the GUID value
      MString str;
      MPlug plug (node, attr);
      status = plug.getValue(str);
      NOC_ASSERT( status != MS::kFailure );

      // parse it
      Nocturnal::UID::GUID id;
      bool parsed = id.FromString(str.asChar());
      NOC_ASSERT( parsed );

      // convert it to a TUID and set the new attribute
      Nocturnal::UID::TUID tuid;
      tuid.FromGUID( id );
      status = SetNodeID( node, tuid );
      NOC_ASSERT( status != MS::kFailure );

      // check to see if we are a locked node
      bool nodeWasLocked = nodeFn.isLocked();
      if ( nodeWasLocked )
      {
        // turn off any node locking so an attribute can be added
        nodeFn.setLocked( false );
      }

      // unlock the attribute
      status = plug.setLocked( false );
      NOC_ASSERT( status != MS::kFailure );

      // remove the attribute
      status = nodeFn.removeAttribute( attr );
      NOC_ASSERT( status != MS::kFailure );

      // reset to the prior state of wasLocked
      if ( nodeWasLocked )
      {
        nodeFn.setLocked( nodeWasLocked );
      }
    }
  }

  // look for the TUID attribute
  attr = nodeFn.attribute(MString (s_TUIDAttributeName));

  // retrieve the attribute value (may be empty if we are not creating a new id)
  MString str;
  MPlug plug (node, attr);
  plug.getValue(str);

  // if we don't have an existing id and we should create one
  if ( str.length() == 0 && create )
  {
    // generate a new ID
    Nocturnal::UID::TUID id( Nocturnal::UID::TUID::Generate() );

    // set the new ID value on the node
    if( SetNodeID( node, id ) )
    {
      return id;
    }
    else
    {
      return Nocturnal::UID::TUID::Null;
    }
  }

  // parse the value (this may be null if we did not create the attribute)
  Nocturnal::UID::TUID id;
  id.FromString(str.asChar());
  return id;
}

MStatus Maya::SetNodeID( const MObject& node, const Nocturnal::UID::TUID& id )
{
  MStatus status;
  MFnDependencyNode nodeFn (node);

  // make sure we have the dynamic attribute to store our id created
  MObject attr = nodeFn.attribute(MString (s_TUIDAttributeName), &status);
  if (status == MS::kFailure)
  {
    // check to see if we are a locked node
    bool nodeWasLocked = nodeFn.isLocked();
    if ( nodeWasLocked )
    {
      // turn off any node locking so an attribute can be added
      nodeFn.setLocked( false );
    }

    // create the attribute
    MFnTypedAttribute attrFn;
    attr = attrFn.create(MString (s_TUIDAttributeName), MString (s_TUIDAttributeName), MFnData::kString);
    status = nodeFn.addAttribute(attr);

    // reset to the prior state of wasLocked
    if ( nodeWasLocked )
    {
      nodeFn.setLocked( nodeWasLocked );
    }

    // error check
    if (status == MS::kFailure)
    {
      MGlobal::displayError(MString ("Unable to create TUID attribute on maya node: ") + nodeFn.name());
      return status;
    }
  }

  MPlug plug (node, nodeFn.attribute(MString (s_TUIDAttributeName)));

  plug.setLocked(false);

  std::string s;
  id.ToString(s);
  plug.setValue(MString(s.c_str()));

  plug.setLocked(true);

  return status;
}