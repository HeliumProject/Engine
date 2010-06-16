#include "Precompile.h"
#include "PivotTransform.h"

#include "Content/PivotTransform.h"
#include "Foundation/Math/EulerAngles.h"
#include "Foundation/Math/Constants.h"

#include "Application/Undo/PropertyCommand.h"
#include "HierarchyNodeType.h"
#include "Scene.h"
#include "Application/UI/ImageManager.h"

using namespace Math;
using namespace Luna;

LUNA_DEFINE_TYPE( Luna::PivotTransform );

void PivotTransform::InitializeType()
{
  Reflect::RegisterClass< Luna::PivotTransform >( "Luna::PivotTransform" );

  Enumerator::InitializePanel("Pivot", CreatePanelSignature::Delegate( &PivotTransform::CreatePanel ));
}

void PivotTransform::CleanupType()
{
  Reflect::UnregisterClass< Luna::PivotTransform >();
}

PivotTransform::PivotTransform( Luna::Scene* scene ) 
: Luna::Transform( scene, new Content::PivotTransform() )
{

}

PivotTransform::PivotTransform(Luna::Scene* scene, Content::PivotTransform* pivotTransform) 
: Luna::Transform( scene, pivotTransform )
{

}

i32 PivotTransform::GetImageIndex() const
{
  return Nocturnal::GlobalImageManager().GetImageIndex( "transform_16.png" );
}

std::string PivotTransform::GetApplicationTypeName() const
{
  return "Group";
}

void PivotTransform::Pack()
{
  __super::Pack();

  Content::PivotTransform* persistent = GetPackage< Content::PivotTransform >();

  persistent->m_Shear = Math::Vector3( m_Shear.xy, m_Shear.yz, m_Shear.xz );
  persistent->m_ScalePivot = m_ScalePivot;
  persistent->m_ScalePivotTranslate = m_ScalePivotTranslate;
  persistent->m_RotatePivot = m_RotatePivot;
  persistent->m_RotatePivotTranslate = m_RotatePivotTranslate;
  persistent->m_TranslatePivot = m_TranslatePivot;
}

void PivotTransform::Unpack() 
{
  __super::Unpack();

  Content::PivotTransform* persistent = GetPackage< Content::PivotTransform >();

  m_Shear = persistent->m_Shear;
  m_ScalePivot = persistent->m_ScalePivot;
  m_ScalePivotTranslate = persistent->m_ScalePivotTranslate;
  m_RotatePivot = persistent->m_RotatePivot;
  m_RotatePivotTranslate = persistent->m_RotatePivotTranslate;
  m_TranslatePivot = persistent->m_TranslatePivot;
}

Math::Shear PivotTransform::GetShear() const
{
  return m_Shear;
}

void PivotTransform::SetShear(const Math::Shear& value)
{
  m_Shear = value;

  Dirty();
}

Math::Vector3 PivotTransform::GetScalePivot() const
{
  Math::Vector3 result = m_ScalePivot;
  (GetScaleComponent() * GetRotateComponent()).TransformVertex( result );
  return result;
}

void PivotTransform::SetScalePivot(const Math::Vector3& value)
{
  SetScalePivot(value, true);
}

void PivotTransform::SetScalePivot(const Math::Vector3& value, bool snapSiblings)
{
  Math::Vector3 v = value;
  (GetScaleComponent() * GetRotateComponent()).Inverted().TransformVertex( v );

  // old pivot sample
  Math::Vector3 a = Math::Vector3::Zero;

  // new pivot sample
  Math::Vector3 b = Math::Vector3::Zero;

  // build total scale with old pivot
  Math::Matrix4 scaleA = GetScaleComponent();
  scaleA.TransformVertex(a);

  // set the new pivot value
  m_ScalePivot = v;

  // build total scale with new pivot
  Math::Matrix4 scaleB = GetScaleComponent();
  scaleB.TransformVertex(b);

  // compensate for the delta
  m_ScalePivotTranslate = m_ScalePivotTranslate + a - b;

  if (GetSnapPivots() && snapSiblings)
  {
    SetRotatePivot(value, false);
    SetTranslatePivot(value, false);
  }

  Dirty();
}

Math::Vector3 PivotTransform::GetScalePivotTranslate() const
{
  return m_ScalePivotTranslate;
}

void PivotTransform::SetScalePivotTranslate(const Math::Vector3& value)
{
  m_ScalePivotTranslate = value;

  Dirty();
}

