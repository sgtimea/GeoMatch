#pragma once

//���ļ�����txt�ı��ĸ������
#include<string>
#include<iostream>
#include<fstream>
#include<sstream>
using namespace std;

    //inline������ͷ�ļ���α��������������ض���
    //����txt����
inline  int CountLines(const string &filename)
    {
        ifstream ReadFile;
        int n = 0;
        string tmp;
        ReadFile.open(filename.c_str());//ios::in ��ʾ��ֻ���ķ�ʽ��ȡ�ļ�
        if (ReadFile.fail())//�ļ���ʧ��:����0
        {
            return 0;
        }
        else//�ļ�����
        {
            while (getline(ReadFile, tmp, '\n'))
            {
                n++;
            }
            ReadFile.close();
            return n;
        }
    }


    //��ȡtxtָ�������ݣ���string����,��һ�п�ʼ
inline  string ReadLine(const string &filename,const int &line)
    {
        int lines, i = 0;
        string temp;
        fstream file;
        file.open(filename.c_str());
        lines = CountLines(filename);

        if (line <= 0)
        {
            return "Error 1: �������󣬲���Ϊ0������";
        }
        if (file.fail())
        {
            return "Error 2: �ļ������ڡ�";
        }
        if (line > lines)
        {
            return "Error 3: ���������ļ����ȡ�";
        }
        while (getline(file, temp) && i < line - 1)
        {
            i++;
        }
        file.close();
        return temp;
    }


               /**       �����ַ������Ӵ����ֵ������±�     **/
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

               /**      ��ȡ�ַ����м�һ�Σ����������±�     **/
inline string substring(const string& s, int32_t i, int32_t j)
{
    if (i >= s.length() || j >= s.length())
    {
        return"�±곬���ַ�������" ;
    }
    else if (i > j)
    {
        return "ǰ�겻������ں��" ;
    }
    string t;
    for (; i <= j; i++) {
        t.push_back(s[i]);
    }
    return t;
}