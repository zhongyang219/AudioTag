﻿// AudioTag.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include "AudioTagImp.h"
#include "Common.h"
#include <map>

struct ID3V1
{
    char header[3];         /* 标签头必须是"TAG"否则认为没有标签 */
    char title[30];         /* 标题 */
    char artist[30];        /* 作者 */
    char album[30];         /* 专集 */
    char year[4];           /* 出品年代 */
    char comment[28];       /* 备注 */
    char reserve;           /* 保留 */
    char track;             /* 音轨 */
    char genre;             /* 类型 */
};


CAudioTag::CAudioTag(const std::wstring& file_path)
    : m_file_path{ file_path }
{
    m_type = CCommon::GetAudioTypeByExtension(m_file_path);
}

void CAudioTag::GetAudioTag(bool id3v2_first)
{
    switch (m_type)
    {
    case AU_MP3:
        if (id3v2_first)
        {
            if (!GetID3V2Tag())
                GetID3V1Tag();
        }
        else
        {
            if (!GetID3V1Tag())
                GetID3V2Tag();
        }
        break;
    case AU_WMA:
        GetWmaTag();
        break;
    case AU_OGG:
        GetOggTag();
        break;
    //case AU_MP4:
    //    GetMp4Tag();
    //    break;
    case AU_FLAC:
        GetFlacTag();
        break;
    default:
        break;
    }
    CCommon::TagStrNormalize(m_song_info.title);
    CCommon::TagStrNormalize(m_song_info.artist);
    CCommon::TagStrNormalize(m_song_info.album);
}


std::wstring CAudioTag::GetAlbumCover(int & image_type, wchar_t* file_name)
{
    if (m_type != AudioType::AU_FLAC)
    {
        std::string tag_content;
        GetID3V2TagContents(tag_content);
        size_t cover_index = tag_content.find("APIC");		//查找专辑封面的标识字符串
        if (cover_index == std::string::npos)
            return std::wstring();
        return _GetAlbumCover(tag_content, cover_index, image_type, file_name);
    }
    else
    {
        std::string tag_contents;
        GetFlacTagContents(tag_contents);
        return _GetAlbumCover(tag_contents, 0, image_type, file_name);
    }
    return std::wstring();
}


const AudioInfo & CAudioTag::GetAudioInfo() const
{
    return m_song_info;
}

CAudioTag::~CAudioTag()
{
}

std::wstring CAudioTag::GetSpecifiedId3V2Tag(const std::string& tag_contents, const std::string& tag_identify)
{
    std::wstring tag_info;
    size_t tag_index;
    tag_index = tag_contents.find(tag_identify);	//查找一个标签标识字符串
    if (tag_identify == "TPE1" && tag_index == std::string::npos)	//如果在查找艺术家时找不到TPE1标签，尝试查找TPE2标签
    {
        tag_index = tag_contents.find("TPE2");
    }
    if (tag_identify == "TYER" && tag_index == std::string::npos)	//如果在查找年份时找不到TYER标签，尝试查找TDRC标签
    {
        tag_index = tag_contents.find("TDRC");
    }
    if (tag_index != std::string::npos && tag_index < tag_contents.size() - 8)
    {
        std::string size = tag_contents.substr(tag_index + 4, 4);
        const size_t tag_size = (BYTE)size[0] * 0x1000000 + (BYTE)size[1] * 0x10000 + (BYTE)size[2] * 0x100 + (BYTE)size[3];	//获取当前标签的大小
        if (tag_size <= 0)
            return std::wstring();
        if (tag_index + 11 >= tag_contents.size())
            return std::wstring();
        //判断标签的编码格式
        CodeType default_code;
        switch (tag_contents[tag_index + 10])
        {
        case 1: case 2:
            default_code = CodeType::UTF16;
            break;
        case 3:
            default_code = CodeType::UTF8;
            break;
        default:
            default_code = CodeType::ANSI;
            break;
        }
        std::string tag_info_str;
        if (tag_identify == "COMM" || tag_identify == "USLT")
        {
            if (default_code == CodeType::UTF16)
                tag_info_str = tag_contents.substr(tag_index + 18, tag_size - 8);
            else
                tag_info_str = tag_contents.substr(tag_index + 15, tag_size - 5);
        }
        else
        {
            tag_info_str = tag_contents.substr(tag_index + 11, tag_size - 1);
        }
        tag_info = CCommon::StrToUnicode(tag_info_str, default_code);
    }
    return tag_info;
}

