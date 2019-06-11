#pragma once
#include "stdafx.h"

struct AudioInfo
{
    std::wstring title;		//����
    std::wstring artist;		//������
    std::wstring album;		//��Ƭ��
    std::wstring year;		//���
    std::wstring comment;	//ע��
    std::wstring genre;		//����
    BYTE genre_idx{ 255 };		//���ֽڱ�ʾ�����ɺ�
    int track{};		//�������
    int tag_type{};		//��ǩ�����ͣ�0��������1��ID3v1��2��ID3v2��

};

//��Ƶ�ļ�����
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
    //��ȡ��Ƶ�ļ��ı�ǩ��Ϣ
    //id3v2_first���Ƿ����Ȼ�ȡID3V2��ǩ���������Ȼ�ȡID3V1��ǩ
    virtual void GetAudioTag(bool id3v2_first) = 0;

    //��ȡ��Ƶ�ļ���ר�����棬�����浽��ʱĿ¼
    //image_type���������շ���ĸ�ʽ 0:jpg, 1:png, 2:gif
    //file_name: ָ�������ר��������ļ��������Ϊnullptr����ʹ��Ĭ�ϵ��ļ���
    //����ֵ��ר������ı���·��
    virtual std::wstring GetAlbumCover(int& image_type, wchar_t* file_name = nullptr) = 0;

    //���ػ�ȡ������Ƶ��Ϣ
    virtual const AudioInfo& GetAudioInfo() const = 0;

    //������Ƶ�ļ�������
    virtual AudioType GetAudioType() const = 0;

    //��һ��MP3�ļ�д��ID3V1��ǩ
    //file_path��mp3�ļ���·��
    //text_cut_off�����д����ı����ȳ���ID3V1�����ɵĳ��ȣ���������ı����ᱻ�ضϣ�����text_cut_off��Ϊtrue
    //����ֵ���ɹ�����true�����򷵻�false
    virtual bool WriteMp3Tag(const AudioInfo & song_info, bool & text_cut_off) = 0;

};

__declspec(dllexport) std::shared_ptr<IAudioTag> ATCreateInstance(const std::wstring& file_path);
