#include<iostream>
using namespace std;

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

void constants(unsigned int* arr) {
	int prime = 2, countPrime = 0, count32Digits = 0;
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
		count32Digits = 0;
		arr[countPrime] = 0;
		while (count32Digits != 32) {
			arr[countPrime] <<= 1;
			cube *= 2;
			arr[countPrime] |= (int)cube;
			cube = cube - (int)cube;
			count32Digits++;
		}
		prime++;
		countPrime++;
	}
}


unsigned int finalDigest[8] = {	    0x6A09E667,  0xBB67AE85,  0x3C6EF372,  0xA54FF53A,  0x510E527F, 0x9B05688C, 0x1F83D9AB,  0x5BE0CD19 };
unsigned int tempCompression[8] = { 0x6A09E667,  0xBB67AE85,  0x3C6EF372,  0xA54FF53A,  0x510E527F, 0x9B05688C, 0x1F83D9AB,  0x5BE0CD19 };

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

void main() {
	unsigned long long int messageSize, tempMessageSize;
	unsigned int constantArr[64];
	int currentBlock = 0;
	string message;
	string* blocks;
	//Initialize the 64 constants
	constants(constantArr);
	cin >> message;
	messageSize = message.length();
	tempMessageSize = messageSize*8;
	//Setting number of blocks
	unsigned int numberOfBlocks = message.length() % 64 < 56 ? ((message.length() / 64) + 1) : ((message.length() / 64) + 2);
	blocks = new string[numberOfBlocks];
	//Setting message schedule size
	auto messageSchedule = new unsigned int[numberOfBlocks][64];
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
		unsigned char temp = ((tempMessageSize & (0xff00000000000000)) >> 56);
		message.push_back(temp);
		tempMessageSize <<= 8;
	}

	//Dividing message into blocks
	// i variable for the number of blocks while j is the variable for bytes inside one block
	for (int i = 0,j=0; i < numberOfBlocks; i++) {
		// k variable is for setting bytes in each block
		for (int k = 0; k < 64; j++, k++)
			blocks[i].push_back(message[j]);
	}

	//mesage schedule and converting messsage schedules into unsigned integers;
	while (currentBlock != numberOfBlocks) {
		//Adding the 46 bytes in each block
		for (int i = 0, j = 0; i < 16; i++) {
			messageSchedule[currentBlock][i] = 0;
			for (int k = 0; k < 4; k++, j++) {
				// Packing 4 characters in one unsigned integer
				messageSchedule[currentBlock][i] <<= 8;
				messageSchedule[currentBlock][i] |= (unsigned int((unsigned char)blocks[currentBlock][j]));
			}
		}
		// Filling the rest of the message schedule with 48 extra bytes
		for (int i = 16; i < 64; i++)
			messageSchedule[currentBlock][i] = schedule(messageSchedule[currentBlock][i - 2],
				messageSchedule[currentBlock][i - 7],
				messageSchedule[currentBlock][i - 15],
				messageSchedule[currentBlock][i - 16]);
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

	cout << hex<< endl << endl;
	for (int i = 0; i < 8; i++)
		cout << finalDigest[i];
	cout << endl;
}