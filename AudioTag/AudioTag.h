#pragma once
#include "stdafx.h"

struct AudioInfo
{
    std::wstring title;		//标题
    std::wstring artist;		//艺术家
    std::wstring album;		//唱片集
    std::wstring year;		//年份
    std::wstring comment;	//注释
    std::wstring genre;		//流派
    BYTE genre_idx{ 255 };		//以字节表示的流派号
    int track{};		//音轨序号
    int tag_type{};		//标签的类型（0：其他；1：ID3v1；2：ID3v2）

};

//音频文件类型
enum AudioType
{
    AU_MP3,
    AU_WMA,
    AU_OGG,
    AU_MP4,
    AU_APE,
    AU_FLAC,
    AU_OTHER
};

class __declspec(dllexport) IAudioTag
{
public:
    //获取音频文件的标签信息
    //id3v2_first：是否优先获取ID3V2标签，否则，优先获取ID3V1标签
    virtual void GetAudioTag(bool id3v2_first) = 0;

    //获取音频文件的专辑封面，并保存到临时目录
    //image_type：用来接收封面的格式 0:jpg, 1:png, 2:gif
    //file_name: 指定保存的专辑封面的文件名，如果为nullptr，则使用默认的文件名
    //返回值：专辑封面的保存路径
    virtual std::wstring GetAlbumCover(int& image_type, wchar_t* file_name = nullptr) = 0;

    //返回获取到的音频信息
    virtual const AudioInfo& GetAudioInfo() const = 0;

    //返回音频文件的类型
    virtual AudioType GetAudioType() const = 0;

    //向一个MP3文件写入ID3V1标签
    //file_path：mp3文件的路径
    //text_cut_off：如果写入的文本长度超过ID3V1可容纳的长度，则过长的文本将会被截断，并将text_cut_off置为true
    //返回值：成功返回true，否则返回false
    virtual bool WriteMp3Tag(const AudioInfo & song_info, bool & text_cut_off) = 0;

};

__declspec(dllexport) std::shared_ptr<IAudioTag> ATCreateInstance(const std::wstring& file_path);
