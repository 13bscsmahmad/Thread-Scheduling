#include <iostream>
#include <fstream>
#include <vector>
#include <queue>
#include <deque>
#include <string>
#include <sstream>

using namespace std;

enum states {READY, RUN, BLOCKED};
int threadID = 0;

int run(int cycle, int threadId);
int IO(thread *qBlocked, int n);

struct thread{
	int processID;
	int threadID;
	int state;
};

struct control_block{
	int processID;
	vector<thread*> activeThreads;
	//thread* activeThreads;
	int size;
	int state;
};

struct event_{
	int cycle;
	int type;
	int processID;
	int size;
};

int main(void){

	vector<control_block> cb; //control block
	vector<thread> threadTable;
	std::deque<int> readyQ;
	std::queue<int>	blockedQ;
	int blockedQsize;

	ifstream myfile;//file pointer
	myfile.open("control.txt");
	string line;//line of a file

	if (!myfile.is_open()){

		cout << "ERROR Opening file\n";


	}

	int cycle = 0;

	while (cycle < 100000){

		int repeat = 0;

		getline(myfile, line);
		
		string str;
		std::stringstream   linestream(line);
		getline(linestream, str, ' ');

		if (stoi(str) == cycle){
			
			control_block* newCB = new control_block();
			
			string processID;
			//int type;
			getline(linestream, processID, ' ');
			getline(linestream, processID, ' ');
			newCB->processID = stoi(processID);

			thread *mainThread = new thread();
			mainThread->processID = stoi(processID);
			mainThread->state = READY;
			mainThread->threadID = threadID + 1;
			newCB->activeThreads.push_back(mainThread);
			threadTable.push_back(*mainThread);

			newCB->state = READY;

			string requiredMemory;
			getline(linestream, requiredMemory, ' ');
			newCB->size = stoi(requiredMemory);

			readyQ.push_back(mainThread->threadID);
			
		}

		if (repeat < 2){
			
		}

		int result = run(cycle, threadID);

		if (result != -1) {
			// An event was requested by the thread
			// This may be an I/O event, termination event, or a request
			// to create another thread

			if (result == 998){ // post IO
				/*thread *currentThread = new thread;
				for (int i = 0; i < threadTable.size; i++){
					if (threadTable[i].threadID == threadID){
						currentThread = &threadTable[i];
					}
				}

				if (!currentThread){
					cout << "998: Current thread not found" << endl;
				}*/

				IO(&blockedQ, blockedQ.size);
			}

			if (result == 995){

			}

			if (result == 996){

			}

		}

		cycle++;
	}


}

int run(int cycle, int threadId) {
	int r = rand() % 100;
	if (r < 40) return -1; // do nothing
	if (r < 60) return 998; // post I/O
	if (r < 80) return 995; // create another thread
	else return 996; // terminate thread
}

int IO(int *qBlocked, int n) {
	int r = rand() % n;
	if (rand() % 100 < 50)
		return -1;
	else
		return qBlocked[r]->threadId;
}