bool CAudioTag::GetID3V1Tag()
{
    std::string tag_content;
    GetID3V1TagContents(tag_content);
    bool success;
    if (tag_content.substr(0, 3) == "TAG")
    {
        ID3V1* id3 = (ID3V1*)tag_content.c_str();

        std::string temp;
        temp = std::string(id3->title, 30);
        if (!temp.empty() && temp.front() != L'\0')
            m_song_info.title = CCommon::StrToUnicode(temp, CodeType::AUTO);

        temp = std::string(id3->artist, 30);
        if (!temp.empty() && temp.front() != L'\0')
            m_song_info.artist = CCommon::StrToUnicode(temp, CodeType::AUTO);

        temp = std::string(id3->album, 30);
        if (!temp.empty() && temp.front() != L'\0')
            m_song_info.album = CCommon::StrToUnicode(temp, CodeType::AUTO);

        temp = std::string(id3->year, 4);
        if (!temp.empty() && temp.front() != L'\0')
            m_song_info.year = CCommon::StrToUnicode(temp, CodeType::AUTO);

        temp = std::string(id3->comment, 28);
        if (!temp.empty() && temp.front() != L'\0')
            m_song_info.comment = CCommon::StrToUnicode(temp, CodeType::AUTO);
        m_song_info.track = id3->track;
        m_song_info.genre = CCommon::GetGenre(id3->genre);
        m_song_info.genre_idx = id3->genre;

        CCommon::TagStrNormalize(m_song_info.title);
        CCommon::TagStrNormalize(m_song_info.artist);
        CCommon::TagStrNormalize(m_song_info.album);
        CCommon::TagStrNormalize(m_song_info.year);
        CCommon::TagStrNormalize(m_song_info.comment);

        bool id3_empty;		//ID3V1标签信息是否为空
        id3_empty = m_song_info.title.empty() && m_song_info.artist.empty() && m_song_info.album.empty() && m_song_info.track == 0 && m_song_info.year.empty();
        success = !id3_empty;
    }
    else
    {
        success = false;
    }
    m_song_info.tag_type = (success ? 1 : 0);
    return success;
}

bool CAudioTag::GetID3V2Tag()
{
    bool success;
    std::string tag_content;
    GetID3V2TagContents(tag_content);
    if (!tag_content.empty())
    {
        const int TAG_NUM{ 7 };
        //要查找的标签标识字符串（标题、艺术家、唱片集、年份、注释、流派、音轨号）
        const std::string tag_identify[TAG_NUM] { "TIT2", "TPE1", "TALB", "TYER", "COMM", "TCON", "TRCK" };
        for (int i{}; i < TAG_NUM; i++)
        {
            std::wstring tag_info;
            tag_info = GetSpecifiedId3V2Tag(tag_content, tag_identify[i]);
            if (!tag_info.empty())
            {
                switch (i)
                {
                case 0: m_song_info.title = tag_info; break;
                case 1: m_song_info.artist = tag_info; break;
                case 2: m_song_info.album = tag_info; break;
                case 3: m_song_info.year = tag_info; break;
                case 4: m_song_info.comment = tag_info; break;
                case 5: m_song_info.genre = CCommon::GenreConvert(tag_info); break;
                case 6: m_song_info.track = _wtoi(tag_info.c_str()); break;
                }
            }
        }
        CCommon::TagStrNormalize(m_song_info.title);
        CCommon::TagStrNormalize(m_song_info.artist);
        CCommon::TagStrNormalize(m_song_info.album);
        bool id3_empty;		//ID3标签信息是否为空
        id3_empty = (m_song_info.title.empty() && m_song_info.artist.empty() && m_song_info.album.empty() && m_song_info.track == 0 && m_song_info.year.empty());
        success = !id3_empty;
    }
    else
    {
        success = false;
    }
    m_song_info.tag_type = (success ? 2 : 0);
    return success;
}

