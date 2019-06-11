#pragma once
#include "AudioTag.h"
#include <vector>

enum class CodeType
{
    ANSI,
    UTF8,
    UTF8_NO_BOM,
    UTF16,
    AUTO
};

#define GENRE_MAX 148
//�����б�
const wchar_t GENRE_TABLE[GENRE_MAX][24]
{
    L"Blues",
    L"ClassicRock",
    L"Country",
    L"Dance",
    L"Disco",
    L"Funk",
    L"Grunge",
    L"Hip-Hop",
    L"Jazz",
    L"Metal",
    L"NewAge",
    L"Oldies",
    L"Other",
    L"Pop",
    L"R&B",
    L"Rap",
    L"Reggae",
    L"Rock",
    L"Techno",
    L"Industrial",
    L"Alternative",
    L"Ska",
    L"DeathMetal",
    L"Pranks",
    L"Soundtrack",
    L"Euro-Techno",
    L"Ambient",
    L"Trip-Hop",
    L"Vocal",
    L"Jazz+Funk",
    L"Fusion",
    L"Trance",
    L"Classical",
    L"Instrumental",
    L"Acid",
    L"House",
    L"Game",
    L"SoundClip",
    L"Gospel",
    L"Noise",
    L"AlternRock",
    L"Bass",
    L"Soul",
    L"Punk",
    L"Space",
    L"Meditative",
    L"InstrumentalPop",
    L"InstrumentalRock",
    L"Ethnic",
    L"Gothic",
    L"Darkwave",
    L"Techno-Industrial",
    L"Electronic",
    L"Pop-Folk",
    L"Eurodance",
    L"Dream",
    L"SouthernRock",
    L"Comedy",
    L"Cult",
    L"Gangsta",
    L"Top40",
    L"ChristianRap",
    L"Pop/Funk",
    L"Jungle",
    L"NativeAmerican",
    L"Cabaret",
    L"NewWave",
    L"Psychadelic",
    L"Rave",
    L"Showtunes",
    L"Trailer",
    L"Lo-Fi",
    L"Tribal",
    L"AcidPunk",
    L"AcidJazz",
    L"Polka",
    L"Retro",
    L"Musical",
    L"Rock&Roll",
    L"HardRock",
    L"Folk",
    L"Folk-Rock",
    L"NationalFolk",
    L"Swing",
    L"FastFusion",
    L"Bebob",
    L"Latin",
    L"Revival",
    L"Celtic",
    L"Bluegrass",
    L"Avantgarde",
    L"GothicRock",
    L"ProgessiveRock",
    L"PsychedelicRock",
    L"SymphonicRock",
    L"SlowRock",
    L"BigBand",
    L"Chorus",
    L"EasyListening",
    L"Acoustic",
    L"Humour",
    L"Speech",
    L"Chanson",
    L"Opera",
    L"ChamberMusic",
    L"Sonata",
    L"Symphony",
    L"BootyBass",
    L"Primus",
    L"PornGroove",
    L"Satire",
    L"SlowJam",
    L"Club",
    L"Tango",
    L"Samba",
    L"Folklore",
    L"Ballad",
    L"PowerBallad",
    L"RhythmicSoul",
    L"Freestyle",
    L"Duet",
    L"PunkRock",
    L"DrumSolo",
    L"Acapella",
    L"Euro-House",
    L"DanceHall",
    L"Goa",
    L"Drum&Bass",
    L"Club-House",
    L"Hardcore",
    L"Terror",
    L"Indie",
    L"BritPop",
    L"Negerpunk",
    L"PolskPunk",
    L"Beat",
    L"ChristianGangstaRap",
    L"HeavyMetal",
    L"BlackMetal",
    L"Crossover",
    L"ContemporaryChristian",
    L"ChristianRock",
    L"Merengue",
    L"Salsa",
    L"TrashMetal",
    L"Anime",
    L"JPop",
    L"Synthpop",
};

class CCommon
{
public:
    CCommon();
    ~CCommon();

    //��string���͵��ַ���ת����Unicode�����wstring�ַ���
    static std::wstring StrToUnicode(const std::string& str, CodeType code_type = CodeType::AUTO);

    //��Unicode�����wstring�ַ���ת����string���͵��ַ�����������ַ��޷�ת����������char_cannot_convertָ���bool������Ϊtrue
    static std::string UnicodeToStr(const std::wstring & wstr, CodeType code_type, bool* char_cannot_convert = nullptr);

    //�ж�һ���ַ����ı����ʽ
    static CodeType JudgeCodeType(const std::string& str, CodeType default_code = CodeType::ANSI);

    //��ȡ��ʱ�ļ���·��
    static std::wstring GetTemplatePath();

    //ɾ��һ���ַ����зǴ�ӡ�ַ���ʼ�ĺ���ȫ���ַ�
    static void TagStrNormalize(std::wstring& str);

    //�����ļ����ж��ļ�������
    static AudioType GetAudioTypeByExtension(const std::wstring& file_name);

    //��ñ�׼������Ϣ
    static std::wstring GetGenre(BYTE genre);

    //�Զ�����ַ�����������
    static void StringCopy(char* dest, size_t size, std::string source);

    //��һ���ַ����ָ�����ɸ��ַ���
    //str: ԭʼ�ַ���
    //div: ���ڷָ���ַ���
    //result: ���շָ��Ľ��
    static void StringSplit(const std::string& str, const std::string& div, std::vector<std::string>& results);

    template<class T>
    static bool StringTransform(T & str, bool upper)
    {
        if (str.empty()) return false;
        for (auto& ch : str)
        {
            if (upper)
            {
                if (ch >= 'a' && ch <= 'z')
                    ch -= 32;
            }
            else
            {
                if (ch >= 'A' && ch <= 'Z')
                    ch += 32;
            }
        }
        return true;
    }

    template<class T>
    static bool StringCompareNoCase(const T & str1, const T & str2)
    {
        T _str1{ str1 }, _str2{ str2 };
        StringTransform(_str1, false);
        StringTransform(_str2, false);
        return (_str1 == _str2);
    }

    template<class T>
    static size_t StringFindNoCase(const T & str, const T & find_str)
    {
        if (str.empty() || find_str.empty())
            return -1;
        T _str{ str }, _find_str{ find_str };
        StringTransform(_str, false);
        StringTransform(_find_str, false);
        return _str.find(_find_str);
    }

};

