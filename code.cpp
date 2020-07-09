
#include <iostream>
#include <vector>
#include <map>
#include <set>
#include <queue>
#include <algorithm>
#include <bitset>
#include <fstream>
#include <string>
using namespace std;
typedef unsigned long long Pos;
typedef unsigned char Line;
int lineTypeCount = 0;
const int searchOrder[12] = { 2,8,4,10,1,7,3,9,0,6,5,11 };
const unsigned char LineType[] = { 0,1,1,1,1,1,2,2,2,2,3,3,3,3,3,3,4,4,4,5,5,5 };
long long stepsPosesCount[10000];//不同步数的局面总数
int maxStepsCount = 0;
vector<Line> allSingleMovesAvailable[22][64];//22=lineType, 64=occupied
struct node
{
	Pos pos;
	unsigned short minSteps;
};
struct complexLine
{
	int blockNum;
	int block1Length;
	int block2Length;
	int block1Place;
	int block2Place;
	//  block1Place block1Length block2Place block2Length 6-block1Place-block1Length-block2Place-block2Length
	complexLine(int bn, int a, int b, int c, int d) :blockNum(bn), block1Length(a), block2Length(b), block1Place(c), block2Place(d) {}
};
vector<complexLine> allComplexLines;
unsigned char occupiedPlaces[22];