bool CAudioTag::GetWmaTag()
{
    std::string tag_contents;
    GetWmaTagContents(tag_contents);

    //获取标准帧信息
    const std::string standered_frame{ '\x33', '\x26', '\xB2', '\x75', '\x8E', '\x66', '\xCF', '\x11', '\xA6', '\xD9', '\x00', '\xAA', '\x00', '\x62', '\xCE', '\x6C', };
    size_t index = tag_contents.find(standered_frame);
    if (index != std::string::npos)
    {
        size_t tag_size = (tag_contents[index + 16] & 0xff)
                          + (tag_contents[index + 17] & 0xff) * 0x100
                          + (tag_contents[index + 18] & 0xff) * 0x10000
                          + (tag_contents[index + 19] & 0xff) * 0x1000000;
        std::string standered_tag_contents = tag_contents.substr(index + 34, tag_size);
        std::vector<std::string> tag_list;
        CCommon::StringSplit(standered_tag_contents, std::string(3, '\0'), tag_list);
        std::vector<std::wstring> tag_list_wcs;
        for (auto& tag : tag_list)
        {
            tag.push_back('\0');
            tag_list_wcs.push_back(CCommon::StrToUnicode(tag, CodeType::UTF16));
        }

        if (tag_list_wcs.size() >= 1)
            m_song_info.title = tag_list_wcs[0];
        if (tag_list_wcs.size() >= 2)
            m_song_info.artist = tag_list_wcs[1];
        if (tag_list_wcs.size() >= 3)
            m_song_info.comment = tag_list_wcs[2];
    }

    //获取扩展帧信息
    const std::string extended_frame{ '\x40', '\xA4', '\xD0', '\xD2', '\x07', '\xE3', '\xD2', '\x11', '\x97', '\xF0', '\x00', '\xA0', '\xC9', '\x5E', '\xA8', '\x50', };
    index = tag_contents.find(extended_frame);
    if (index != std::wstring::npos)
    {
        size_t tag_size = (tag_contents[index + 16] & 0xff)
                          + (tag_contents[index + 17] & 0xff) * 0x100
                          + (tag_contents[index + 18] & 0xff) * 0x10000
                          + (tag_contents[index + 19] & 0xff) * 0x1000000;

        std::string extended_tag_contents = tag_contents.substr(index + 26, tag_size - 26);
        std::vector<std::string> tag_list;
        CCommon::StringSplit(extended_tag_contents, std::string(3, '\0'), tag_list);
        std::map<std::wstring, std::wstring> extended_tag_map;
        int i = 0;
        std::wstring name, content;
        for (auto& tag : tag_list)
        {
            tag.push_back('\0');
            if (i % 2 == 0)
            {
                if (tag.size() > 2)
                    tag = tag.substr(2);
                else
                    continue;
                name = CCommon::StrToUnicode(tag, CodeType::UTF16);
            }
            else
            {
                if (tag.size() > 4)
                    tag = tag.substr(4);
                else
                    continue;
                content = CCommon::StrToUnicode(tag, CodeType::UTF16);
                extended_tag_map[name] = content;
            }
            i++;
        }

        m_song_info.track = _wtoi(extended_tag_map[L"WM/TrackNumber"].c_str());
        m_song_info.album = extended_tag_map[L"WM/AlbumTitle"];
        m_song_info.genre = extended_tag_map[L"WM/Genre"];
        m_song_info.year = extended_tag_map[L"WM/Year"];
    }

    return true;
}

