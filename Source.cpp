#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <fstream>
#include <vector>
#include <queue>
#include <deque>
#include <string>
#include <string.h>

using namespace std;

enum states { NEW, READY, RUNNING, BLOCKED, KILLED };

int totalThreads = 0;
int currentThreadID;

struct thread{
	int processID;
	int threadID;
	int state;

	thread(){

		processID = 0;
		threadID = 0;
		state = NEW;
	}

};

struct control_block{

	int processID;
	vector<thread*> activeThreads;
	int size;
	int state;
	int cycle_started;
	int cycle_terminated;
	bool has_terminated;
	int running_clock_cycles;

	control_block(){
		size = 0;
		state = NEW;
		cycle_started = -1;
		cycle_terminated = -2;
		running_clock_cycles = 0;
	}

};

struct event_{
	int cycle;
	int type;
	int processID;
	int size;
};

struct instruction{

	int cycle;
	int opCode;
	int processID;
	int memorySize;
};

int run(int cycle, int threadId);
int IO(std::vector<thread> *blockedList, int n);
int loadFromFile(void);

vector<instruction> instructions;


int main(void){

	srand(10);
	bool resetRepeat = false;

	vector<control_block> cb; //Store all control blocks
	vector<thread> threadTable; // Store all threads and related info
	vector<thread> blockedThreads; // Store list of all blocked threads
	std::deque<int> readyQ; // Store list of all threads that are ready to RUN


	if (loadFromFile() == 0){ // loads all things into instructions vector

		int cycle = 0;
		int repeat = 0;

		while (cycle < 100000){

			//for (int i = 0; i < readyQ.size(); i++){
			//	cout << readyQ[i] << " ";
			//}
			//
			//cout << endl;
			//cout << "Cycle: " << cycle + 1 << " Repeat: " << repeat << endl;

			for (int i = 0; i < instructions.size(); i++){
				if (instructions[i].cycle == cycle){

					control_block newCB;
					newCB.processID = instructions[i].processID;
					thread mainThread;
					mainThread.processID = instructions[i].processID;

					mainThread.state = READY;
					totalThreads++;
					mainThread.threadID = totalThreads;

					newCB.activeThreads.push_back(&mainThread);
					threadTable.push_back(mainThread);

					newCB.state = READY;
					newCB.size = instructions[i].memorySize;
					readyQ.push_back(mainThread.threadID);

					newCB.cycle_started = cycle;
					newCB.state = RUNNING;
					cb.push_back(newCB);

//					cout << "Control block created for thread ID" << mainThread.threadID << endl;

					break;
				}
			}


			// Get a thread from the ready queue to run. When extracting thread from ready queue, remove thread from queue, update thread
			// state.

//			cout << "readyQ size: " << readyQ.size() << " Blocked threads: " << blockedThreads.size() << endl;

			if (repeat == 0 && readyQ.size() > 0){ // Only get thread ID from readyQ if repeat = 0 (3 clk cycles finished for
				// previous thread) and there is a thread in the readyQ

				currentThreadID = readyQ.front(); // get ID of thread to RUN
//				cout << "readyQ.front()" << readyQ.front() << endl;
				readyQ.pop_front(); // Update ready queue to remove thread from ready queue



			}

			// threadID

			thread *currentThread = new thread; // thread to RUN

			for (int i = 0; i < threadTable.size(); i++){
				if (threadTable[i].threadID == currentThreadID){
					currentThread = &threadTable[i]; // this gets all values from threadTable about the relevant thread
				}
			}

			if (!currentThread){
//				cout << "main: Current thread not found" << endl;
				return 1;
			}

			for (int u = 0; u < cb.size(); u++){ // increment clock cycles for RUNNING state of the process
				if (cb[u].processID == currentThread->processID){
					cb[u].running_clock_cycles++;
					cb[u].state = RUNNING;

				}
			}

			currentThread->state = RUNNING; // update state of thread

//			cout << "Current thread: " << currentThread->threadID << endl;
//			cout << "readyQ size: " << readyQ.size() << " Blocked threads: " << blockedThreads.size() << endl;

			if ((readyQ.size() == 0 && blockedThreads.size() > 0)){


//				cout << "readyQ.size is 0 and blockedThreads is NOT 0" << endl;

			}
			else {

				if (!(currentThread->threadID == 0)){
					int result = run(cycle, currentThread->threadID);


//					cout << "This thread's opCode: " << result << endl;

					if (result != -1) {
						// An event was requested by the thread
						// This may be an I/O event, termination event, or a request
						// to create another thread
						////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
						if (result == 998){ // POST I/O, update state of thread to BLOCKED, and add thread to blocked vector, reset repeat
							// so that in the next cycle, we can pop thread from ready queue




							currentThread->state = BLOCKED;
							blockedThreads.push_back(*currentThread);
//							cout << currentThread->threadID << " post I/0. State: " << currentThread->state << endl;
							resetRepeat = true;

						}

						////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

						if (result == 995){ // SPAWN THREAD, update number of active threads for the process in the cb,
							// add to ready queue, update thread table, update totalThreads

							thread newThread;


							// Create new thread

							newThread.processID = currentThread->processID;
							newThread.state = READY;
							totalThreads++; // update totalThreads
							newThread.threadID = totalThreads;
							readyQ.push_front(newThread.threadID); // pushing newThread ID to the front of the READY queue

							for (int i = 0; i < cb.size(); i++){ // update number of active threads for the process in the cb
								if (cb[i].processID == currentThread->processID){
									cb[i].activeThreads.push_back(&newThread);
									break;
								}
							}

							threadTable.push_back(newThread); // update threadTable
//							cout << "New thread's ID: " << newThread.threadID << endl;

						}

						////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

						if (result == 996){ // TERMINATE THREAD. Update threadTable and list of active threads for the process. Reset repeat.
							// Note if all threads of the process have been terminated.

							// Check if process has any active threads. If not, process has been terminated.
							
//							cout << currentThread->threadID << " is being terminated!" << endl;
							int tempThreadId = (currentThread)->threadID;
							//cout << "CHECKING HERE! THERE MUST BE A THREAD ID. THREAD ID IS:" << tempThreadId << endl;
							int tempProcessId = -1;
							for (int i = 0; i < threadTable.size(); i++){
								if (threadTable[i].threadID == tempThreadId){
									tempProcessId = threadTable[i].processID;
									break;
								}
							}



							for (int i = 0; i < cb.size(); i++){
								if (cb[i].processID == tempProcessId){
									if (cb[i].activeThreads.size() == 1){ // See if this is the last thread
										cb[i].has_terminated = true; // Process will be terminated once this thread is terminated
										cb[i].cycle_terminated = cycle;
										cb[i].state = KILLED;
										break;
									}
								}
							}


							// Update list of active threads from cb first, since it has a pointer to the thread

							int vectorPositionOfProcess = 2;

							for (int i = 0; i < cb.size(); i++){
								if (cb[i].processID == currentThread->processID){

									vectorPositionOfProcess = i;
									//int threadIDtobeKilled = -1;
									for (int q = 0; q < cb[i].activeThreads.size(); q++){
										if (currentThread->threadID == cb[i].activeThreads[q]->threadID){
											cb[i].activeThreads.erase(cb[i].activeThreads.begin() + q); // erased thread from list of active
											//cout << cb[i].activeThreads.size() << endl;				// threads from the cb.

											break;
										}
									}
									break;
								}



							}


							// Update threadTable						

							for (int i = 0; i < threadTable.size(); i++){
								if (threadTable[i].threadID == currentThread->threadID){
									threadTable.erase(threadTable.begin() + i);
									break;
								}

							}

							

				

							// Reset repeat, so that at the next cycle, the thread at the front of ready queue will be run
							resetRepeat = true;



						}

					}


					// Once execution of thread finished for 3 times, put it back in ready queue, otherwise repeat.

					if (repeat == 2){ // After 3rd clock cycle, update thread state, and push thread to the end of ready queue if not blocked
						// and reset repeat
						currentThread->state = READY;
						readyQ.push_back(currentThread->threadID);

						resetRepeat = true;


					}

				}
				else {
					if (currentThread->threadID == 0 && readyQ.size() > 0){
						resetRepeat = true;
					}
				}
			}
			// check for I/O returns
			int result1 = -1;
			if (blockedThreads.size() > 0){
				result1 = IO(&blockedThreads, blockedThreads.size());
			}
			if (result1 != -1){

//				cout << "I/O returned for thread " << result1 << endl;
				// result has the threadID which was in blocked list and the IO has been handled now

				// Remove from blocked queue, update thread status for that thread, push it to the front of the ready queue

				for (int i = 0; i < blockedThreads.size(); i++){
					if (blockedThreads[i].threadID == result1){

						// update thread status to READY
						blockedThreads[i].state = READY;

						// remove thread from blocked queue
						thread temp = blockedThreads[i];
						blockedThreads.erase(blockedThreads.begin() + i);

						//push to the front of the ready queue

						readyQ.push_front(temp.threadID);


						break;


					}
				}

			}

			if (resetRepeat){
				repeat = 0;
				currentThread = NULL;
			}
			else{
				repeat++;
			}

			cycle++;

		}


	}
	else { cout << "Some error" << endl; }

	
	for (int i = 0; i < cb.size(); i++){
		//cout << "Process ID: " << cb[i].processID << " Started at cycle " << cb[i].cycle_started << endl;
		//cout << "Cycles for all threads from start to finish: " << cb[i].cycle_terminated - cb[i].cycle_started << endl;

		//cout << "Active threads:  " << cb[i].activeThreads.size() << endl;
		//cout << "Has terminated: " << cb[i].has_terminated << endl;
		//cout << "Total cycles = " << cb[i].cycle_terminated - cb[i].cycle_started;
		//cout << "RUNNING cycles = " << cb[i].running_clock_cycles << endl;
	}
	
	cout << "Control block size: " << cb.size();
	int completed_processes = 0;
	for (int y = 0; y < cb.size(); y++){
		if (cb[y].has_terminated == 1){
			completed_processes++;
		}
	}

	cout << ". Completed processes = " << completed_processes << "." << endl << endl;

	for (int i = 0; i < cb.size(); i++){
		if (cb[i].has_terminated == 1){
			//cout << "Cycles for all threads from start to finish: " << cb[i].cycle_terminated - cb[i].cycle_started << endl;
			cout << "Process " << cb[i].processID << endl;
			cout << "First thread started at cycle # " << cb[i].cycle_started << " . Last thread terminated at cycle # " << cb[i].cycle_terminated << ". " << endl;
			cout << "Process ran for " << (cb[i].cycle_terminated - cb[i].cycle_started) + 1 << " cycles." << endl;

		}
	}

	cout << "----------------------------------------------------------------------------------------------" << endl << endl;

	for (int i = 0; i < cb.size(); i++){
		if (cb[i].has_terminated == 1){
			
			cout << "Process " << cb[i].processID << " ran for " << cb[i].running_clock_cycles << " cycles." << endl;
		

		}
	}


	cout << endl << endl << endl << "Thread Table" << endl << endl;

	for (int i = 0; i < threadTable.size(); i++){
		cout << "Process ID: " << threadTable[i].processID << " Thread ID: " << threadTable[i].threadID << " Thread state: " << threadTable[i].state << endl;
	}


}

