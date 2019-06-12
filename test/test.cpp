// test.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include "pch.h"
#include <iostream>
#include "../AudioTag/AudioTag.h"

int main()
{
    //测试标签读取
    auto pAudioTag = ATCreateInstance(L"D:\\Temp\\Six002凤凌长空．演奏曲.ogg");
    pAudioTag->GetAudioTag(true);
    AudioInfo info = pAudioTag->GetAudioInfo();

    //测试ID3V1写入
    AudioInfo testInfo;
    testInfo.title = L"title!@##$$";
    testInfo.artist = L"artist#$%^^!@#$";
    testInfo.year = L"1222";
    testInfo.comment = L"asdfghjkl;";
    bool text_cut_off;
    pAudioTag->WriteMp3Tag(testInfo, text_cut_off);

    //获取专辑封面
    int img_type;
    std::wstring cover_path = pAudioTag->GetAlbumCover(img_type);

    std::cout << "Hello World!\n";
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
