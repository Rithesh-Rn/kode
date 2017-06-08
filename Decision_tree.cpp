#include <iostream>
#include <ctime>
#include <fstream>
#include <vector>
#include <string.h>
#include<stdlib.h>
#include <windows.h>
#include <direct.h>
#include <shlobj.h>
#include <sstream>
#include <tchar.h>
#pragma warning(disable : 4996)
using namespace std;

vector<int> line;


int L1F = 0, L2F = 0, L3F = 0, L4F = 0;
//int LC[0] = 0, LC[1] = 0, LC[2] = 0, LC[3] = 0;
int LC[4];
int l;
time_t start;



void readData()
{
	int a;
	/*
	line.clear();
	ifstream myfile1("data.txt" , std::ios_base::in);
	if (myfile1.is_open())
	{
		int i = 0;
	//while (getline(myfile1, l))
	while(myfile1 >> a)
	{
	//line.push_back(a);
		LC[i++] = a;
	}
	myfile1.close();
	}
	*/


	std::ifstream file;

	TCHAR appData[MAX_PATH];
	if (SUCCEEDED(SHGetFolderPath(NULL,
		CSIDL_DESKTOPDIRECTORY | CSIDL_FLAG_CREATE,
		NULL,
		SHGFP_TYPE_CURRENT,
		appData)))

		std::wcout << appData << std::endl;
	std::basic_ostringstream<TCHAR> file_path;
	file_path << appData << _TEXT("\\filename.txt");

	file.open(file_path.str().c_str());
	int i = 0;
	while (file >> a)
	{
		LC[i++] = a;
	}
	file.close();
	
}
void writeL2L4()
{
	start = time(0);
	std::ofstream myfile;
	myfile.open("signal.txt", std::ios_base::app);
	time_t nnow = time(0);
	char* dt = ctime(&nnow);
	myfile << dt;
	myfile << "Lane 1 -> RED " << std::endl;
	myfile << "Lane 2 -> GREEN " << std::endl;
	myfile << "Lane 3 -> RED " << std::endl;
	myfile << "Lane 4 -> GREEN " << std::endl;
	//seconds_since_start = difftime(time(0), start);

	myfile.close();
	Sleep(10000);

}

void writeL1L3()
{
	start = time(0);
	std::ofstream myfile;
	myfile.open("signal.txt", std::ios_base::app);
	time_t nnow = time(0);
	char* dt = ctime(&nnow);
	myfile << dt;
	myfile << "Lane 1 -> GREEN " << std::endl;
	myfile << "Lane 2 -> RED " << std::endl;
	myfile << "Lane 3 -> GREEN " << std::endl;
	myfile << "Lane 4 -> RED " << std::endl;
	//seconds_since_start = difftime(time(0), start);

	myfile.close();
	Sleep(10000);
}


//This Funtion Starts the first decision of Descision tree ie when L2 AND L4 IS TO BE SELECTED FIRST
void goWithL2L4()
{
	//write output file L2 AND L4 green
	writeL2L4();
	//read from input file and assign it to vector line
	readData();

	/*LC[0] = line[0];
	LC[1] = line[1];
	LC[2] = line[2];
	LC[3] = line[3];
	*/
	if (LC[1] == 0)
	{
		//make L1 and L3 green
		writeL1L3();
		readData();

		LC[0] = line[0];
		LC[1] = line[1];
		LC[2] = line[2];
		LC[3] = line[3];

		if (LC[0] == 0)
		{
			//Make L2 L4 green
			writeL2L4();

		}
		else
		{
			writeL1L3();
		}

	}
	else if(LC[1] > 0)
	{
		writeL2L4();
		readData();
		/*LC[0] = line[0];
		LC[1] = line[1];
		LC[2] = line[2];
		LC[3] = line[3];*/
		if (LC[1] == 0)
		{
			writeL1L3();
		}
		else
		{
			writeL2L4();
			L2F = L4F = 1;
		}
	}
}


void goWithL1L3()
{
	writeL1L3();
	readData();

	/*LC[0] = line[0];
	LC[1] = line[1];
	LC[2] = line[2];
	LC[3] = line[3];*/
	if (LC[0] == 0)
	{
		//make L1 and L3 green
		writeL2L4();
		readData();

		/*LC[0] = line[0];
		LC[1] = line[1];
		LC[2] = line[2];
		LC[3] = line[3];*/

		if (LC[1] == 0)
		{
			//Make L2 L4 green
			writeL1L3();

		}
		else
		{
			writeL2L4();
		}

	}
	else
	{
		writeL1L3();
		readData();
/*
		LC[0] = line[0];
		LC[1] = line[1];
		LC[2] = line[2];
		LC[3] = line[3];*/
		if (LC[0] == 0)
		{
			writeL2L4();
		}
		else
		{
			writeL1L3();
			L1F = L3F = 1;
		}
	}

}


int decision()
{
	
	int res;
	float g = 0.0;
	readData();
	/*
	LC[0] = line[0];
	LC[1] = line[1];
	LC[2] = line[2];
	LC[3] = line[3];*/

	int L1 = LC[0] + LC[2];
	int L2 = LC[1] + LC[3];

	g = (L2 / L1) - 1;
	if ((g > 0 && g < 10) || (g < -1))
		res = 2;
	else// if ((g > 0.1 && g <0) || (g > 10))
		res = 1;
	return res;

}


int main(void)
{
	double seconds_since_start;

	//DEcision tree double seconds_since_start = difftime(time(0), start);
	while (1)
	{

		if (((L1F == 1) && (L3F == 1)) || ((L2F == 1) && (L4F == 1)))
		{
			if (((L1F == 1) && (L3F == 1)))
			{
				L1F = 0; L3F = 0;
				goWithL2L4();

			}
			else
			{
				L2F = 0; L4F = 0;
				goWithL1L3();
			}

		}
		else
		{
			int d = decision();
			if (d == 1)//decision fn i.e g(x) returns 1 or 2 respectively.
			{
				goWithL1L3();

			}
			else if (d == 2)
			{
				goWithL2L4();
			}

		}

	}
	return(0);
}
