#include "stdafx.h"
#include "Common.h"


CCommon::CCommon()
{
}


CCommon::~CCommon()
{
}

std::wstring CCommon::StrToUnicode(const std::string & str, CodeType code_type)
{
    if (str.empty()) return std::wstring();

    if (code_type == CodeType::AUTO)
    {
        //code_typeΪAUTOʱ�Զ��жϱ�������
        //���ǰ����UTF8��BOM�����������ΪUTF8
        if (str.size() >= 3 && str[0] == -17 && str[1] == -69 && str[2] == -65)
            code_type = CodeType::UTF8;
        //���ǰ����UTF16��BOM�����������ΪUTF16
        else if (str.size() >= 2 && str[0] == -1 && str[1] == -2)
            code_type = CodeType::UTF16;
        //else if (IsUTF8Bytes(str.c_str()))		//���û���ҵ�UTF8��UTF16��BOM�����ж��ַ����Ƿ���UTF8���������
        //	code_type = CodeType::UTF8_NO_BOM;
        else
            code_type = CodeType::ANSI;
    }

    std::wstring result;
    int size;
    if (code_type == CodeType::ANSI)
    {
        size = MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, NULL, 0);
        if (size <= 0) return std::wstring();
        wchar_t* str_unicode = new wchar_t[size + 1];
        MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, str_unicode, size);
        result.assign(str_unicode);
        delete[] str_unicode;
    }
    else if (code_type == CodeType::UTF8 || code_type == CodeType::UTF8_NO_BOM)
    {
        std::string temp;
        //���ǰ����BOM����ȥ��BOM
        if (str.size() >= 3 && str[0] == -17 && str[1] == -69 && str[2] == -65)
            temp = str.substr(3);
        else
            temp = str;
        size = MultiByteToWideChar(CP_UTF8, 0, temp.c_str(), -1, NULL, 0);
        if (size <= 0) return std::wstring();
        wchar_t* str_unicode = new wchar_t[size + 1];
        MultiByteToWideChar(CP_UTF8, 0, temp.c_str(), -1, str_unicode, size);
        result.assign(str_unicode);
        delete[] str_unicode;
    }
    else if (code_type == CodeType::UTF16)
    {
        std::string temp;
        //���ǰ����BOM����ȥ��BOM
        if (str.size() >= 2 && str[0] == -1 && str[1] == -2)
            temp = str.substr(2);
        else
            temp = str;
        if (temp.size() % 2 == 1)
            temp.pop_back();
        temp.push_back('\0');
        result = (const wchar_t*)temp.c_str();
    }
    return result;
}

std::string CCommon::UnicodeToStr(const std::wstring & wstr, CodeType code_type, bool * char_cannot_convert)
{
    if (wstr.empty()) return std::string();
    if (char_cannot_convert != nullptr)
        *char_cannot_convert = false;
    BOOL UsedDefaultChar{ FALSE };
    std::string result;
    int size{ 0 };
    if (code_type == CodeType::ANSI)
    {
        size = WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), -1, NULL, 0, NULL, NULL);
        if (size <= 0) return std::string();
        char* str = new char[size + 1];
        WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), -1, str, size, NULL, &UsedDefaultChar);
        result.assign(str);
        delete[] str;
    }
    else if (code_type == CodeType::UTF8 || code_type == CodeType::UTF8_NO_BOM)
    {
        result.clear();
        if (code_type == CodeType::UTF8)
        {
            result.push_back(-17);
            result.push_back(-69);
            result.push_back(-65);
        }
        size = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, NULL, 0, NULL, NULL);
        if (size <= 0) return std::string();
        char* str = new char[size + 1];
        WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, str, size, NULL, NULL);
        result.append(str);
        delete[] str;
    }
    else if (code_type == CodeType::UTF16)
    {
        result.clear();
        result.push_back(-1);	//��ǰ�����UTF16��BOM
        result.push_back(-2);
        result.append((const char*)wstr.c_str(), (const char*)wstr.c_str() + wstr.size() * 2);
        result.push_back('\0');
    }
    if (char_cannot_convert != nullptr)
        *char_cannot_convert = (UsedDefaultChar != FALSE);
    return result;
}

CodeType CCommon::JudgeCodeType(const std::string & str, CodeType default_code)
{
    //���ǰ����UTF8��BOM�����������ΪUTF8
    if (str.size() >= 3 && str[0] == -17 && str[1] == -69 && str[2] == -65)
        return CodeType::UTF8;
    //���ǰ����UTF16��BOM�����������ΪUTF16
    else if (str.size() >= 2 && str[0] == -1 && str[1] == -2)
        return CodeType::UTF16;
    //else if (IsUTF8Bytes(str.c_str()))		//���û���ҵ�UTF8��UTF16��BOM�����ж��ַ����Ƿ���UTF8���������
    //	return CodeType::UTF8_NO_BOM;
    else
        return default_code;
}

std::wstring CCommon::GetTemplatePath()
{
    std::wstring result;
    wchar_t buff[MAX_PATH];
    GetTempPath(MAX_PATH, buff);		//��ȡ��ʱ�ļ��е�·��
    result = buff;
    if (result.back() != L'\\' && result.back() != L'/')		//ȷ��·��������б��
        result.push_back(L'\\');
    return result;
}

void CCommon::TagStrNormalize(std::wstring & str)
{
    for (size_t i{}; i < str.size(); i++)
    {
        if (str[i] < 32 || str[i] >= static_cast<wchar_t>(0xfff0))
        {
            str = str.substr(0, i);
            return;
        }
    }
}

AudioType CCommon::GetAudioTypeByExtension(const std::wstring & file_name)
{
    std::wstring type;
    size_t index = file_name.rfind(L'.');
    if (index != std::wstring::npos && index < file_name.size() - 1)
    {
        type = file_name.substr(index + 1);
    }

    if (type == L"mp3")
        return AU_MP3;
    else if (type == L"wma")
        return AU_WMA;
    else if (type == L"ogg" || type == L"oga")
        return AU_OGG;
    else if (type == L"m4a")
        return AU_MP4;
    else if (type == L"mp4")
        return AU_MP4;
    else if (type == L"flac" || type == L"fla")
        return AU_FLAC;
    else
        return AU_OTHER;
}

std::wstring CCommon::GetGenre(BYTE genre)
{
    if (genre < GENRE_MAX)
        return GENRE_TABLE[genre];
    else
        return L"Unknown";
}

void CCommon::StringCopy(char * dest, size_t size, std::string source)
{
    size_t source_size = source.size();
    size_t i;
    size_t max_size = min(size, source_size);
    for (i = 0; i < size && i < source_size; i++)
    {
        dest[i] = source[i];
    }

    //ȷ��Ŀ���ַ���ĩβ��һ��\0
    size_t copy_cnt = i;
    if (copy_cnt < max_size)
        dest[copy_cnt] = L'\0';

}

void CCommon::StringSplit(const std::string & str, const std::string& div, std::vector<std::string>& results)
{
    results.clear();
    size_t split_index = 0;
    size_t last_split_index = 0;
    while (true)
    {
        size_t offset = (last_split_index == 0 ? 0 : div.size());
        split_index = str.find(div, split_index + offset);
        std::string split_str = str.substr(last_split_index + offset, split_index - last_split_index - offset);
        //StringNormalize(split_str);
        if (!split_str.empty())
            results.push_back(split_str);
        if (split_index == std::string::npos)
            break;
        last_split_index = split_index;
    }
}
