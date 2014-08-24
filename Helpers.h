// ----------------------------------------------------------------------------
// Helpers.h
// ��������������� ��������� � ������
// @author EreTIk
// ----------------------------------------------------------------------------


#pragma once



// ----------------------------------------------------------------------------
// �c������������� ����� ������ � WSZ-�������, ���������� ����������� COM 
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
// �c������������� ����� ������ ����� PROPVARIANT, ���������� ������
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