bool CAudioTag::GetOggTag()
{
    std::string tag_contents;
    GetFileFrontContent(2048, tag_contents);
    if (tag_contents.substr(0, 4) == "OggS")
    {
        std::map<std::string, std::wstring> tag_map;
        const std::vector<std::string> tag_ids{ "Title", "Artist", "Album", "Tracknumber" };
        for (const auto& tag_id : tag_ids)
        {
            size_t index = CCommon::StringFindNoCase(tag_contents, tag_id);       //查找标签
            if (index != std::string::npos && index >= 4)
            {
                //标签前面4个字节是标签的大小
                size_t tag_size = (tag_contents[index - 4] & 0xff)
                                  + (tag_contents[index - 3] & 0xff) * 0x100
                                  + (tag_contents[index - 2] & 0xff) * 0x10000
                                  + (tag_contents[index - 1] & 0xff) * 0x1000000;
                std::string cur_tag_raw = tag_contents.substr(index, tag_size);
                index = cur_tag_raw.find('=');
                if(index == std::string::npos || index >= cur_tag_raw.size() - 1)
                    continue;
                tag_map[tag_id] = CCommon::StrToUnicode(cur_tag_raw.substr(index + 1), CodeType::UTF8);

            }
        }

        m_song_info.title = tag_map[tag_ids[0]];
        m_song_info.artist = tag_map[tag_ids[1]];
        m_song_info.album = tag_map[tag_ids[2]];
        m_song_info.track = _wtoi(tag_map[tag_ids[3]].c_str());
        return true;
    }

    return false;
}

bool CAudioTag::GetFlacTag()
{
    std::string tag_contents;		//整个标签区域的内容
    GetFlacTagContents(tag_contents);
    std::string flac_tag_str;		//当前标签的字符
    std::string flac_tag_title;
    std::string flac_tag_artist;
    std::string flac_tag_album;
    std::string flac_tag_track;
    std::string flac_tag_year;
    std::string flac_tag_genre;
    char tag_count{};
    size_t tag_size = tag_contents.size();
    if (tag_size < 4)
        return false;

    for (size_t i{}; i < tag_size; i++)	//只获取标签前面指定个数的字节
    {
        flac_tag_str.push_back(tag_contents[i]);
        if (tag_contents[i] == '\0' && tag_contents[i + 1] == '\0' && tag_contents[i + 2] == '\0')		//遇到3个'\0'，一组标签结束
        {
            if (flac_tag_str.size() < 2)
            {
                flac_tag_str.clear();
                continue;
            }
            flac_tag_str.pop_back();
            flac_tag_str.pop_back();
            size_t index;
            index = flac_tag_str.find_first_of('=');
            if (CCommon::StringFindNoCase(flac_tag_str, std::string("title")) != std::string::npos)
            {
                flac_tag_title = flac_tag_str.substr(index + 1);
                tag_count++;
            }
            else if (CCommon::StringFindNoCase(flac_tag_str, std::string("Artist")) != std::string::npos)
            {
                flac_tag_artist = flac_tag_str.substr(index + 1);
                tag_count++;
            }
            else if (CCommon::StringFindNoCase(flac_tag_str, std::string("Album")) != std::string::npos)
            {
                flac_tag_album = flac_tag_str.substr(index + 1);
                tag_count++;
            }
            else if (CCommon::StringFindNoCase(flac_tag_str, std::string("TrackNumber")) != std::string::npos)
            {
                flac_tag_track = flac_tag_str.substr(index + 1);
                tag_count++;
            }
            else if (CCommon::StringFindNoCase(flac_tag_str, std::string("Date")) != std::string::npos)
            {
                flac_tag_year = flac_tag_str.substr(index + 1);
                tag_count++;
            }
            else if (CCommon::StringFindNoCase(flac_tag_str, std::string("Genre")) != std::string::npos)
            {
                flac_tag_genre = flac_tag_str.substr(index + 1);
                tag_count++;
            }

            flac_tag_str.clear();
        }

        if (tag_count >= 6)		//已经获取到了6个标签，退出循环
            break;

        if (tag_contents.substr(i, 6) == "image/")	//遇到"image/"，后面就是专辑封面了
            break;
    }

    if (!flac_tag_title.empty())
        m_song_info.title = CCommon::StrToUnicode(flac_tag_title, CodeType::UTF8);
    if (!flac_tag_artist.empty())
        m_song_info.artist = CCommon::StrToUnicode(flac_tag_artist, CodeType::UTF8);
    if (!flac_tag_album.empty())
        m_song_info.album = CCommon::StrToUnicode(flac_tag_album, CodeType::UTF8);
    if (!flac_tag_track.empty())
        m_song_info.track = atoi(flac_tag_track.c_str());
    if (!flac_tag_year.empty())
        m_song_info.year = CCommon::StrToUnicode(flac_tag_year, CodeType::UTF8);
    if (!flac_tag_genre.empty())
        //m_song_info.genre = CAudioCommon::GenreConvert(CCommon::StrToUnicode(flac_tag_genre, CodeType::UTF8));
        m_song_info.genre = CCommon::StrToUnicode(flac_tag_genre, CodeType::UTF8);
    return true;
}


