#include<iostream>
#include<fstream>
#include<vector>
#include<string>
#include<charconv>
#include<pthread.h>
#include<mutex>



using namespace std;

//Global Variables
int numOfPrimes = 0;
int numOfPalindroms = 0;
int numOfPalindromicPrimes = 0;
int TotalNums = 0;
vector<int> PrimeList;
vector<int> PalindromeList;
vector<int> PalindromicPrimesList;
int rangeStart;
int rangeEnd;
int rangeSec;

mutex aLock;


//Functions
bool isPrime(int num) {
	if (num <= 1) return false; 
	for (int i = 2; i * i <= num; i++) {
		if (num % i == 0) return false; 
	}
	return true; 
}

bool isPalindrome(int num) {
	int original = num, reversed = 0;
	while (num > 0) {
		reversed = reversed * 10 + num % 10; 
		num /= 10; 
	}
	return original == reversed; 
}

//Thread Function
void* Worker(void* arg) {
	int TID= (int64_t)arg; 
	int rangeStartT=rangeStart+(TID*rangeSec);
	int rangeEndT=(rangeStart + ((TID+1) * rangeSec));
	printf("ThreadID=%d, startNum=%d, endNum=%d\n", TID, rangeStartT, rangeEndT);
	for (int i = rangeStartT; i < rangeEndT; i++) {
		bool prime = false;
		bool palindrome = false;

		if (isPrime(i)) {
			prime = true;
			aLock.lock();//Lock
			PrimeList.push_back(i);
			numOfPrimes++;
			aLock.unlock();//Unlock
		}

		if (isPalindrome(i)) {
			palindrome = true;
			aLock.lock();//Lock
			PalindromeList.push_back(i);
			numOfPalindroms++;
			aLock.unlock();//Unlock
		}

		if (prime && palindrome) {
			aLock.lock();//Lock
			PalindromicPrimesList.push_back(i);
			numOfPalindromicPrimes++;
			aLock.unlock();//Unlock
		}
		aLock.lock();//Lock
		TotalNums++;
		aLock.unlock();//Unlock
	}
		return nullptr; //Terminates the thread
}

//Files Functions
void readDataFromFile(string fileName) {
	ifstream file(fileName);
	string line;
	if (file.is_open()) {
		while (getline(file, line)) {
			string num = "";
			for (int i = 0; i < line.size(); i++) {
				if (line[i] != ' ')
					num += line[i];
				if (line[i] == ' ') {
					from_chars(num.data(), num.data() + num.size(), rangeStart);
					num = "";
				}
				if (i == line.size() - 1) {
					from_chars(num.data(), num.data() + num.size(), rangeEnd);
					break;
				}
			}
			break;
		}
	}
	else {
		cout << "The input file failed to open." << endl;
	}
}

void writeDataOnAFile(string fileName) {
	ofstream file(fileName);
	if (file.is_open()) {
		file << "The prime numbers are: " << endl;
		for (int i = 0; i < PrimeList.size(); i++) {
			file << PrimeList[i] << endl;
		}
		file << "The palindrome numbers are: " << endl;
		for (int i = 0; i < PalindromeList.size(); i++) {
			file << PalindromeList[i] << endl;
		}
		file << "The palindromicPrime numbers are: " << endl;
		for (int i = 0; i < PalindromicPrimesList.size(); i++) {
			file << PalindromicPrimesList[i] << endl;
		}
	}
	else {
		cout << "The output file failed to open.";
	}
}

//Main
int main(int argv,char* argc[]) {
	readDataFromFile("in.txt");
	int T = stoi(argc[1]);
	PrimeList.reserve(rangeEnd-rangeStart);
    PalindromeList.reserve(rangeEnd-rangeStart);
    PalindromicPrimesList.reserve(rangeEnd-rangeStart);
	vector<pthread_t> Threads(T);
	rangeSec = (rangeEnd - rangeStart) / T;
	if (rangeSec > 0) {
		for (int i = 0; i < T; i++) {
			pthread_create(&Threads[i], NULL, Worker, (void*)(int64_t)i);
		}
		for (int i = 0; i < T; i++) {
			pthread_join(Threads[i], nullptr);
		}
	}
	else {
		rangeSec = 1;
		for (int i = 0; i < (rangeEnd-rangeStart); i++) {
			pthread_create(&Threads[i], NULL, Worker, (void*)i);
		}
		for (int i = (rangeEnd-rangeStart); i < T; i++) {
			printf("ThreadID=%d Does Nothing\n", i);
		}
		for (int i = 0; i < (rangeEnd-rangeStart); i++) {
			pthread_join(Threads[i], nullptr);
		}
	}
	cout << "TotalNums=" << TotalNums << ", numOfPrime=" << numOfPrimes << ",numOfpalindrome = "<<numOfPalindroms<<", numOfPalindromicPrime = "<<numOfPalindromicPrimes;
	writeDataOnAFile("out.txt");
	return 0;
}