void init_allSingleMovesAvailable()
{
	//不用担心跨块挪动的情况
	const unsigned char simpleOccupiedConditions[] = { 0b0,0b000001,0b100000,0b010000,0b000010,0b110000,0b101000,0b100010,0b100001,0b011000,0b010001,0b000110,0b000101,0b000011,0b111000,0b110001,0b100011,0b000111 };
	const int simpleOccupiedConditionsNum = 18;
	for (int conID = 0; conID < simpleOccupiedConditionsNum; conID++)
	{
		unsigned char occ = simpleOccupiedConditions[conID];
		for (int i = 0; i < 22; i++)
			for (int j = 0; j < 22; j++)
			{
				if(!((occupiedPlaces[i]&occ)|| (occupiedPlaces[j] & occ)))
				if (i != j && allComplexLines[i].block1Length == allComplexLines[j].block1Length&&allComplexLines[i].block2Length == allComplexLines[j].block2Length)
				{
					if (allComplexLines[i].blockNum == 2)
					{
						if (allComplexLines[i].block1Place == allComplexLines[j].block1Place || (allComplexLines[i].block2Place + allComplexLines[i].block1Place == allComplexLines[j].block2Place + allComplexLines[j].block1Place))
						{
							allSingleMovesAvailable[i][occ].push_back(j);
						//	cout << "i" << i << "j" << j << "occ" << bitset<6>(occ) << endl;
						}
					}
					else if (allComplexLines[i].blockNum == 1)
					{
						allSingleMovesAvailable[i][occ].push_back(j);
					//	cout << "i" << i << "j" << j << "occ" << bitset<6>(occ) << endl;

					}
				}
			}
	}


	//special condition:001000 000100
	allSingleMovesAvailable[1][0b001000].push_back(2);
	allSingleMovesAvailable[2][0b001000].push_back(1);
	allSingleMovesAvailable[4][0b000100].push_back(5);
	allSingleMovesAvailable[5][0b000100].push_back(4);
	allSingleMovesAvailable[11][0b000100].push_back(12);
	allSingleMovesAvailable[12][0b000100].push_back(11);
	allSingleMovesAvailable[12][0b001000].push_back(14);
	allSingleMovesAvailable[14][0b001000].push_back(12);
}
Line getLine(Pos pos, unsigned char lineID)//获取一行或者一列中，被非这行/列的块所挡住的区域
{
	return (pos >> (5 * lineID)) & 0b11111;
}
Pos setLine(Pos pos, unsigned char lineID,Line line)//获取一行或者一列中，被非这行/列的块所挡住的区域
{
	Pos mask = 0b11111;
	mask = mask << (5 * lineID);
	mask = ~mask;
	pos &= mask;
	mask = line;
	mask = mask << (5 * lineID);
	pos |= mask; 
	return pos;

}
Pos getTypeID(Pos pos)
{
	Pos type = 0;
	for (int i = 0; i < 12; i++)
	{
		type = setLine(type, i, LineType[getLine(pos, i)]);
	}
	return type;
}
bool checkLegal(Pos pos)
{
	bool legalMap[6][6];
	for (int i = 0; i < 36; i++)legalMap[i / 6][i % 6]=false;
	for (int i = 0; i < 6; i++)
	{
		unsigned char occupied = occupiedPlaces[getLine(pos, i)];
		for (int j = 0; j < 6; j++)
		{
			if ((occupied >> j) & 0b1)
				legalMap[i][j] = true;
		}
	}
	for (int i = 0; i < 6; i++)
	{
		unsigned char occupied = occupiedPlaces[getLine(pos, i+6)];
		for (int j = 0; j < 6; j++)
		{
			if (((occupied >> j) & 0b1) && legalMap[j][i])return false;
		}
	}
	return true;
}
void printPos(Pos pos)
{
	cout <<"ID:"<< pos << endl;
	char printMap[6][6];
	for (int i = 0; i < 36; i++)printMap[i / 6][i % 6] = ' ';
	for (int i = 0; i < 6; i++)
	{
		complexLine cl = allComplexLines[getLine(pos, i )];
		for (int j = cl.block1Place; j < cl.block1Place + cl.block1Length; j++)
		{
			printMap[i][j] = 'a' + i ;

		}
		for (int j = cl.block2Place + cl.block1Place + cl.block1Length; j < cl.block2Place + cl.block2Length + cl.block1Place + cl.block1Length; j++)
		{
			printMap[i][j] = 'a' + i + 12;

		}
	}
	for (int i = 0; i < 6; i++)
	{
		complexLine cl = allComplexLines[getLine(pos, i + 6)];
		for (int j = cl.block1Place; j < cl.block1Place + cl.block1Length; j++)
		{
			printMap[j][i] = 'a' + i + 6;

		}
		for (int j = cl.block2Place+ cl.block1Place + cl.block1Length; j < cl.block2Place + cl.block2Length + cl.block1Place + cl.block1Length; j++)
		{
			printMap[j][i] = 'a' + i + 18;

		}
	}
	for (int y = 0; y < 6; y++)
	{
		for (int x = 0; x < 6; x++)
		{
			cout << printMap[y][5 - x];
		}
		cout << endl;
	}

	cout << endl;
	//	return true;
}
unsigned char getLineOccupied(Pos pos,unsigned char lineID)//获取一行或者一列中，被非这行/列的块所挡住的区域
{
	bool occupied[6] = { false, false, false, false, false, false };
	if (lineID < 6)//行
	{
		pos = pos >> (5 * 6);
		for (int i = 0; i < 6; i++)
		{
			Line thisLine = pos & 0b11111;
			occupied[i] = (occupiedPlaces[thisLine] >> lineID) & 0b1;
			pos = pos >> 5;

		}
	}
	else if (lineID < 12)//列
	{
		for (int i = 0; i < 6; i++)
		{
			Line thisLine = pos & 0b11111;
			occupied[i] = (occupiedPlaces[thisLine] >>( lineID-6)) & 0b1;
			pos = pos >> 5;

		}
	}

	unsigned char c = 0;
	for (int p = 0; p < 6; p++)c |= (occupied[p] << p);
	return c;
}
void getAllAvailableNextPoses(Pos pos, vector<Pos>* list)
{
	for (int lineID = 0; lineID < 12; lineID++)
	{
		Line line = getLine(pos, lineID);
		unsigned char occ = getLineOccupied(pos, lineID);
		for (int i = 0; i < allSingleMovesAvailable[line][occ].size(); i++)
		{
			list->push_back(setLine(pos, lineID, allSingleMovesAvailable[line][occ][i]));
		}
	}
}
void findAllLegalPoses(int calculatedLineNum, Pos posNow,short forcedZeros, vector<Pos>* initialList, set<Pos>* initialSet)
{
	if (calculatedLineNum == 12)
	{

		initialList->push_back(posNow);
		initialSet->insert(posNow);
		return;
	}
	int LineID = searchOrder[calculatedLineNum];
	unsigned char occupied = getLineOccupied(posNow,LineID);
	if((forcedZeros>> LineID)&1)findAllLegalPoses(calculatedLineNum + 1, posNow, forcedZeros,  initialList,  initialSet);

	else
		for (int i = 1; i < lineTypeCount; i++)
	{
		if (!(occupied&occupiedPlaces[i]))
		{
			findAllLegalPoses(calculatedLineNum + 1, setLine(posNow, LineID, i), forcedZeros, initialList, initialSet);
		}
	//	else illegalPos++;
	}
	
}
void init_allFinishedPoses(short forcedZeros, vector<Pos>* initialList, set<Pos>* initialSet)
{
	Pos pos = 0;
	pos = setLine(pos, searchOrder[0], 1);
	findAllLegalPoses(1, pos, forcedZeros, initialList,initialSet);
}
const string line2str[]=
{

 "0" , 
 "20000" , 
 "02000" , 
 "00200" , 
 "00020" , 
 "00002" , 
 "3000" , 
 "0300" , 
 "0030" , 
 "0003" , 
 "2200" , 
 "2020" , 
 "2002" , 
 "0220" , 
 "0202" , 
 "0022" , 
 "230" , 
 "203" , 
 "023" , 
 "320" , 
 "302" , 
 "032" 
};
void init_allComplexLines()
{
	allComplexLines.push_back(complexLine(0, 0, 0, 0, 0));
	for (int i = 0; i < 5; i++)allComplexLines.push_back(complexLine(1, 2, 0, i, 0));
	for (int i = 0; i < 4; i++)allComplexLines.push_back(complexLine(1, 3, 0, i, 0));
	int b1L, b2L;
	b1L = 2, b2L = 2;
	for (int i = 0; i <= 6 - b1L - b2L; i++)
		for (int j = 0; j <= 6 - b1L - b2L - i; j++)
			allComplexLines.push_back(complexLine(2, b1L, b2L, i, j));
	b1L = 2, b2L = 3;
	for (int i = 0; i <= 6 - b1L - b2L; i++)
		for (int j = 0; j <= 6 - b1L - b2L - i; j++)
			allComplexLines.push_back(complexLine(2, b1L, b2L, i, j));
	b1L = 3, b2L = 2;
	for (int i = 0; i <= 6 - b1L - b2L; i++)
		for (int j = 0; j <= 6 - b1L - b2L - i; j++)
			allComplexLines.push_back(complexLine(2, b1L, b2L, i, j));

	lineTypeCount = allComplexLines.size();
}
void init_occupiedPlaces()
{
	for (int i = 0; i < lineTypeCount; i++)
	{
		bool occupied[6] = { false, false, false, false, false, false };
		int p0 = 0;
		p0 += allComplexLines[i].block1Place;
		for (int p = 0; p < allComplexLines[i].block1Length; p++)occupied[p + p0] = true;
		p0 += allComplexLines[i].block1Length;
		p0 += allComplexLines[i].block2Place;
		for (int p = 0; p < allComplexLines[i].block2Length; p++)occupied[p + p0] = true;
		unsigned char c = 0;
		for (int p = 0; p < 6; p++)c |= (occupied[p] << p);
		occupiedPlaces[i] = c;
	}
}