void CAudioTag::GetFlacTagContents(std::string & contents_buff)
{
    std::ifstream file{ m_file_path.c_str(), std::ios::binary };
    size_t size;
    if (file.fail())
        return;
    contents_buff.clear();
    while (!file.eof())
    {
        size = contents_buff.size();
        contents_buff.push_back(file.get());
        if (size > 1024 * 1024)
            break;
        //找到flac音频的起始字节时，表示标签信息已经读取完了
        if (size > 4 && (contents_buff[size - 1] & (BYTE)0xF8) == (BYTE)0xF8 && contents_buff[size - 2] == -1)
            break;
    }
}

void CAudioTag::GetID3V2TagContents(std::string & contents_buff)
{
    std::ifstream file{ m_file_path.c_str(), std::ios::binary };
    if (!file.fail())
    {
        //从文件前10个字节获取标签大小
        char tag_head[10];
        file.read(tag_head, 10);

        int tag_size{};

        //id3v2标签位于文件头部，且前3个字节必须是"ID3"，否则认为标签不存在
        if (std::string(tag_head, 3) == "ID3")
        {
            //读取大小
            tag_size = (tag_head[6] & 0x7F) * 0x200000 + (tag_head[7] & 0x7F) * 0x4000 + (tag_head[8] & 0x7F) * 0x80 + (tag_head[9] & 0x7F);	//获取标签区域的总大小
        }

        //读取到了标签大小，开始获取标签内容
        if (tag_size > 0)
        {
            file.seekg(std::ios::beg);  //移动到文件最前面
            char* buff = new char[tag_size];
            file.read(buff, tag_size);
            contents_buff.assign(buff, tag_size);
            delete[] buff;
        }
    }
}

void CAudioTag::GetID3V1TagContents(std::string & contents_buff)
{
    contents_buff.clear();
    std::ifstream file{ m_file_path.c_str(), std::ios::binary };
    if (file.fail())
        return;
    file.seekg(-128, std::ios::end);
    while (!file.eof())
    {
        contents_buff.push_back(file.get());
        if(contents_buff.size() > 128)
            break;
    }
    contents_buff.pop_back();
}

void CAudioTag::GetWmaTagContents(std::string & contents_buff)
{
    std::ifstream file{ m_file_path.c_str(), std::ios::binary };
    size_t size;
    if (file.fail())
        return;
    contents_buff.clear();
    size_t tag_size{};
    while (!file.eof())
    {
        size = contents_buff.size();
        contents_buff.push_back(file.get());
        if (size > 1024 * 1024)
            break;

        if (size == 24)
        {
            //获取标签区域的总大小
            tag_size = (contents_buff[16] & 0xff)
                       + (contents_buff[17] & 0xff) * 0x100
                       + (contents_buff[18] & 0xff) * 0x10000
                       + (contents_buff[19] & 0xff) * 0x1000000;
        }

        if (size >= 24 && size >= tag_size)
            break;
    }
}

void CAudioTag::GetFileFrontContent(size_t size, std::string & contents_buff)
{
    std::ifstream file{ m_file_path.c_str(), std::ios::binary };
    if (file.fail())
        return;
    contents_buff.clear();
    while (!file.eof())
    {
        contents_buff.push_back(file.get());
        if (contents_buff.size() > size)
            break;
    }
}

