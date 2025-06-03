#pragma once
#include "framework.h"

template<typename TReturn, typename TParam>
class __declspec(novtable) ICallable {
public:
    virtual ~ICallable() {}
    virtual TReturn Invoke(TParam param) = 0;
    virtual ICallable* Clone() const = 0;
};

template<typename TReturn, typename TParam>
class CallbackFuncPtr : public ICallable<TReturn, TParam> {
private:
    typedef TReturn(*FuncPtr)(TParam);

private:
    FuncPtr m_func;

public:
    CallbackFuncPtr(FuncPtr func) : m_func(func) {  }

    virtual TReturn Invoke(TParam param) {
        return m_func(param);
    }

    virtual ICallable<TReturn, TParam>* Clone() const {
        return new CallbackFuncPtr<TReturn, TParam>(m_func);
    }
};

template<typename TReturn, typename TParam>
class Delegate {
private:
    ICallable<TReturn, TParam>* m_callable;

public:
    Delegate() : m_callable(NULL) {}
    ~Delegate() { delete m_callable; }

    Delegate(const Delegate& other) : m_callable(NULL) {
        if (other.m_callable) m_callable = other.m_callable->Clone();
    }

    Delegate& operator=(const Delegate& other) {
        if (this != &other) {
            delete m_callable;
            m_callable = other.m_callable ? other.m_callable->Clone() : NULL;
        }
        return *this;
    }

    void Bind(TReturn(*func)(TParam)) { m_callable = new CallbackFuncPtr<TReturn, TParam>(func); }

    TReturn Execute(TParam param) {
        if (m_callable)
            return m_callable->Invoke(param);
        return TReturn();
    }
};