#include "FoundationPch.h"
#include "PropertyCommand.h"

#include "Queue.h"

using namespace Helium::Undo;

class Test
{
public:
  int v;
  static int vs;

  void GetParam(int& i) const
  {
    i = v;
  }

  static void GetParamStatic(int& i)
  {
    i = vs;
  }

  const int& GetReference() const
  {
    return v;
  }

  static const int& GetReferenceStatic()
  {
    return vs;
  }

  int GetValue() const
  {
    return v;
  }

  static int GetValueStatic()
  {
    return vs;
  }

  void SetReference(const int& i)
  {
    v = i;
  }

  static void SetReferenceStatic(const int& i)
  {
    vs = i;
  }

  void SetValue(int i)
  {
    v = i;
  }

  static void SetValueStatic(int i)
  {
    vs = i;
  }
};

int Test::vs;

void TestFunc()
{
  Test test;

  int v = 5;

  Queue queue;

  queue.Push( new PropertyCommand<int> ( new Helium::StaticProperty<int> (&Test::GetParamStatic,      &Test::SetReferenceStatic) ) );
  queue.Push( new PropertyCommand<int> ( new Helium::StaticProperty<int> (&Test::GetParamStatic,      &Test::SetReferenceStatic), v ) );

  queue.Push( new PropertyCommand<int> ( new Helium::StaticProperty<int> (&Test::GetReferenceStatic,  &Test::SetValueStatic) ) );
  queue.Push( new PropertyCommand<int> ( new Helium::StaticProperty<int> (&Test::GetReferenceStatic,  &Test::SetValueStatic), v ));

  queue.Push( new PropertyCommand<int> ( new Helium::StaticProperty<int> (&Test::GetValueStatic,      &Test::SetReferenceStatic) ) );
  queue.Push( new PropertyCommand<int> ( new Helium::StaticProperty<int> (&Test::GetValueStatic,      &Test::SetReferenceStatic), v ) );

  queue.Push( new PropertyCommand<int> ( new Helium::StaticProperty<int> (&Test::GetParamStatic,      &Test::SetValueStatic) ) );
  queue.Push( new PropertyCommand<int> ( new Helium::StaticProperty<int> (&Test::GetParamStatic,      &Test::SetValueStatic), v ) );

  queue.Push( new PropertyCommand<int> ( new Helium::StaticProperty<int> (&Test::GetReferenceStatic,  &Test::SetReferenceStatic) ) );
  queue.Push( new PropertyCommand<int> ( new Helium::StaticProperty<int> (&Test::GetReferenceStatic,  &Test::SetReferenceStatic), v ) );

  queue.Push( new PropertyCommand<int> ( new Helium::StaticProperty<int> (&Test::GetValueStatic,      &Test::SetValueStatic) ) );
  queue.Push( new PropertyCommand<int> ( new Helium::StaticProperty<int> (&Test::GetValueStatic,      &Test::SetValueStatic), v ) );

  queue.Push( new PropertyCommand<int> ( new Helium::MemberProperty<Test, int> (&test,   &Test::GetParam,      &Test::SetReference) ) );
  queue.Push( new PropertyCommand<int> ( new Helium::MemberProperty<Test, int> (&test,   &Test::GetParam,      &Test::SetReference), v ) );

  queue.Push( new PropertyCommand<int> ( new Helium::MemberProperty<Test, int> (&test,   &Test::GetReference,  &Test::SetValue) ) );
  queue.Push( new PropertyCommand<int> ( new Helium::MemberProperty<Test, int> (&test,   &Test::GetReference,  &Test::SetValue), v ));

  queue.Push( new PropertyCommand<int> ( new Helium::MemberProperty<Test, int> (&test,   &Test::GetValue,      &Test::SetReference) ) );
  queue.Push( new PropertyCommand<int> ( new Helium::MemberProperty<Test, int> (&test,   &Test::GetValue,      &Test::SetReference), v ) );

  queue.Push( new PropertyCommand<int> ( new Helium::MemberProperty<Test, int> (&test,   &Test::GetParam,      &Test::SetValue) ) );
  queue.Push( new PropertyCommand<int> ( new Helium::MemberProperty<Test, int> (&test,   &Test::GetParam,      &Test::SetValue), v ) );

  queue.Push( new PropertyCommand<int> ( new Helium::MemberProperty<Test, int> (&test,   &Test::GetReference,  &Test::SetReference) ) );
  queue.Push( new PropertyCommand<int> ( new Helium::MemberProperty<Test, int> (&test,   &Test::GetReference,  &Test::SetReference), v ) );

  queue.Push( new PropertyCommand<int> ( new Helium::MemberProperty<Test, int> (&test,   &Test::GetValue,      &Test::SetValue) ) );
  queue.Push( new PropertyCommand<int> ( new Helium::MemberProperty<Test, int> (&test,   &Test::GetValue,      &Test::SetValue), v ) );
}