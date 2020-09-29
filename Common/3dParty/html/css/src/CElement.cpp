#include "CElement.h"
#include <iostream>
#include <algorithm>
#include <cwctype>
#include <numeric>

CElement::CElement() {}

CElement::~CElement()
{
    for (CElement* oChildren : m_arChildrens)
        delete oChildren;

    m_arChildrens.clear();
    m_arSelectors.clear();
    m_arDeclarations.clear();
}

bool CElement::Empty()
{
    return m_arSelectors.empty() && m_arDeclarations.empty();
}


std::wstring CElement::GetText() const
{
    std::wstring sText;

    if(m_arSelectors.size() > 0)
    {
        for (const std::wstring& sSelector : m_arSelectors)
            sText += sSelector + L", ";

        sText.erase(sText.length() - 2, 2);
    }
    if (!m_arDeclarations.empty() ||
        !m_arChildrens.empty())
    {
        sText += L"{\n";

        for (const CElement* oChildren : m_arChildrens)
            sText += oChildren->GetText();

        for (const std::map<std::wstring, std::wstring>::value_type& pDeclaration : m_arDeclarations)
        {
            sText += L"   " + pDeclaration.first + L": " + pDeclaration.second + L";\n";
        }
        sText += L"};\n";
    }
    else
        sText += L";\n";
    return sText;
}


void CElement::AddChildren(CElement* oChildren)
{
    m_arChildrens.push_back(oChildren);
}

void CElement::AddSelector(std::wstring sSelector)
{
    sSelector.erase(std::remove_if(sSelector.begin(), sSelector.end(), [] (wchar_t ch) { return std::iswspace(ch); }), sSelector.end());
    m_arSelectors.push_back(sSelector);
    std::sort(m_arSelectors.begin(), m_arSelectors.end());
}

/*
void CElement::AddDeclaration(const std::pair<std::wstring, std::wstring>& pDeclaration)
{
    m_arDeclarations.insert(pDeclaration);
}
*/

void CElement::AddSelectors(const std::vector<std::wstring>& arSelectors)
{
    m_arSelectors.insert(m_arSelectors.end(), arSelectors.begin(), arSelectors.end());
    std::sort(m_arSelectors.begin(), m_arSelectors.end());
}

void CElement::AddDeclarations(const std::map<std::wstring, std::wstring>& arDeclarations)
{
    m_arDeclarations.insert(arDeclarations.begin(), arDeclarations.end());
}

/*
void CElement::SetDeclaratins(const std::map<std::wstring, std::wstring>& arDeclarations)
{
    m_arDeclarations = arDeclarations;
}
*/

size_t CElement::GetCountSelectors() const
{
    return m_arSelectors.size();
}

size_t CElement::GetCountDeclarations() const
{
    return m_arDeclarations.size();
}

/*
int CElement::GetCountChildrens() const
{
    return m_arChildrens.size();
}
*/

bool CElement::FindSelector(const std::wstring& sSelector) const
{
    return (m_arSelectors.size() == 1) ? m_arSelectors[0] == sSelector :
           (m_arSelectors.size() == 2) ? m_arSelectors[0] == sSelector || m_arSelectors[1] == sSelector :
           (m_arSelectors.size() == 3) ? m_arSelectors[0] == sSelector || m_arSelectors[1] == sSelector || m_arSelectors[2] == sSelector :
           std::find(m_arSelectors.begin(), m_arSelectors.end(), sSelector) != m_arSelectors.end();
}

/*
std::vector<std::wstring> CElement::GetSelectors() const
{
    return m_arSelectors;
}

std::map<std::wstring, std::wstring> CElement::GetDeclarations() const
{
    return m_arDeclarations;
}
*/

std::map<std::wstring, std::wstring> CElement::GetDeclarations(const std::wstring& sSelector, const std::vector<std::wstring>& arParents) const
{
    if (m_arSelectors.size() == 0)
        return std::map<std::wstring, std::wstring>();

    switch (m_arSelectors.size())
    {
        case 1:
            if (m_arSelectors[0] == sSelector)
                return m_arDeclarations;
            break;
        case 2:
            if (m_arSelectors[0] == sSelector || m_arSelectors[1] == sSelector)
                return m_arDeclarations;
            break;
        case 3:
            if (m_arSelectors[0] == sSelector || m_arSelectors[1] == sSelector || m_arSelectors[2] == sSelector)
                return m_arDeclarations;
            break;
        default:
        {
            if(std::find(m_arSelectors.begin(), m_arSelectors.end(), sSelector) != m_arSelectors.end())
                return m_arDeclarations;
            break;
        }
    }
    return std::map<std::wstring, std::wstring>();

}

/*
std::vector<CElement*> CElement::GetChildrens() const
{
    return m_arChildrens;
}
*/

CElement& CElement::operator= (const CElement &oElement)
{
    m_arChildrens.clear();
    m_arSelectors = oElement.m_arSelectors;
    m_arDeclarations = oElement.m_arDeclarations;
    for (const CElement *oChildren : oElement.m_arChildrens)
    {
        CElement *oTempChildren = new CElement();
        *oTempChildren = *oChildren;
        m_arChildrens.push_back(oTempChildren);
    }
    return *this;
}
