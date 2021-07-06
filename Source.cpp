#include<bitset>
#include<algorithm>
#include<iostream>
#include<Bits.h>
#include<iomanip>
#include<cmath>
using namespace std;

/*
Operations:
shift right
rotate right
xor
normal addition

Functions:
o0(x) -> sigma (lower case) zero
	for a word X do the following:
		rotate right 7
		rotete right 18
		shift right 3
		Then take the XOR of these three words

o1(x) -> sigma (lower case) one
	for a word X do the following:
		rotate right 17
		rotate right 19
		shift right 10
		Then take the XOR of these three words

E0(x) -> sigma (upper case) zero
	for a word X do the following:
		rotate right 2
		rotate right 13
		rotate right 22
		then take the XOR of these three words

E1(x) -> sigma (upper case) one
	for a word X do the following:
		rotate right 6
		rotate right 11
		rotate right 25
		then take the XOR of these three words

Choice function:
choice(x,y,z):
	if x bit is 1:
		take y bit
	else
		take z bit

Majority function:
	majority(x,y,z): takes the bit that repeated most

*/

//string convertToBinary(string number, int precision) {
//	string binary = "";
//	int integerPart = number;
//	double fractionPart = number - integerPart;
//	while (integerPart != 0) {
//		binary.push_back((integerPart % 2) + '0');
//		integerPart /= 2;
//	}
//	reverse(binary.begin(), binary.end());
//	binary.push_back('.');
//	int countPrecision = 0,tempInteger;
//	while (countPrecision != precision) {
//		tempInteger = fractionPart * 2;
//		binary.push_back(tempInteger + '0');
//		fractionPart = (fractionPart * 2) - int(fractionPart * 2);
//		countPrecision++;
//	}
//	return binary;
//}

int* convertStringToLongInteger(string str) {
	int length = str.size(), count = 0;
	int* arr = new int[length];
	while (count != length) {
		if (str[count] != '.')
			arr[count] = str[count] + '0';
		else
			arr[count] = -1;
		count++;
	}
	return arr;
}

void constants(string* arr) {
	int prime = 2, countPrime = 0;
	while (countPrime != 64) {
		for (int i = 2; i <= (prime / 2)+1; i++) {
			if (prime % i == 0) {
				if (countPrime == 0)
					break;
				prime++;
				i = 2;
				continue;
			}
		}
		
		double cube = pow(prime, 1 / 3.0);
		cube = cube - (int)cube;
		int count32Digits = 0;
		while (count32Digits != 32) {
			cube *= 2;
			arr[countPrime].push_back((int)cube + '0');
			cube = cube - (int)cube;
			count32Digits++;
		}
		prime++;
		countPrime++;
	}
}


const unsigned int a = 0x6A09E667, b = 0xBB67AE85, c = 0x3C6EF372, d = 0xA54FF53A, e = 0x510E527F, f = 0x9B05688C, g = 0x1F83D9AB, h = 0x5BE0CD19;


string sigma0Lower(string x) {
	string a, b, c;
	a = x; b = x; c = x;
	rotate(a.begin(), a.begin() + a.size() - 7, a.end()), rotate(b.begin(), b.begin() + b.size() - 18, b.end());
	for (int i = 0; i < 3; i++)
		for (int j = 31; j > 3; j--)
			c[j] = c[j - 1];
	for (int i = 2; i >= 0; i--)
		c[i] = '0';
	for (int i = 0; i < 32; i++)
		x[i] = a[i] ^ b[i] ^ c[i];
	return x;
}
string sigma1Lower(string x) {
	string a, b, c;
	a = x; b = x; c = x;
	rotate(a.begin(), a.begin() + a.size() - 17, a.end()), rotate(b.begin(), b.begin() + b.size() - 19, b.end());
	for (int i = 0; i < 10; i++)
		for (int j = 31; j > 3; j--)
			c[j] = c[j - 1];
	for (int i = 9; i >= 0; i--)
		c[i] = '0';
	for (int i = 0; i < 32; i++)
		x[i] = a[i] ^ b[i] ^ c[i];
	return x;
}
string sigma0Upper(string x) {
	string a, b, c;
	a = x; b = x; c = x;
	rotate(a.begin(), a.begin() + a.size() - 2, a.end()), rotate(b.begin(), b.begin() + b.size() - 13, b.end());
	rotate(c.begin(), c.begin() + c.size() - 22, c.end());
	for (int i = 0; i < 32; i++)
		x[i] = a[i] ^ b[i] ^ c[i];
	return x;
}
string sigma1Upper(string x) {
	string a, b, c;
	a = x; b = x; c = x;
	rotate(a.begin(), a.begin() + a.size() - 6, a.end()), rotate(b.begin(), b.begin() + b.size() - 11, b.end());
	rotate(c.begin(), c.begin() + c.size() - 25, c.end());
	for (int i = 0; i < 32; i++)
		x[i] = a[i] ^ b[i] ^ c[i];
	return x;
}
string schedule(string _2, string _7, string _15, string _16) {
	string output;
	//_2 = sigma1Lower(_2);
	//_15 = sigma0Lower(_15);
	for (int i = 0; i < 32; i++)
		output.push_back(_2[i] + _7[i] + _15[i] + _16[i] % 2 !=0 ? '1' : '0');
	return output;
}
string choice(string x, string y, string z) {
	string output = "";
	for (int i = 0; i < 32; i++) {
		if (x[i] == '1')
			output.push_back(y[i]);
		else
			output.push_back(z[i]);
	}
	return output;
}
string majority(string x, string y, string z) {
	return "";
}


