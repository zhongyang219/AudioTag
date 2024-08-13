// test.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include "pch.h"
#include <iostream>
#include "../AudioTag/AudioTag.h"

static void PrintAudioInfo(const std::wstring& file_path)
{
    std::wcout << L"file path: " << file_path << std::endl;
    //测试标签读取
    auto pAudioTag = ATCreateInstance(file_path);
    pAudioTag->GetAudioTag(true);
    AudioInfo info = pAudioTag->GetAudioInfo();
    std::wcout << L"title: " << info.title << std::endl;
    std::wcout << L"artist: " << info.artist << std::endl;
    std::wcout << L"album: " << info.album << std::endl;
    std::wcout << L"year: " << info.year << std::endl;
    std::wcout << L"comment: " << info.comment << std::endl;
    std::wcout << L"genre: " << info.genre << std::endl;
    std::cout << "track:" << info.track << std::endl;
    std::cout << "tag_type:" << info.tag_type << std::endl;

    //获取专辑封面
    int img_type{ -1 };
    std::wstring cover_path = pAudioTag->GetAlbumCover(img_type);
    std::wcout << L"cover path: " << cover_path << std::endl;
    std::cout << "cover type: ";
    switch (img_type)
    {
    case 0:
        std::cout << "jpg";
        break;
    case 1:
        std::cout << "png";
        break;
    case 2:
        std::cout << "gif";
        break;
    default:
        break;
    }

    std::cout << std::endl;
    std::cout << std::endl;

    delete pAudioTag;
}

int main()
{
    std::wcout.imbue(std::locale("chs"));
    PrintAudioInfo(L"D:\\其他文件\\音频测试\\μ's - 友情ノーチェンジ.mp3");
    PrintAudioInfo(L"D:\\其他文件\\音频测试\\Amanda.wma");

    ////测试ID3V1写入
    //AudioInfo testInfo;
    //testInfo.title = L"title!@##$$";
    //testInfo.artist = L"artist#$%^^!@#$";
    //testInfo.year = L"1222";
    //testInfo.comment = L"asdfghjkl;";
    //bool text_cut_off;
    //pAudioTag->WriteMp3Tag(testInfo, text_cut_off);


}

// 运行程序: Ctrl + F5 或调试 >“开始执行(不调试)”菜单
// 调试程序: F5 或调试 >“开始调试”菜单

// 入门提示:
//   1. 使用解决方案资源管理器窗口添加/管理文件
//   2. 使用团队资源管理器窗口连接到源代码管理
//   3. 使用输出窗口查看生成输出和其他消息
//   4. 使用错误列表窗口查看错误
//   5. 转到“项目”>“添加新项”以创建新的代码文件，或转到“项目”>“添加现有项”以将现有代码文件添加到项目
//   6. 将来，若要再次打开此项目，请转到“文件”>“打开”>“项目”并选择 .sln 文件
