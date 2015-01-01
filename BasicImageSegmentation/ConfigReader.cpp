#include "ConfigReader.h"


ConfigReader::ConfigReader(void)
{
	m_filename = "config.ini";
	ReadConfig();
}

ConfigReader::ConfigReader(const string& _name)
{
	m_filename = _name;
	ReadConfig();
}

ConfigReader::~ConfigReader(void)
{
}

void ConfigReader::GetSegRequire(vector<SegReq>& _req)
{
	_req = segList;
}

void ConfigReader::ReadConfig()
{
	ifstream conFile(m_filename, ios::in);
	if (!conFile)
	{
		cout<<"--Error: Failed to read config!"<<endl;
	}
	string line;
	SegReq req;
	while(getline(conFile, line))
	{
		Trim(line);
		if (line.empty() || COMMENT_CHAR == line[0])
			continue;
		if ('[' == line[0])
		{
			if (!req.first.empty() && req.second.empty())
			{
				segList.push_back(req);
			}
			int start = 1;
			int end = line.find(']');
			if (end == string::npos)
				continue;
			string segName = line.substr(start, end-1);
			Trim(segName);
			req.first = segName;
		} else {
			stringstream ss;
			vector<float> args;
			ss<<line;
			float arg;
			while(ss>>arg) {
				args.push_back(arg);
			}
			req.second = args;
			segList.push_back(req);
			req.first.clear();
			req.second.clear();
		}
	}
	if (!req.first.empty() && req.second.empty())
	{
		segList.push_back(req);
	}

	conFile.close();
}

void ConfigReader::Trim(string & str)
{
    if (str.empty()) {
        return;
    }
    int i, start_pos, end_pos;
    for (i = 0; i < str.size(); ++i) {
        if (!IsSpace(str[i])) {
            break;
        }
    }
    if (i == str.size()) {
        str = "";
        return;
    }
    
    start_pos = i;
    
    for (i = str.size() - 1; i >= 0; --i) {
        if (!IsSpace(str[i])) {
            break;
        }
    }
    end_pos = i;
    
    str = str.substr(start_pos, end_pos - start_pos + 1);
}

bool ConfigReader::IsSpace(char c)
{
    if (' ' == c || '\t' == c)
        return true;
    return false;
}