string t1(string kn, string wn) {
	string E = sigma1Upper(e);
	string ch = choice(e, f, g);
	return "";
}


void main() {
	string test = schedule("01111101101010000110010000000101", "10000000001100011001100001111111", "00110010011001100011110001011011", "01111111101101001100011011100010");
	cout << test << endl;
	exit(9);
	unsigned long long int messageSize, tempMessageSize, currentBlock = 0;
	string constantArr[64], message;
	string* blocks;
	//Initialize the 64 constants
	constants(constantArr);
	cin >> message;
	messageSize = message.length();
	tempMessageSize = messageSize;
	//Setting number of blocks
	unsigned int numberOfBlocks = message.length() % 64 < 56 ? message.length() / 64 + 1 : message.length() / 64 + 2;
	blocks = new string[numberOfBlocks];
	//Initializing blocks
	for (int i = 0; i < numberOfBlocks; i++)
		blocks[i] = "";
	auto messageSchedule = new string[numberOfBlocks][64];
	//Separate message from the padding by adding 0x1 after the message
	message.push_back(0x80);
	//Padding zeroes
	if (messageSize %64 <56) {
		for (int i = messageSize % 64 + 1 /*for the 1 added*/; i < 56; i++)
			message.push_back(0x00);
	}
	else {
		for (int i = messageSize % 64 + 1; i < 64; i++)
			message.push_back(0x00);
		for (int i = 1; i < 56; i++)
			message.push_back(0x00);
	}
	// Adding the length of the message at the end of the message
	for (int i = 0; i < 8; i++) {
		unsigned char temp = (tempMessageSize & 0xff00000000000000) >> 56;
		message.push_back(temp);
		tempMessageSize = tempMessageSize << 8;
	}
	////Padding
	//for (int i = 0, j = 0; i < numberOfBlocks; i++, j += 64) {
	//	if (messageSize / 64 == 0 && messageSize < 56) {
	//		//Append message
	//		blocks[i].append(message);
	//		
	//		blocks[i].push_back(0x80);
	//		//Fill the rest of the block with zeroes until it reaches 56 bytes
	//		for (int j = messageSize + 1; j < 56; j++)
	//			blocks[i].push_back(0x00);
	//		tempMessageSize = messageSize;
	//		//Append the size of the original message to the end of the block (last 8 bytes)
	//		for (int i = 0; i < 8; i++) {
	//			unsigned char temp = (tempMessageSize & 0xff00000000000000) >> 56;
	//			cout << "First byte: " << hex << int(temp) << endl;
	//			blocks[0].push_back(temp);
	//			tempMessageSize = tempMessageSize << 8;
	//		}
	//	}
	//	else if (messageSize / 64 == 0 && messageSize > 56) {
	//		//Append message
	//		blocks[i].append(message);
	//		//Separate message from the padding by adding 0x1 after the message
	//		blocks[i].push_back(0x80);
	//		//Fill the rest of the block with zeroes until it reaches 56 bytes
	//		for (int j = messageSize + 1; j < 56; j++)
	//			blocks[i].push_back(0x00);
	//		tempMessageSize = messageSize;
	//		//Append the size of the original message to the end of the block (last 8 bytes)
	//		for (int i = 0; i < 8; i++) {
	//			unsigned char temp = (tempMessageSize & 0xff00000000000000) >> 56;
	//			cout << "First byte: " << hex << int(temp) << endl;
	//			blocks[0].push_back(temp);
	//			tempMessageSize = tempMessageSize << 8;
	//		}
	//	}
	//}

	//Dividing message into blocks
	// i variable for the number of blocks while j is the variable for bytes inside one block
	for (int i = 0,j=0; i < numberOfBlocks; i++) {
		// k variable is for setting bytes in each block
		for (int k = 0; k < 64; j++, k++)
			blocks[i].push_back(message[j]);
	}

	//mesage schedule
	while (currentBlock != numberOfBlocks) {
		for (int i = 0, j = 0; i < 16; i++)
			for (int k = 0; k < 32; k++, j++)
				messageSchedule[currentBlock][i].push_back(blocks[currentBlock][j]);

		for (int i = 16; i < 64; i++)
			messageSchedule[currentBlock][i] = schedule(messageSchedule[currentBlock][i - 2],
				messageSchedule[currentBlock][i - 7],
				messageSchedule[currentBlock][i - 15],
				messageSchedule[currentBlock][i - 16]);
		currentBlock++;
	}
	currentBlock = 0;
	//Compression
	while (currentBlock != numberOfBlocks) {

	}



	cout << hex;
	int tempsize = blocks[0].length() * 8;
	int* arr = new int[tempsize];
	// j variable loops through the 512 bits, while i variable loops through the 64 bytes, and k variable loops through the 8 bits of a single byte
	for (int i = 63, j = tempsize - 1,k; i >= 0; i--) {
		unsigned char temp = blocks[0][i];
		for (k = 0; k < 8; k++, j--) {
			arr[j] = temp & 0x1;
			temp = temp >> 1;
		}
	}
	for (int i = 0;i<tempsize;i++)
		cout << arr[i];
	exit(8);


}