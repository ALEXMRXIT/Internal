#pragma once
#include "framework.h"
#include "Transform.h"

class Component {
private:
    typedef std::unordered_map<size_t, AbstractBaseComponent*> ComponentMap;
    ComponentMap m_components;
    mutable std::vector<AbstractBaseComponent*> m_componentsCache;
    mutable bool m_cacheDirty;

public:
    ALWAYS_INLINE Component() : m_cacheDirty(true) {}

    Component(const Component&) = delete;
    Component& operator=(const Component&) = delete;

    ~Component() {
        for (ComponentMap::iterator it = m_components.begin(); it != m_components.end(); ++it)
            delete it->second;
    }

    template<class TComponent>
    ALWAYS_INLINE TComponent* AddComponent(GameObject* obj) {
        const size_t typeHash = typeid(TComponent).hash_code();
        if (m_components.find(typeHash) != m_components.end())
            return nullptr;

        TComponent* component = new TComponent(obj);
        m_components[typeHash] = component;
        m_cacheDirty = true;
        return component;
    }

    template<class TComponent>
    ALWAYS_INLINE TComponent* GetComponentByType() const {
        const size_t typeHash = typeid(TComponent).hash_code();
        ComponentMap::const_iterator it = m_components.find(typeHash);
        return (it != m_components.end()) ? static_cast<TComponent*>(it->second) : nullptr;
    }

    ALWAYS_INLINE size_t size() const { return m_components.size(); }

    ALWAYS_INLINE const std::vector<AbstractBaseComponent*>& components() const {
        if (m_cacheDirty) {
            m_componentsCache.clear();
            for (ComponentMap::const_iterator it = m_components.begin(); it != m_components.end(); ++it)
                m_componentsCache.push_back(it->second);
            m_cacheDirty = false;
        }
        return m_componentsCache;
    }
};