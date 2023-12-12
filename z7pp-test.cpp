// z7pp-test.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include <7zpp.h>

using namespace SevenZip;

int main()
{
    SevenZipLibrary lib;
    lib.Load(L"D:\\Program Files\\7-Zip\\7z.dll");
    SevenZipCompressor z(lib, L"D:\\mensong.7z");
    z.AddDirectory(L"D:\\Programming\\Git\\7zip-cpp\\7zpp");
    z.SetCompressionFormat(CompressionFormat::SevenZip);
    z.SetCompressionLevel(CompressionLevel::Normal);
    z.SetPassword(L"mensong");
    z.DoCompress();
}
