#pragma once
#include "AudioTag.h"

class CAudioTag : public IAudioTag
{
public:
    CAudioTag(const std::wstring& file_path);

    //获取音频文件的标签信息，结果保存在构造函数传递进来的AudioInfo结构里，
    //id3v2_first：是否优先获取ID3V2标签，否则，优先获取ID3V1标签
    void GetAudioTag(bool id3v2_first) override;

    std::wstring GetAlbumCover(int& image_type, wchar_t* file_name = nullptr) override;

    virtual bool WriteMp3Tag(const AudioInfo& song_info, bool& text_cut_off) override;

    AudioType GetAudioType() const override
    {
        return m_type;
    }

    const AudioInfo& GetAudioInfo() const override;

    ~CAudioTag();

private:
    std::wstring m_file_path;
    AudioInfo m_song_info;
    AudioType m_type;

    bool GetID3V1Tag();
    bool GetID3V2Tag();
    bool GetWmaTag();
    //bool GetMp4Tag();
    //bool GetOggTag();
    bool GetFlacTag();

    //获取音频的标签区域的内容
    void GetFlacTagContents(std::string& contents_buff);
    void GetID3V2TagContents(std::string& contents_buff);
    void GetID3V1TagContents(std::string& contents_buff);
    void GetWmaTagContents(std::string& contents_buff);

    static std::wstring _GetAlbumCover(const std::string& tag_content, size_t cover_index, int& image_type, wchar_t* file_name = nullptr);

};