Math::Vector3 PivotTransform::GetRotatePivot() const
{
  Math::Vector3 result = m_RotatePivot;
  GetRotateComponent().TransformVertex( result );
  return result;
}

void PivotTransform::SetRotatePivot(const Math::Vector3& value)
{
  SetRotatePivot(value, true);
}

void PivotTransform::SetRotatePivot(const Math::Vector3& value, bool snapSiblings)
{
  Math::Vector3 v = value;
  GetRotateComponent().Inverted().TransformVertex( v );

  // old pivot sample
  Math::Vector3 a = Math::Vector3::Zero;

  // new pivot sample
  Math::Vector3 b = Math::Vector3::Zero;

  // build total rotation with old pivot
  Math::Matrix4 rotateA = GetRotateComponent();
  rotateA.TransformVertex(a);

  // set the new pivot value
  m_RotatePivot = v;

  // build total rotation with new pivot
  Math::Matrix4 rotateB = GetRotateComponent();
  rotateB.TransformVertex(b);

  // compensate for the delta
  m_RotatePivotTranslate = m_RotatePivotTranslate + a - b;

  if (GetSnapPivots() && snapSiblings)
  {
    SetScalePivot(value, false);
    SetTranslatePivot(value, false);
  }

  Dirty();
}

Math::Vector3 PivotTransform::GetRotatePivotTranslate() const
{
  return m_RotatePivotTranslate;
}

void PivotTransform::SetRotatePivotTranslate(const Math::Vector3& value)
{
  m_RotatePivotTranslate = value;

  Dirty();
}

Math::Vector3 PivotTransform::GetTranslatePivot() const
{
  return m_TranslatePivot;
}

void PivotTransform::SetTranslatePivot(const Math::Vector3& value)
{
  SetTranslatePivot(value, true);
}

void PivotTransform::SetTranslatePivot(const Math::Vector3& value, bool snapSiblings)
{
  m_TranslatePivot = value;

  if (GetSnapPivots() && snapSiblings)
  {
    SetScalePivot(value, false);
    SetRotatePivot(value, false);
  }

  Dirty();
}

bool PivotTransform::GetSnapPivots() const
{
  return GetPackage<Content::PivotTransform>()->m_SnapPivots;
}

void PivotTransform::SetSnapPivots(bool value)
{
  GetPackage<Content::PivotTransform>()->m_SnapPivots = value;
}

Math::Matrix4 PivotTransform::GetScaleComponent() const
{
  Math::Matrix4 spi( m_ScalePivot * -1.0f );
  Math::Matrix4 sh( m_Shear );
  Math::Matrix4 sp( m_ScalePivot + m_ScalePivotTranslate );

  return spi * __super::GetScaleComponent() * sh * sp;
}

Math::Matrix4 PivotTransform::GetRotateComponent() const
{
  Math::Matrix4 rpi( m_RotatePivot * -1.0f );
  Math::Matrix4 rp( m_RotatePivot + m_RotatePivotTranslate );

  return rpi * __super::GetRotateComponent() * rp;
}

Math::Matrix4 PivotTransform::GetTranslateComponent() const
{
  return __super::GetTranslateComponent();
}

