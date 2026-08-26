#include <iostream>
#include <iomanip>
#include <vector>
#include <conio.h>

#include <stdlib.h>
#include "mysql_connection.h"
#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/prepared_statement.h>



using namespace std;

unsigned int finalDigest[8] = { 0x6A09E667,  0xBB67AE85,  0x3C6EF372,  0xA54FF53A,  0x510E527F, 0x9B05688C, 0x1F83D9AB,  0x5BE0CD19 };
unsigned int tempCompression[8] = { 0x6A09E667,  0xBB67AE85,  0x3C6EF372,  0xA54FF53A,  0x510E527F, 0x9B05688C, 0x1F83D9AB,  0x5BE0CD19 };
unsigned int constantArr[64] = { 0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
									0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3, 0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
									0xe49b69c1, 0xefbe4786, 0xfc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
									0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147, 0x6ca6351, 0x14292967,
									0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13, 0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
									0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
									0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
									0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208, 0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2 };

unsigned int sigma0Lower(unsigned int x) {
	unsigned int a, b, c;
	a = x; b = x; c = x;
	a = (a >> 7) | (a << 25);
	b = (b >> 18) | (b << 14);
	c = c >> 3;
	x = a ^ b ^ c;
	return x;
}
unsigned int sigma1Lower(unsigned int x) {
	unsigned int a, b, c;
	a = x; b = x; c = x;
	a = (a >> 17) | (a << 15);
	b = (b >> 19) | (b << 13);
	c = c >> 10;
	x = a ^ b ^ c;
	return x;
}
unsigned int sigma0Upper(unsigned int x) {
	unsigned int a, b, c;
	a = x; b = x; c = x;
	a = (a >> 2) | (a << 30);
	b = (b >> 13) | (b << 19);
	c = (c >> 22) | (c << 10);
	x = a ^ b ^ c;
	return x;
}
unsigned int sigma1Upper(unsigned int x) {
	unsigned int a, b, c;
	a = x; b = x; c = x;
	a = (a >> 6) | (a << 26);
	b = (b >> 11) | (b << 21);
	c = (c >> 25) | (c << 7);
	x = a ^ b ^ c;
	return x;
}
unsigned int schedule(unsigned int _2, unsigned int _7, unsigned int _15, unsigned int _16) {
	unsigned int output;
	_2 = sigma1Lower(_2);
	_15 = sigma0Lower(_15);
	output = _2 + _7 + _15 + _16;
	return output;
}
unsigned int choice(unsigned int x, unsigned int y, unsigned int z) {
	unsigned int output = 0;
	for (int i = 0; i < 32; i++) {
		output <<= 1;

		if (x & 0x80000000)
			output |= ((y & 0x80000000) >> 31);
		else
			output |= ((z & 0x80000000) >> 31);
		x <<= 1;	y <<= 1;	z <<= 1;
	}
	return output;
}
unsigned int majority(unsigned int x, unsigned int y, unsigned int z) {
	unsigned int output = 0;
	int bit;
	for (int i = 0; i < 32; i++) {
		output <<= 1;
		bit = ((x & 0x80000000) >> 31) + ((y & 0x80000000) >> 31) + ((z & 0x80000000) >> 31);
		if (bit == 2 || bit == 3)
			output |= 0x1;
		else
			output |= 0x0;

		x <<= 1;	y <<= 1;	z <<= 1;
	}
	return output;
}
unsigned int t1(unsigned int kn, unsigned int wn) {
	unsigned int E = sigma1Upper(finalDigest[4]);
	unsigned int ch = choice(finalDigest[4], finalDigest[5], finalDigest[6]);
	unsigned int output = (E + ch + finalDigest[7] + kn + wn);
	return output;
}
unsigned int t2() {
	unsigned int A = sigma0Upper(finalDigest[0]);
	unsigned int maj = majority(finalDigest[0], finalDigest[1], finalDigest[2]);
	unsigned int output = A + maj;
	return output;
}
void pad1(vector<bool>& bits) {
	bits.push_back(1);
}
void pad0s(vector<bool>& bits, int i) {
	for (int j = 0; j < i; j++) {
		bits.push_back(0);
	}
}
void padMessageSize(vector<bool>& bits, unsigned long long int messageSize) {
	for (int i = 0; i < 64; i++) {
		bits.push_back(messageSize & 0x8000000000000000);
		messageSize <<= 1;
	}
}

