//
// Author: hujiagao@gmail.com
//

#pragma once

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
using namespace std;

typedef pair<string, vector<float>> SegReq;		// 分割需求信息，分割算法名+参数列表

#define COMMENT_CHAR '#'

class ConfigReader
{
public:
	ConfigReader(void);
	ConfigReader(const string&);
	~ConfigReader(void);

	void GetSegRequire(vector<SegReq>& );

private:
	void ReadConfig();
	void Trim(string & str);
	bool IsSpace(char c);

	string m_filename;

	vector<SegReq> segList;
};