Undo::CommandPtr PivotTransform::ResetTransform()
{
  // we need to snapshot here to save pivot state
  Undo::CommandPtr command = SnapShot();

  // disregard the base class' command, its not complete
  __super::ResetTransform ();

  m_Shear = Math::Shear::Identity;

  m_ScalePivot = Math::Vector3::Zero;
  m_ScalePivotTranslate = Math::Vector3::Zero;

  m_RotatePivot = Math::Vector3::Zero;
  m_RotatePivotTranslate = Math::Vector3::Zero;

  m_TranslatePivot = Math::Vector3::Zero;

  return command;
}
// outputs
void MatrixDecomposeFixedPivots(Math::Scale& scale, Math::Shear& shear, 
                                Math::EulerAngles& rotate, Math::Vector3& translate, 
                                // inputs 
                                const Math:: Matrix4& totalMatrix, 
                                const Math::Vector3& scalePivot, const Math::Vector3& scalePivotTranslate, 
                                const Math::Vector3& rotatePivot, const Math::Vector3& rotatePivotTranslate, 
                                const Math::Vector3& translatePivot)
{
  // [tfm] = [Sp]-1x[S]x[Sh]x[Sp]x[Spt]x[Rp]-1x[R]x[Rp]x[Rpt]x[T]

  // [tfm] = [Sp]-1x[M1]
  //    M1 =        [S]x[Sh]x[Sp]x[Spt]x[Rp]-1x[R]x[Rp]x[Rpt]x[T]
  //    M1 = [Sp]x[Sp]-1[M1]; 
  //    M1 = [Sp]x[tfm]; 

  Matrix4 M1 = Matrix4(scalePivot) * totalMatrix; 

  //    M1 = [S]x[Sh]x[Sp]x[Spt]x[Rp]-1x[R]x[Rp]x[Rpt]x[T]
  //    M1 = [S]x[Sh]x[M2]; 

  Math::Shear   localShear; 
  Math::Scale   localScale; 
  Math::Matrix3 localRot; 
  Math::Vector3 localTrans; 

  //    M1 = [S]x[Sh]x[localRot]x[localTrans]
  // 
  M1.Decompose(localScale, localShear, localRot, localTrans); 

  //    M2 = [localRot]x[localTrans]
  Matrix4 M2 = Matrix4(localRot) * Matrix4(localTrans); 

  //    M2 = [Sp]x[Spt]x[Rp]-1x[R]x[Rp]x[Rpt]x[T]
  //    M2 = [M3]             x[R]x[M4]
  //    M2 = [M3]             x[M5]

  // we know Sp, Spt, and Rp so get them out of there
  // 
  //    M3  =                   [Sp]x[Spt]x[Rp']
  //     I  = [Rp]x[Spt']x[Sp']x[Sp]x[Spt]x[Rp']
  //     I  = [M3']            x[M3] 
  //    M3' = [Rp]x[Spt']x[Sp'] 
  // 
  //         M2 = [M3]x[M5]; 
  //   [M3']xM2 = [M5]; 
  Matrix4 M3_inv = Matrix4(rotatePivot) * Matrix4(scalePivotTranslate * -1.0f) * Matrix4(scalePivot * -1.0f); 

  Math::Shear   noShear; 
  Math::Scale   noScale; 
  Math::Matrix3 finalRot; 
  Math::Vector3 M4_trans; 

  Math::Matrix4 M5 = M3_inv * M2; 

  // recall: [M5] = [R]x[M4]

  M5.Decompose(noScale, noShear, finalRot, M4_trans); 

  // recall: [M4] = [Rp]x[Rpt]x[T] -- just translation in all of those
  Math::Matrix4 M4(M4_trans); 

  // therefore: [Rpt']x[Rp']x[Rp]x[Rpt]x[T] = [T]
  //            [Rpt']x[Rp']x[M4] = [T]

  Math::Matrix4 T = Matrix4(rotatePivotTranslate * -1.0f) * Matrix4(rotatePivot*-1.0f) * M4; 

  // that's our final translation
  // 
  Math::Vector3 finalTrans(T.t.x, T.t.y, T.t.z); 

  shear = localShear; 
  scale = localScale; 

  rotate = finalRot; 
  translate = finalTrans; 
}

Undo::CommandPtr PivotTransform::ComputeObjectComponents()
{
  const Matrix4& parentGlobalTransform = m_Parent->GetTransform()->GetGlobalTransform();
  const Matrix4& parentInverseGlobalTransform = m_Parent->GetTransform()->GetInverseGlobalTransform();


  //
  // Save pivots into locals and transform them
  //

  Math::Vector3 scalePivot = m_ScalePivot;
  Math::Vector3 rotatePivot = m_RotatePivot;
  Math::Vector3 translatePivot = m_TranslatePivot;
  Math::Vector3 scalePivotTranslate = m_ScalePivotTranslate; 
  Math::Vector3 rotatePivotTranslate = m_RotatePivotTranslate; 

  // take from the base class, but we don't use the base class because we are 
  // decomposing to include shear 
  // 

  Undo::CommandPtr command = ResetTransform();

  Math::Scale scale;
  Math::Shear shear; 
  Math::EulerAngles rotate;
  Math::Vector3 translate;

  Matrix4 nextLocalMatrix; 

  // zero the compensators out. i don't 100% understand the compensators, and 
  // the old code was disregarding them, so i'm zeroing them out as well
  //
  // the code below will work if we want to keep them in the future. 
  //
  scalePivotTranslate = Vector3::Zero; 
  rotatePivotTranslate = Vector3::Zero; 

  if (GetInheritTransform())
  {
    nextLocalMatrix = m_GlobalTransform * m_Parent->GetTransform()->GetInverseGlobalTransform(); 
  }
  else
  {
    nextLocalMatrix = m_GlobalTransform; 
  }

  // decompose the next localMatrix into the parts we don't know...
  MatrixDecomposeFixedPivots(scale, shear, rotate, translate, 
    nextLocalMatrix, 
    scalePivot, scalePivotTranslate, 
    rotatePivot, rotatePivotTranslate, 
    translatePivot); 

  // local vars to member variables 
  //
  m_Shear = shear; 
  m_Scale = scale;
  m_Rotate = rotate;
  m_Translate = translate;


  m_ScalePivot = scalePivot;
  m_RotatePivot = rotatePivot;
  m_TranslatePivot = translatePivot;

  m_ScalePivotTranslate = scalePivotTranslate; 
  m_RotatePivotTranslate = rotatePivotTranslate; 

  return command;
}

