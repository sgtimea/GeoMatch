#pragma once

//此文件用于txt文本的各项操作
#include<string>
#include<iostream>
#include<fstream>
#include<sstream>
using namespace std;

    //inline可以让头文件多次被包含而不出现重定义
    //计算txt行数
inline  int CountLines(const string &filename)
    {
        ifstream ReadFile;
        int n = 0;
        string tmp;
        ReadFile.open(filename.c_str());//ios::in 表示以只读的方式读取文件
        if (ReadFile.fail())//文件打开失败:返回0
        {
            return 0;
        }
        else//文件存在
        {
            while (getline(ReadFile, tmp, '\n'))
            {
                n++;
            }
            ReadFile.close();
            return n;
        }
    }


    //读取txt指定行内容，以string返回,第一行开始
inline  string ReadLine(const string &filename,const int &line)
    {
        int lines, i = 0;
        string temp;
        fstream file;
        file.open(filename.c_str());
        lines = CountLines(filename);

        if (line <= 0)
        {
            return "Error 1: 行数错误，不能为0或负数。";
        }
        if (file.fail())
        {
            return "Error 2: 文件不存在。";
        }
        if (line > lines)
        {
            return "Error 3: 行数超出文件长度。";
        }
        while (getline(file, temp) && i < line - 1)
        {
            i++;
        }
        file.close();
        return temp;
    }


               /**       查找字符串内子串出现的所有下标     **/
inline std::vector<int32_t> find_pix(const string& src, const string& tar){
    std::vector<int32_t> pix; int num=0;
    while (num < src.length()) {
        if (src.find(tar, num) < src.length())
        {
            num = src.find(tar, num);
            pix.push_back(num);
            num++;
        }
        else
            break;
    }
    return pix;
}

               /**      截取字符串中间一段，包含两个下标     **/
inline string substring(const string& s, int32_t i, int32_t j)
{
    if (i >= s.length() || j >= s.length())
    {
        return"下标超出字符串长度" ;
    }
    else if (i > j)
    {
        return "前标不允许大于后标" ;
    }
    string t;
    for (; i <= j; i++) {
        t.push_back(s[i]);
    }
    return t;
}