int test1()
{
	init_allComplexLines();
	init_occupiedPlaces(); 
	Pos pos = 0;
	pos = setLine(pos, 0+6, 1);
	pos = setLine(pos, 1 + 6, 3);
	pos = setLine(pos, 2 + 6, 7);
	pos = setLine(pos, 3 + 6, 9);
	pos = setLine(pos, 4 + 6, 10);
	pos = setLine(pos, 5 + 6, 8);
	cout << checkLegal(pos) << endl;
	pos = setLine(pos, 0, 7);
	cout << checkLegal(pos) << endl;
	pos = setLine(pos, 4, 1);
	cout << checkLegal(pos) << endl;
	pos = setLine(pos, 2, 1);
	cout << checkLegal(pos) << endl;
	return 0;
}

int main_enumerate()
{
	for (int i = 0; i < 10000; i++)stepsPosesCount[i] = 0;
	//test1();
	string logFileName = "log.txt";
	long long totalCalculatedConditions = 0;
	init_allComplexLines();
	init_occupiedPlaces();
	init_allSingleMovesAvailable();
	for (int forcedZerosNum = 0; forcedZerosNum < 12; forcedZerosNum++)
		for (int forcedZeros = 0b0; forcedZeros < 4096; forcedZeros++)
	{
		if ((forcedZeros >> 2) & 1)continue;
		int forcedZerosNumCount = 0;
		for (int i = 0; i < 12; i++)
		{
			forcedZerosNumCount += ((forcedZeros >> i) & 1);
		}
		if (forcedZerosNumCount != forcedZerosNum)continue;
		//vector<Pos> posQueue1, posQueue2 ;

		ofstream logFile(logFileName, ios::app);
		vector<Pos> *curQueue = new vector<Pos>;
		vector<Pos> *nextQueue = new vector<Pos>;
		set<Pos> posSet;
		init_allFinishedPoses(forcedZeros, curQueue, &posSet);
		for (int steps = 0; steps < 10000; steps++)
		{
			cout << "steps = " << steps << " forcedZeros = " << bitset<12>(forcedZeros) << " remain conditions " << curQueue->size() << endl;
			logFile << "steps = " << steps << " forcedZeros = " << bitset<12>(forcedZeros) << " remain conditions " << curQueue->size() << endl;
			totalCalculatedConditions += curQueue->size();
			if (curQueue->size() == 0)
			{
				cout << "Max steps is " << steps - 1 << " for forcedZeros = " << bitset<12>(forcedZeros) << endl;
				cout << "Calculated " << totalCalculatedConditions << " conditions" << endl << endl;
				logFile << "Max steps is " << steps - 1 << " for forcedZeros = " << bitset<12>(forcedZeros) << endl;
				logFile << "Calculated " << totalCalculatedConditions << " conditions" << endl << endl;
				delete curQueue;
				delete nextQueue;
				break;
			}
			if (steps >= 25)//write conditions to txt file
			{
				string fileName = to_string(steps) + "steps.txt";
				ofstream ofs(fileName, ios::app);
				for (int i = 0; i < curQueue->size(); i++)
				{
					ofs << (*curQueue)[i]<<"  "<<getTypeID((*curQueue)[i]) << endl;
				}
				ofs.close();
			}
			//now poses in curQueue need "steps" steps
			stepsPosesCount[steps] += curQueue->size();
			for (int i = 0; i < curQueue->size(); i++)
			{
				vector<Pos> nextPoses;
				getAllAvailableNextPoses((*curQueue)[i], &nextPoses);
				for (int j = 0; j < nextPoses.size(); j++)
				{
					Pos pos = nextPoses[j];
					if (!posSet.count(pos))
					{
						posSet.insert(pos); 
						nextQueue->push_back(pos);
					}
				}
				
			}
			delete curQueue;
			curQueue = nextQueue;
			nextQueue = new vector<Pos>;

		}
		logFile.close();
	}
	ofstream ofs("StepsCount.txt");
	for (int i = 0; i < 1000; i++)
	{
		ofs << i << " " << stepsPosesCount[i] << endl;
	}
	//vector<Pos> initialList;
	//set<Pos> initialSet;
	//init_allFinishedPoses(0, &initialList,&initialSet);

}

