#include "FoundationPch.h"
#include "UndoCommand.h"

#include "Foundation/Undo/UndoQueue.h"

using namespace Helium;

UndoCommand::UndoCommand()
{

}

UndoCommand::~UndoCommand()
{

}

BatchUndoCommand::BatchUndoCommand()
    : m_IsSignificant( false )
{

}

BatchUndoCommand::BatchUndoCommand(const std::vector<UndoCommandPtr>& objects)
    : m_IsSignificant( false )
{
    Set( objects );
}

void BatchUndoCommand::Set(const std::vector<UndoCommandPtr>& commands)
{
    m_Commands = commands;

    std::vector<UndoCommandPtr>::iterator itr = m_Commands.begin();
    std::vector<UndoCommandPtr>::iterator end = m_Commands.end();
    for ( m_IsSignificant = false; itr != end && !m_IsSignificant; ++itr )
    {
        UndoCommandPtr& command = *itr;
        m_IsSignificant |= command->IsSignificant();
    }
}

void BatchUndoCommand::Push(const UndoCommandPtr& command)
{
    if (command.ReferencesObject())
    {
        m_IsSignificant |= command->IsSignificant();
        m_Commands.push_back(command);
    }
}

void BatchUndoCommand::Undo()
{
    std::vector<UndoCommandPtr>::reverse_iterator rItr = m_Commands.rbegin();
    std::vector<UndoCommandPtr>::reverse_iterator rEnd = m_Commands.rend();
    for ( ; rItr != rEnd; ++rItr )
    {
        UndoCommandPtr& command = *rItr;
        command->Undo();
    }
}

void BatchUndoCommand::Redo()
{
    std::vector<UndoCommandPtr>::iterator itr = m_Commands.begin();
    std::vector<UndoCommandPtr>::iterator end = m_Commands.end();
    for ( ; itr != end; ++itr )
    {
        UndoCommandPtr& command = *itr;
        command->Redo();
    }
}

bool BatchUndoCommand::IsSignificant() const
{
    return m_IsSignificant;
}

bool BatchUndoCommand::IsEmpty() const
{
    return m_Commands.empty();
}

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

    UndoQueue queue;

    queue.Push( new PropertyUndoCommand<int> ( new Helium::StaticProperty<int> (&Test::GetParamStatic,      &Test::SetReferenceStatic) ) );
    queue.Push( new PropertyUndoCommand<int> ( new Helium::StaticProperty<int> (&Test::GetParamStatic,      &Test::SetReferenceStatic), v ) );

    queue.Push( new PropertyUndoCommand<int> ( new Helium::StaticProperty<int> (&Test::GetReferenceStatic,  &Test::SetValueStatic) ) );
    queue.Push( new PropertyUndoCommand<int> ( new Helium::StaticProperty<int> (&Test::GetReferenceStatic,  &Test::SetValueStatic), v ));

    queue.Push( new PropertyUndoCommand<int> ( new Helium::StaticProperty<int> (&Test::GetValueStatic,      &Test::SetReferenceStatic) ) );
    queue.Push( new PropertyUndoCommand<int> ( new Helium::StaticProperty<int> (&Test::GetValueStatic,      &Test::SetReferenceStatic), v ) );

    queue.Push( new PropertyUndoCommand<int> ( new Helium::StaticProperty<int> (&Test::GetParamStatic,      &Test::SetValueStatic) ) );
    queue.Push( new PropertyUndoCommand<int> ( new Helium::StaticProperty<int> (&Test::GetParamStatic,      &Test::SetValueStatic), v ) );

    queue.Push( new PropertyUndoCommand<int> ( new Helium::StaticProperty<int> (&Test::GetReferenceStatic,  &Test::SetReferenceStatic) ) );
    queue.Push( new PropertyUndoCommand<int> ( new Helium::StaticProperty<int> (&Test::GetReferenceStatic,  &Test::SetReferenceStatic), v ) );

    queue.Push( new PropertyUndoCommand<int> ( new Helium::StaticProperty<int> (&Test::GetValueStatic,      &Test::SetValueStatic) ) );
    queue.Push( new PropertyUndoCommand<int> ( new Helium::StaticProperty<int> (&Test::GetValueStatic,      &Test::SetValueStatic), v ) );

    queue.Push( new PropertyUndoCommand<int> ( new Helium::MemberProperty<Test, int> (&test,   &Test::GetParam,      &Test::SetReference) ) );
    queue.Push( new PropertyUndoCommand<int> ( new Helium::MemberProperty<Test, int> (&test,   &Test::GetParam,      &Test::SetReference), v ) );

    queue.Push( new PropertyUndoCommand<int> ( new Helium::MemberProperty<Test, int> (&test,   &Test::GetReference,  &Test::SetValue) ) );
    queue.Push( new PropertyUndoCommand<int> ( new Helium::MemberProperty<Test, int> (&test,   &Test::GetReference,  &Test::SetValue), v ));

    queue.Push( new PropertyUndoCommand<int> ( new Helium::MemberProperty<Test, int> (&test,   &Test::GetValue,      &Test::SetReference) ) );
    queue.Push( new PropertyUndoCommand<int> ( new Helium::MemberProperty<Test, int> (&test,   &Test::GetValue,      &Test::SetReference), v ) );

    queue.Push( new PropertyUndoCommand<int> ( new Helium::MemberProperty<Test, int> (&test,   &Test::GetParam,      &Test::SetValue) ) );
    queue.Push( new PropertyUndoCommand<int> ( new Helium::MemberProperty<Test, int> (&test,   &Test::GetParam,      &Test::SetValue), v ) );

    queue.Push( new PropertyUndoCommand<int> ( new Helium::MemberProperty<Test, int> (&test,   &Test::GetReference,  &Test::SetReference) ) );
    queue.Push( new PropertyUndoCommand<int> ( new Helium::MemberProperty<Test, int> (&test,   &Test::GetReference,  &Test::SetReference), v ) );

    queue.Push( new PropertyUndoCommand<int> ( new Helium::MemberProperty<Test, int> (&test,   &Test::GetValue,      &Test::SetValue) ) );
    queue.Push( new PropertyUndoCommand<int> ( new Helium::MemberProperty<Test, int> (&test,   &Test::GetValue,      &Test::SetValue), v ) );
}