#include <iostream>
#include <cstring>

using namespace std;

int main(int argc, char const *argv[])
{
	string line = "haha";
	if(line.substr(0, 3) == "hah") cout << "haha" << endl;
	return 0;
}