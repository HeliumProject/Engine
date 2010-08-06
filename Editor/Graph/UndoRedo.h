#pragma once

#include <vector>

#include "Editor/Graph/MenuState.h"
#include "Editor/Graph/Debug.h"

class UndoRedo
{
public:
	virtual ~UndoRedo();

	virtual void Undo() = 0;
	virtual void Redo() = 0;

	virtual void SetActive(bool active);
	virtual void Destroy() {}

protected:
	bool m_active;
};

class URGroup: public UndoRedo
{
public:
	void AddUR(UndoRedo *ur);

	virtual void Undo();
	virtual void Redo();

	virtual void SetActive(bool active);
	virtual void Destroy();

private:
	std::vector<UndoRedo *> m_items;
};

class URNeg: public UndoRedo
{
public:
	URNeg(UndoRedo *ur): m_ur(ur) {}
	virtual ~URNeg()    { DESTROY(m_ur); }

	virtual void Undo() { m_ur->Redo(); }
	virtual void Redo() { m_ur->Undo(); }

	virtual void SetActive(bool active);
	virtual void Destroy();

private:
	UndoRedo *m_ur;
};

class Undoer
{
public:
	Undoer();
	~Undoer();

	void Add(UndoRedo *ur);
	void Undo();
	void Redo();
	void Clear();
  bool IsDirty() const { return m_list.size() > 0; }

private:
	std::vector<UndoRedo *> m_list;
	int m_index;
};