Undo::CommandPtr PivotTransform::CenterTransform()
{
  Undo::BatchCommandPtr batch = new Undo::BatchCommand();

  batch->Push( __super::CenterTransform() );

  // if we are not a group or we don't have children then don't bother
  if (!IsGroup() || m_Children.Empty())
  {
    return batch;
  }

  Math::Vector3 pos;
  float w = 1.0f / m_Children.Size();
  for ( OS_HierarchyNodeDumbPtr::Iterator itr = m_Children.Begin(), end = m_Children.End(); itr != end; ++itr )
  {
    Luna::HierarchyNode* hierarchyNode = *itr;

    const Luna::Transform* transform = hierarchyNode->GetTransform();
    NOC_ASSERT( transform );
    if ( transform )
    {
      const Math::Matrix4& globalTransform = transform->GetGlobalTransform();
      Math::Vector3 p = Math::Vector3( globalTransform.t.x, globalTransform.t.y, globalTransform.t.z );
      p *= w;
      pos += p;
    }
  }

  batch->Push( new Undo::PropertyCommand<Math::Matrix4> ( new Nocturnal::MemberProperty<Luna::Transform, Math::Matrix4> (this, &Luna::Transform::GetGlobalTransform, &Luna::Transform::SetGlobalTransform), Matrix4 (pos) ) );

  Evaluate(GraphDirections::Downstream);

  for ( OS_HierarchyNodeDumbPtr::Iterator itr = m_Children.Begin(), end = m_Children.End(); itr != end; ++itr )
  {
    Luna::HierarchyNode* n = *itr;

    Luna::Transform* t = Reflect::ObjectCast<Luna::Transform>( n );

    if (!t)
    {
      continue;
    }

    batch->Push( t->ComputeObjectComponents() );
  }

  Dirty();

  return batch;
}

bool PivotTransform::ValidatePanel(const std::string& name)
{
  if (name == "Pivot")
  {
    return true;
  }

  return __super::ValidatePanel(name);
}

