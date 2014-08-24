// ----------------------------------------------------------------------------
// Helpers.h
// Вспомогательные структуры и классы
// @author EreTIk
// ----------------------------------------------------------------------------


#pragma once



// ----------------------------------------------------------------------------
// Вcпомогательный класс работы с WSZ-чтрокой, выделенной библиотекой COM 
// ----------------------------------------------------------------------------
template <typename E>
class TCoAllocatedStr{
public:
    TCoAllocatedStr()
      : m_pBuffer(NULL)
    {
    }
    ~TCoAllocatedStr()
    {
        if (m_pBuffer)
            CoTaskMemFree(m_pBuffer);
    }

    operator E **()
    {
        return &m_pBuffer;
    }
    operator E *() const
    {
        return m_pBuffer;
    }

private:
    E *m_pBuffer;
};

// ----------------------------------------------------------------------------
// Вcпомогательный класс работы типом PROPVARIANT, содержащим строку
// ----------------------------------------------------------------------------
class TPropvarString {
public:
    TPropvarString()
    {
        PropVariantInit(&m_Value);
    }
    ~TPropvarString()
    {
        PropVariantClear(&m_Value);
    }

    operator PROPVARIANT() const
    {
        return m_Value;
    }
    operator LPPROPVARIANT()
    {
        return &m_Value;
    }

private:
    PROPVARIANT m_Value;
};