int run(int cycle, int threadId) {
	int r = rand() % 100;
	if (r < 40) return -1; // do nothing
	if (r < 60) return 998; // post I/O
	if (r < 80) return 995; // create another thread
	else return 996; // terminate thread
}

int IO(std::vector<thread> *blockedList, int n) {
	int r = rand() % n;
	if (rand() % 100 < 50){
		cout << "Nothing returned." << endl;
		return -1;
	}
	else
		return blockedList->at(r).threadID;



}


///////////////////////////////// Function for file i/o ////////////////////////////////////////////////////

int loadFromFile(void){ // function returns 0 if successful, else returns 1

	string line;
	ifstream myfile;
	myfile.open("control.txt");

	if (!myfile.is_open()){
		cout << "Error opening file" << endl;
		return 1;
	}
//	else cout << "File opened" << endl;

	instruction instr;

	string array1[4];

	while (!myfile.eof()){


		getline(myfile, line);
		char* s1 = (char*)line.c_str();

		char* bla = strtok(s1, " ");

		int count = 0;

		while (bla != NULL)
		{

			array1[count] = bla;

			bla = strtok(NULL, " ");
			if (count < 3){
				count++;
			}
			else {
				break;
			}
		}


		instr.cycle = stoi(array1[0]);
		instr.opCode = stoi(array1[1]);
		instr.processID = stoi(array1[2]);
		instr.memorySize = stoi(array1[3]);

		instructions.push_back(instr);

	}

	//cout << "##################################### File Contents #########################################" << endl;

	//for (int i = 0; i < instructions.size(); i++){
	//	cout << instructions[i].cycle << " " << instructions[i].opCode << " " << instructions[i].processID << " " << instructions[i].memorySize << endl;
	//}

	//cout << "####################################################################################################" << endl;

	return 0;

}