std::wstring CAudioTag::_GetAlbumCover(const std::string & tag_content, size_t cover_index, int & image_type, wchar_t* file_name)
{
    //获取图片起始位置
    size_t type_index = tag_content.find("image", cover_index);
    if (type_index == std::wstring::npos)
        type_index = cover_index;

    //根据图片类型设置文件扩展名
    size_t image_index;		//图片数据的起始位置
    size_t image_size;		//根据图片结束字节计算出的图片大小
    //设置图片文件的头和尾

    const std::string jpg_head{ '\xff', '\xd8' };
    const std::string jpg_tail{ '\xff', '\xd9' };
    const std::string png_head{ '\x89', '\x50', '\x4e', '\x47' };
    const std::string png_tail{ '\x49', '\x45', '\x4e', '\x44', '\xae', '\x42', '\x60', '\x82' };
    const std::string gif_head{ "GIF89a" };
    const std::string gif_tail{ '\x80', '\x00', '\x00', '\x3b' };

    std::string image_contents;
    //if (image_type_str == "image/jpeg" || image_type_str2 == "image/jpg" || image_type_str2 == "image/peg")
    image_index = tag_content.find(jpg_head, type_index);
    if (image_index < type_index + 100)		//在专辑封面开始处的100个字节查找
    {
        image_type = 0;
        size_t end_index = tag_content.find(jpg_tail, image_index + jpg_head.size());
        image_size = end_index - image_index + jpg_tail.size();
        image_contents = tag_content.substr(image_index, image_size);
    }
    else		//没有找到jpg文件头则查找png文件头
    {
        image_index = tag_content.find(png_head, type_index);
        if (image_index < type_index + 100)		//在专辑封面开始处的100个字节查找
        {
            image_type = 1;
            size_t end_index = tag_content.find(png_tail, image_index + png_head.size());
            image_size = end_index - image_index + png_tail.size();
            image_contents = tag_content.substr(image_index, image_size);
        }
        else		//没有找到png文件头则查找gif文件头
        {
            image_index = tag_content.find(gif_head, type_index);
            if (image_index < type_index + 100)		//在专辑封面开始处的100个字节查找
            {
                image_type = 2;
                size_t end_index = tag_content.find(gif_tail, image_index + gif_head.size());
                image_size = end_index - image_index + gif_tail.size();
                image_contents = tag_content.substr(image_index, image_size);
            }
        }
    }

    //将专辑封面保存到临时目录
    std::wstring file_path{ CCommon::GetTemplatePath() };
    std::wstring _file_name;
    if (file_name == nullptr)
        _file_name = L"AlbumCover-NmU3Yb51";
    else
        _file_name = file_name;
    if (!image_contents.empty())
    {
        file_path += _file_name;
        std::ofstream out_put{ file_path, std::ios::binary };
        out_put << image_contents;
        return file_path;
    }
    image_type = -1;
    return std::wstring();
}

bool CAudioTag::WriteMp3Tag(const AudioInfo & song_info, bool & text_cut_off)
{
    std::string title, artist, album, year, comment;
    title = CCommon::UnicodeToStr(song_info.title, CodeType::ANSI);
    artist = CCommon::UnicodeToStr(song_info.artist, CodeType::ANSI);
    album = CCommon::UnicodeToStr(song_info.album, CodeType::ANSI);
    year = CCommon::UnicodeToStr(song_info.year, CodeType::ANSI);
    comment = CCommon::UnicodeToStr(song_info.comment, CodeType::ANSI);
    ID3V1 id3{};
    CCommon::StringCopy(id3.header, 3, "TAG");
    CCommon::StringCopy(id3.title, 30, title.c_str());
    CCommon::StringCopy(id3.artist, 30, artist.c_str());
    CCommon::StringCopy(id3.album, 30, album.c_str());
    CCommon::StringCopy(id3.year, 4, year.c_str());
    CCommon::StringCopy(id3.comment, 28, comment.c_str());
    id3.track = song_info.track;
    id3.genre = song_info.genre_idx;
    text_cut_off = (title.size() > 30 || artist.size() > 30 || album.size() > 30 || year.size() > 4 || comment.size() > 28);

    std::fstream fout;
    fout.open(m_file_path, std::fstream::binary | std::fstream::out | std::fstream::in);
    if (fout.fail())
        return false;
    fout.seekp(-128, std::ios::end);		//移动到文件末尾的128个字节处
    fout.write((const char*)&id3, 128);		//将TAG_ID3V1结构体的128个字节写到文件末尾
    fout.close();
    return true;
}

IAudioTag* ATCreateInstance(const std::wstring& file_path)
{
    return new CAudioTag(file_path);
}
