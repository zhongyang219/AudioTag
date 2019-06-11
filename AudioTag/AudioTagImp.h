#pragma once
#include "AudioTag.h"

class CAudioTag : public IAudioTag
{
public:
    CAudioTag(const std::wstring& file_path);

    //��ȡ��Ƶ�ļ��ı�ǩ��Ϣ����������ڹ��캯�����ݽ�����AudioInfo�ṹ�
    //id3v2_first���Ƿ����Ȼ�ȡID3V2��ǩ���������Ȼ�ȡID3V1��ǩ
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

    //��ȡ��Ƶ�ı�ǩ���������
    void GetFlacTagContents(std::string& contents_buff);
    void GetID3V2TagContents(std::string& contents_buff);
    void GetID3V1TagContents(std::string& contents_buff);
    void GetWmaTagContents(std::string& contents_buff);

    static std::wstring _GetAlbumCover(const std::string& tag_content, size_t cover_index, int& image_type, wchar_t* file_name = nullptr);

};