void connectToMySQL() {
	// id, seed (initial), s1, s2, s3, s4, s5, s6, s7, s8, digest (final), d1, d2, d3, d4, d5, d6, d7, d8
	// derived: s1, s2, s3, s4, s5, s6, s7, s8, d1, d2, d3, d4, d5, d6, d7, d8, 

	string server = "tcp://127.0.0.1:3306";
	string username = "root";
	string password = "";

	sql::Driver* driver;
	sql::Connection* con;
	sql::Statement* stmt;
	sql::PreparedStatement* pstmt;

	try
	{
		driver = get_driver_instance();
		con = driver->connect(server, username, password);
		cout << "Connected successfully" << endl;
	}
	catch (sql::SQLException e)
	{
		cout << "Could not connect to server. Error message: " << e.what() << endl;
		system("pause");
		exit(1);
	}

	//please create database "quickstartdb" ahead of time
	con->setSchema("quickstartdb");

	stmt = con->createStatement();
	stmt->execute("DROP TABLE IF EXISTS inventory");
	cout << "Finished dropping table (if existed)" << endl;
	stmt->execute("CREATE TABLE inventory (id serial PRIMARY KEY, name VARCHAR(50), quantity INTEGER);");
	cout << "Finished creating table" << endl;
	delete stmt;

	pstmt = con->prepareStatement("INSERT INTO inventory(name, quantity) VALUES(?,?)");
	pstmt->setString(1, "banana");
	pstmt->setInt(2, 157);
	pstmt->execute();
	cout << "One row inserted." << endl;

	pstmt->setString(1, "orange");
	pstmt->setInt(2, 154);
	pstmt->execute();
	cout << "One row inserted." << endl;

	pstmt->setString(1, "apple");
	pstmt->setInt(2, 100);
	pstmt->execute();
	cout << "One row inserted." << endl;

	delete pstmt;
	delete con;
}

void main() {
	//connectToMySQL();
	unsigned long long int messageSize;
	int currentBlock = 0;
	string message;
	char choice;

	vector<bool> bits;
	cout << "Enter chars or whole message? [c/m]: ";
	cin >> choice;
	if (choice=='c') {
		char ch;
		while (true) {
			ch = _getch();
			if (ch == '#') break;
			cout << ch;
			message += ch;
		}
	} else {
		cout << "Please enter the entire message: ";
		cin >> message;
	}
	cout << "Message size: " << message.size() << endl;

	for (char c : message) {
		if (c == '1') {
			bits.push_back(1);
		}
		else {
			bits.push_back(0);
		}
	}
	messageSize = choice == 'c' ? bits.size() : 0;
	//Setting number of blocks
	unsigned int numberOfBlocks = choice == 'c' ? bits.size() % 512 < 448 ? ((bits.size() / 512) + 1) : ((bits.size() / 512) + 2) : 1;

	vector<vector<bool>> bitsBlocks(numberOfBlocks, vector<bool>(512));

	//Setting message schedule size
	auto messageSchedule = new unsigned int[numberOfBlocks][64];
	//Separate message from the padding by adding binary 1 after the message
	if (choice == 'c') {
		pad1(bits);
		//Padding zeroes
		if (messageSize % 512 < 448) {
			pad0s(bits, (448 - (messageSize % 512 + 1)));
		}
		else {
			// pad current block first
			pad0s(bits, (512 - (messageSize % 512 + 1)));
			// pad 448 bits from next block
			pad0s(bits, 448);
		}
		// Adding the length of the message at the end of the message
		padMessageSize(bits, messageSize);
	} 
	//else {
	//	pad0s(bits, 1); // not in procedure
	//}

	cout << "Final bits[] size: " << bits.size() << endl;
	cout << "numberOfBlocks: " << numberOfBlocks << endl;

	cout << "bits[] content: " << endl;
	for (bool b : bits) {
		cout << b;
	}
	cout << endl << endl;


	//Dividing message into blocks
	// i variable for the number of blocks while j is the variable for bytes inside one block
	for (int i = 0, j = 0; i < numberOfBlocks; i++) {
		// k variable is for setting bytes in each block
		for (int k = 0; k < 512; j++, k++) {
			bitsBlocks[i][k] = bits[j];
		}
	}

	//mesage schedule and converting messsage schedules into unsigned integers;
	while (currentBlock != numberOfBlocks) {
		//Adding the 46 bytes in each block
		for (int i = 0, j = 0; i < 16; i++) {
			messageSchedule[currentBlock][i] = 0;
			for (int k = 0; k < 32; k++, j++) {
				messageSchedule[currentBlock][i] <<= 1;
				messageSchedule[currentBlock][i] |= bitsBlocks[currentBlock][j];
			}
		}
		// Filling the rest of the message schedule with 48 extra bytes
		for (int i = 16; i < 64; i++) {
			messageSchedule[currentBlock][i] = schedule(messageSchedule[currentBlock][i - 2],
				messageSchedule[currentBlock][i - 7],
				messageSchedule[currentBlock][i - 15],
				messageSchedule[currentBlock][i - 16]);
		}
		currentBlock++;
	}


	currentBlock = 0;
	//Compression
	unsigned int T1, T2;
	while (currentBlock != numberOfBlocks) {
		for (int i = 0; i < 64; i++) {
			T1 = t1(constantArr[i], messageSchedule[currentBlock][i]);
			T2 = t2();
			for (int j = 7; j > 0; j--) {
				finalDigest[j] = finalDigest[j - 1];
			}
			finalDigest[0] = T1 + T2;
			finalDigest[4] += T1;
		}
		for (int i = 0; i < 8; i++) {
			finalDigest[i] += tempCompression[i];
			tempCompression[i] = finalDigest[i];
		}
		currentBlock++;
	}

	cout << hex << endl << endl;
	for (int i = 0; i < 8; i++)
		cout << setw(8) << setfill('0') << finalDigest[i];
	cout << endl << endl;

	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 32; j++) {
			bool k = 0x80000000 & finalDigest[i];
			cout << k;
			finalDigest[i] <<= 1;
		}
	}
}