void PivotTransform::CreatePanel(CreatePanelArgs& args)
{
  args.m_Enumerator->PushPanel("Pivots");

  {
    args.m_Enumerator->PushContainer();
    args.m_Enumerator->AddLabel("Shear");
    args.m_Enumerator->AddValue<Luna::PivotTransform, f32>(args.m_Selection, &PivotTransform::GetShearYZ, &PivotTransform::SetShearYZ);
    args.m_Enumerator->AddValue<Luna::PivotTransform, f32>(args.m_Selection, &PivotTransform::GetShearXZ, &PivotTransform::SetShearXZ);
    args.m_Enumerator->AddValue<Luna::PivotTransform, f32>(args.m_Selection, &PivotTransform::GetShearXY, &PivotTransform::SetShearXY);
    args.m_Enumerator->Pop();
  }

  {
    args.m_Enumerator->PushContainer();
    args.m_Enumerator->AddLabel("Scale Pivot");
    args.m_Enumerator->AddValue<Luna::PivotTransform, f32>(args.m_Selection, &PivotTransform::GetScalePivotX, &PivotTransform::SetScalePivotX);
    args.m_Enumerator->AddValue<Luna::PivotTransform, f32>(args.m_Selection, &PivotTransform::GetScalePivotY, &PivotTransform::SetScalePivotY);
    args.m_Enumerator->AddValue<Luna::PivotTransform, f32>(args.m_Selection, &PivotTransform::GetScalePivotZ, &PivotTransform::SetScalePivotZ);
    args.m_Enumerator->Pop();
  }

  {
    args.m_Enumerator->PushContainer();
    args.m_Enumerator->AddLabel("Scale Pivot Translate");
    args.m_Enumerator->AddValue<Luna::PivotTransform, f32>(args.m_Selection, &PivotTransform::GetScalePivotTranslateX, &PivotTransform::SetScalePivotTranslateX);
    args.m_Enumerator->AddValue<Luna::PivotTransform, f32>(args.m_Selection, &PivotTransform::GetScalePivotTranslateY, &PivotTransform::SetScalePivotTranslateY);
    args.m_Enumerator->AddValue<Luna::PivotTransform, f32>(args.m_Selection, &PivotTransform::GetScalePivotTranslateZ, &PivotTransform::SetScalePivotTranslateZ);
    args.m_Enumerator->Pop();
  }

  {
    args.m_Enumerator->PushContainer();
    args.m_Enumerator->AddLabel("Rotate Pivot");
    args.m_Enumerator->AddValue<Luna::PivotTransform, f32>(args.m_Selection, &PivotTransform::GetRotatePivotX, &PivotTransform::SetRotatePivotX);
    args.m_Enumerator->AddValue<Luna::PivotTransform, f32>(args.m_Selection, &PivotTransform::GetRotatePivotY, &PivotTransform::SetRotatePivotY);
    args.m_Enumerator->AddValue<Luna::PivotTransform, f32>(args.m_Selection, &PivotTransform::GetRotatePivotZ, &PivotTransform::SetRotatePivotZ);
    args.m_Enumerator->Pop();
  }

  {
    args.m_Enumerator->PushContainer();
    args.m_Enumerator->AddLabel("Rotate Pivot Translate");
    args.m_Enumerator->AddValue<Luna::PivotTransform, f32>(args.m_Selection, &PivotTransform::GetRotatePivotTranslateX, &PivotTransform::SetRotatePivotTranslateX);
    args.m_Enumerator->AddValue<Luna::PivotTransform, f32>(args.m_Selection, &PivotTransform::GetRotatePivotTranslateY, &PivotTransform::SetRotatePivotTranslateY);
    args.m_Enumerator->AddValue<Luna::PivotTransform, f32>(args.m_Selection, &PivotTransform::GetRotatePivotTranslateZ, &PivotTransform::SetRotatePivotTranslateZ);
    args.m_Enumerator->Pop();
  }

  {
    args.m_Enumerator->PushContainer();
    args.m_Enumerator->AddLabel("Translate Pivot");
    args.m_Enumerator->AddValue<Luna::PivotTransform, f32>(args.m_Selection, &PivotTransform::GetTranslatePivotX, &PivotTransform::SetTranslatePivotX);
    args.m_Enumerator->AddValue<Luna::PivotTransform, f32>(args.m_Selection, &PivotTransform::GetTranslatePivotY, &PivotTransform::SetTranslatePivotY);
    args.m_Enumerator->AddValue<Luna::PivotTransform, f32>(args.m_Selection, &PivotTransform::GetTranslatePivotZ, &PivotTransform::SetTranslatePivotZ);
    args.m_Enumerator->Pop();
  }

  {
    args.m_Enumerator->PushContainer();
    args.m_Enumerator->AddLabel("Snap Pivots");
    args.m_Enumerator->AddCheckBox<Luna::PivotTransform, bool>(args.m_Selection, &PivotTransform::GetSnapPivots, &PivotTransform::SetSnapPivots);
    args.m_Enumerator->Pop();
  }

  args.m_Enumerator->Pop();
}

f32 PivotTransform::GetShearYZ() const
{
  return GetShear().yz;
}

void PivotTransform::SetShearYZ(f32 shear)
{
  Math::Shear s = GetShear();
  s.yz = shear;
  SetShear(s);
}

f32 PivotTransform::GetShearXZ() const
{
  return GetShear().xz;
}

void PivotTransform::SetShearXZ(f32 shear)
{
  Math::Shear s = GetShear();
  s.xz = shear;
  SetShear(s);
}

f32 PivotTransform::GetShearXY() const
{
  return GetShear().xy;
}

void PivotTransform::SetShearXY(f32 shear)
{
  Math::Shear s = GetShear();
  s.xy = shear;
  SetShear(s);
}

f32 PivotTransform::GetScalePivotX() const
{
  return GetScalePivot().x;
}