int main_printCondold()
{
	init_allComplexLines();
	Pos pos;
	cin >> pos;
	for (int i = 0; i < 12; i++)
	{
		Line line = getLine(pos, i);
		complexLine cl = allComplexLines[line];
		cout << i << " " << cl.block1Place << cl.block1Length << cl.block2Place << cl.block2Length << endl;
	}
	return 0;
}
int main_print()
{
	init_allComplexLines();
	init_occupiedPlaces();
	Pos pos;
	cin >> pos;
	printPos(pos);
	for (int i = 0; i < 12; i++)
	{
		Line line = getLine(pos, i);
		complexLine cl = allComplexLines[line];
		cout << i << " " << cl.block1Place << cl.block1Length << cl.block2Place << cl.block2Length << endl;
	}
	return 0;
}
Pos getPosID()
{
	cout<<"Instruction\n"<< "   11 10  9  8  7  6" << endl << "  0\n  1\n  2\n  3\n  4\n  5\n";
	vector<string> lineStrs;
	Pos pos = 0;
	for (int i = 0; i < 12; i++)
	{
		string lineStr;
		Line l=22;
		cout << i << ":";
		cin >> lineStr;
		for (int j = 0; j < 22; j++)
		{
			if (line2str[j] == lineStr)
			{
				l = j;
				break;
			}
		}
		if (l == 22)
		{
			cout << "Wrong" << endl;
			i--;
			continue;
		}
		lineStrs.push_back(lineStr);
		pos = setLine(pos, i, l);
	}
	cout << "ID:" << pos << endl;
	cout << "Lines:\n";
	for (int i = 0; i < 12; i++)cout << lineStrs[i] << endl;
	cout << endl;
	return pos;
}
int solve(Pos targetPos)
{
	init_allComplexLines();
	init_occupiedPlaces();
	init_allSingleMovesAvailable();
	vector<Pos> *curQueue = new vector<Pos>;
	vector<Pos> *nextQueue = new vector<Pos>;
	map<Pos,Pos> posSet;
	Pos finalResult = 0;
	int finalSteps = 100000;
	curQueue->push_back(targetPos);
	posSet.insert(make_pair(targetPos, 0));
	for (int steps = 0; steps < 10000; steps++)
	{
		cout << "steps = " << steps << " remain conditions " << curQueue->size() << endl;
		if (curQueue->size() == 0)
		{
			cout << "No solution!" << endl << endl;
			delete curQueue;
			delete nextQueue;
			return 0;
			break;
		}
		//now poses in curQueue need "steps" steps
		for (int i = 0; i < curQueue->size(); i++)
		{
		//	printPos((*curQueue)[i]);
			vector<Pos> nextPoses;
			getAllAvailableNextPoses((*curQueue)[i], &nextPoses);
			for (int j = 0; j < nextPoses.size(); j++)
			{
				Pos pos = nextPoses[j];
				if (getLine(pos, 2) == 1)
				{
					finalResult = pos;
					finalSteps = steps + 1;
				}
				if (!posSet.count(pos))
				{
					posSet.insert(make_pair(pos, (*curQueue)[i]));
					nextQueue->push_back(pos);
				}
			}

		}

		if (finalResult != 0)
		{
			break;
		}
		delete curQueue;
		curQueue = nextQueue;
		nextQueue = new vector<Pos>;

	}
	delete curQueue;
	delete nextQueue;
	Pos lastPos = finalResult;
	int stepCount = finalSteps;
	while (lastPos != 0)
	{
		cout <<"Step "<< stepCount<<" :"<< endl;
		printPos(lastPos);
		stepCount--;
		lastPos = posSet[lastPos];
	}
	return 0;
}
int main_solve()
{

	Pos targetPos;// = getPosID();
	cin >> targetPos;
	solve(targetPos);
	
}
