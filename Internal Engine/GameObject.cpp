#include "GameObject.h"
#include "Model.h"

GameObject::GameObject() {
	m_components = new Component();
	m_enable = true;
	serialize = true;
	selectable = false;
	m_parent = nullptr;
	m_firstChild = nullptr;
	m_prev = nullptr;
	m_next = nullptr;
	model = nullptr;
	selectedLayer = 0;
	selectedTag = 0;
}

void GameObject::SetParent(GameObject* parent) {
    if (m_parent)
        m_parent->RemoveChild(this);
    m_parent = parent;
    if (m_parent)
        m_parent->AddChild(this);
}

void GameObject::AddChild(GameObject* child) {
    if (!child) child->SetParent(this);
    if (!m_firstChild)
        m_firstChild = child;
    else {
        GameObject* lastChild = m_firstChild;
        while (lastChild->m_next)
            lastChild = lastChild->m_next;
        lastChild->m_next = child;
        child->m_prev = lastChild;
    }
}

void GameObject::RemoveChild(GameObject* child) {
    if (!child || child->m_parent != this) return;
    if (child->m_prev)
        child->m_prev->m_next = child->m_next;
    else m_firstChild = child->m_next;
    if (child->m_next)
        child->m_next->m_prev = child->m_prev;
    child->m_parent = nullptr;
    child->m_prev = nullptr;
    child->m_next = nullptr;
}

void GameObject::Enable(bool enable) {
	m_enable = enable;
}

bool GameObject::isTransparent() const { return false; }