void PivotTransform::SetScalePivotX(f32 translate)
{
  Math::Vector3 s = GetScalePivot();
  s.x = translate;
  SetScalePivot(s);
}

f32 PivotTransform::GetScalePivotY() const
{
  return GetScalePivot().y;
}

void PivotTransform::SetScalePivotY(f32 translate)
{
  Math::Vector3 s = GetScalePivot();
  s.y = translate;
  SetScalePivot(s);
}

f32 PivotTransform::GetScalePivotZ() const
{
  return GetScalePivot().z;
}

void PivotTransform::SetScalePivotZ(f32 translate)
{
  Math::Vector3 s = GetScalePivot();
  s.z = translate;
  SetScalePivot(s);
}

f32 PivotTransform::GetScalePivotTranslateX() const
{
  return GetScalePivotTranslate().x;
}

void PivotTransform::SetScalePivotTranslateX(f32 translate)
{
  Math::Vector3 s = GetScalePivotTranslate();
  s.x = translate;
  SetScalePivotTranslate(s);
}

f32 PivotTransform::GetScalePivotTranslateY() const
{
  return GetScalePivotTranslate().y;
}

void PivotTransform::SetScalePivotTranslateY(f32 translate)
{
  Math::Vector3 s = GetScalePivotTranslate();
  s.y = translate;
  SetScalePivotTranslate(s);
}

f32 PivotTransform::GetScalePivotTranslateZ() const
{
  return GetScalePivotTranslate().z;
}

void PivotTransform::SetScalePivotTranslateZ(f32 translate)
{
  Math::Vector3 s = GetScalePivotTranslate();
  s.z = translate;
  SetScalePivotTranslate(s);
}
f32 PivotTransform::GetRotatePivotX() const
{
  return GetRotatePivot().x;
}

void PivotTransform::SetRotatePivotX(f32 translate)
{
  Math::Vector3 s = GetRotatePivot();
  s.x = translate;
  SetRotatePivot(s);
}

f32 PivotTransform::GetRotatePivotY() const
{
  return GetRotatePivot().y;
}

void PivotTransform::SetRotatePivotY(f32 translate)
{
  Math::Vector3 s = GetRotatePivot();
  s.y = translate;
  SetRotatePivot(s);
}

f32 PivotTransform::GetRotatePivotZ() const
{
  return GetRotatePivot().z;
}

void PivotTransform::SetRotatePivotZ(f32 translate)
{
  Math::Vector3 s = GetRotatePivot();
  s.z = translate;
  SetRotatePivot(s);
}

f32 PivotTransform::GetRotatePivotTranslateX() const
{
  return GetRotatePivotTranslate().x;
}

void PivotTransform::SetRotatePivotTranslateX(f32 translate)
{
  Math::Vector3 s = GetRotatePivotTranslate();
  s.x = translate;
  SetRotatePivotTranslate(s);
}

f32 PivotTransform::GetRotatePivotTranslateY() const
{
  return GetRotatePivotTranslate().y;
}

void PivotTransform::SetRotatePivotTranslateY(f32 translate)
{
  Math::Vector3 s = GetRotatePivotTranslate();
  s.y = translate;
  SetRotatePivotTranslate(s);
}

f32 PivotTransform::GetRotatePivotTranslateZ() const
{
  return GetRotatePivotTranslate().z;
}

void PivotTransform::SetRotatePivotTranslateZ(f32 translate)
{
  Math::Vector3 s = GetRotatePivotTranslate();
  s.z = translate;
  SetRotatePivotTranslate(s);
}

f32 PivotTransform::GetTranslatePivotX() const
{
  return GetTranslatePivot().x;
}

void PivotTransform::SetTranslatePivotX(f32 translate)
{
  Math::Vector3 s = GetTranslatePivot();
  s.x = translate;
  SetTranslatePivot(s);
}

f32 PivotTransform::GetTranslatePivotY() const
{
  return GetTranslatePivot().y;
}

void PivotTransform::SetTranslatePivotY(f32 translate)
{
  Math::Vector3 s = GetTranslatePivot();
  s.y = translate;
  SetTranslatePivot(s);
}

f32 PivotTransform::GetTranslatePivotZ() const
{
  return GetTranslatePivot().z;
}

void PivotTransform::SetTranslatePivotZ(f32 translate)
{
  Math::Vector3 s = GetTranslatePivot();
  s.z = translate;
  SetTranslatePivot(s);
}
