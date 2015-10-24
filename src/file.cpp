#include <file.h>
#include <fstream>

using namespace std;

char* read_file(const char* filename)
{
    ifstream ifs(filename);
    string s;
    if(ifs.is_open())
    {
        s.assign((istreambuf_iterator<char>(ifs.rdbuf())), istreambuf_iterator<char>());
        ifs.close();
    }
    else
        cerr << "Unable to open file" << endl;
    char* ret = new char[s.length()];
    for (int i = 0; i < s.length(); i++)
    {
        ret[i] = s[i];
    }
    return ret;

}
