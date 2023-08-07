#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MaxInputLength 1000
#define MaxRotors 3
#define AlphabetSize 26
#define MaxMsgLength 1000

char RotorConstants[][AlphabetSize+1] = 
{
	"ABCDEFGHIJKLMNOPQRSTUVWXYZ",
	"EKMFLGDQVZNTOWYHXUSPAIBRCJ",
	"AJDKSIRUXBLHWTMCQGZNPYFVOE",
	"BDFHJLCPRTXVZNYEIWGAKMUSQO",
	"ESOVPZJAYQUIRHXLNFTGKDCMWB",
	"RFHLVUGENSOMKWXZDBCYTIJPAQ",
	"JPGVOUMFYQBENHZRDKASXLICTW",
	"NZJHGRCXMYSWBOUFAIVLPEKQDT",
	"FKQHTLXOCBJSPDZRAMEWNIUYGV",
	"NMOYHJLRVZKEDGCQIPFSWUXABT"
};
char WelcomeMsg[] = "Willkommen auf der Enigma-Maschine!\n\n";
char MenuOptions[] = 
	"\nOPTIONS\n"
	"1 : encrypt a message\n"
	"2 : decrypt a message\n"
	"0 : exit the Enigma Machine\n\n"
	"ENTER YOUR OPTION: ";
char ExitMessage[] = "Auf Wiedersehen!\n";
char EncryptionMessage[] =
	"\nENCRYPTION WITH ENIGMA!\n"
	"=======================\n";
char RotorPrompt[] =
	"\nROTOR CONFIGURATION\n"
	"-----------------------------------\n"
	"This must be a list of numbers in the range from 0 to 8, separated by spaces.\n"
	"Maximum number of rotors you can use is 3. Note that rotor 0 is the\n"
	"identity rotor.\n\n"
	"ENTER YOUR ROTOR CONFIGURATION: ";
char DecryptionMessage[] = "\nDECRYPTION WITH ENIGMA\n"
	"=======================\n";
char WrongOption[] = "Option is not valid. Try again.\n\n";
char NoRotorsMsg[] = "You must specify at least one rotor. Try again.\n\n";
char MoreThanAllowed[] = "You cannot use more than 3 rotors. Try again.\n\n";
char InvalidRotorMsg[] = "Invalid rotor configuration. Try again.\n\n";
char DupRotorMsg[] = "You cannot use the same rotor twice. Try again.\n\n";
char RotorBeforeRotation[] = 
	"\n\nYOUR ROTOR CONFIGURATION BEFORE ROTATION\n";
char RotorConfigurationHeader[] =
	"===================================================\n"
	"A B C D E F G H I J K L M N O P Q R S T U V W X Y Z\n"
	"===================================================\n";
char RotorConfigurationFooter[] = 
	"---------------------------------------------------\n";
char NumRotationsMsg[] = 
	"How many rotations do you want to perform on the rotors?: ";
char InvalidNumRotations[] = "Invalid number of rotations. Try again.\n";
char RotorAfterRotation[] = "\n\nYOUR ROTOR CONFIGURATION AFTER ROTATION\n";
char UserPrompt1[] = "Enter the line(s) of text to be encrypted: ";
char UserPrompt2[] = "Enter the line(s) of text to be decrypted: ";
char EncryptedOutputPrompt[] = "ENCRYPTED MESSAGE: ";
char DecryptedOutputPrompt[] = "DECRYPTED MESSAGE: ";

int ValidateOptions(char* options);
int* ParseRotorIndices(char* rotorIndicesLine);
int GetNumRotors(int* rotorIndices);
int** SetUpRotors(int* rotorIndices);
void DisplayRotorConfiguration(int** rotorConfig, int numRows, int numCols);
int ValidateNumRotations(char* numRotationsInput);
int** RotateRotors(int numOfRotations, int** rotorSet, int numRows, int numCols);
char* Encrypt(char* message, int** rotorSet, int numRows, int numCols);
char* Decrypt(char* message, int** rotorSet, int numRows, int numCols);


int main()
{
	char userInput[MaxInputLength];
	int option;
	int* rotorConfig;
	int numRotors;
	int** rotorArray; 
	int numRotations;
	char userMsg[MaxMsgLength];
	char* msgOut;

	printf("%s", WelcomeMsg);

	while(1)
	{
		option = -1;
		while (option == -1)
		{
			printf("%s", MenuOptions);

			fgets(userInput, MaxInputLength, stdin);
			option = ValidateOptions(userInput);	

			if (option == -1)
			{
				printf("%s", WrongOption);
			}
		}
		
		if (option == 0)
		{
			printf("%s", ExitMessage);
			return 0;
		}

		printf("%s", option == 1 ? EncryptionMessage : DecryptionMessage);
		
		rotorConfig = NULL;
		while (rotorConfig == NULL)
		{
			printf("%s", RotorPrompt);
			fgets(userInput, MaxInputLength, stdin);
			rotorConfig = ParseRotorIndices(userInput);
		}
		
		numRotors = GetNumRotors(rotorConfig);
		rotorArray = SetUpRotors(rotorConfig);
		
		printf("%s", RotorBeforeRotation);
		DisplayRotorConfiguration(rotorArray, numRotors, AlphabetSize);
		
		numRotations = -1;
		while (numRotations == -1)
		{
			printf("%s", NumRotationsMsg);
			fgets(userInput, MaxInputLength, stdin);
			numRotations = ValidateNumRotations(userInput);
		}

		rotorArray = RotateRotors(numRotations, rotorArray, numRotors, AlphabetSize);

		printf("%s", RotorAfterRotation);
		DisplayRotorConfiguration(rotorArray, numRotors, AlphabetSize);
		
		printf("%s", option == 1 ? UserPrompt1 : UserPrompt2);

		fgets(userMsg, MaxMsgLength, stdin);

		msgOut = option == 1 ? 
					Encrypt(userMsg, rotorArray, numRotors, AlphabetSize) 
					: Decrypt(userMsg, rotorArray, numRotors, AlphabetSize);
		
		printf("%s", option == 1 ? EncryptedOutputPrompt : DecryptedOutputPrompt);
		printf("%s", msgOut);

		free(rotorConfig);
		rotorConfig = NULL;
		for (int i=0; i < MaxRotors; i++)
		{
			free(rotorArray[i]);
		}
		free(rotorArray);
		rotorArray = NULL;
		free(msgOut);
		msgOut = NULL;
	}

	return 0;
}

/* 
 * Validates user option. Valid option must a 0, 1, or 2 followed by newline
 * followed by null string
 * param options string of user input
 * returns valid user option as an integer, -1 otherwise.
 */
int ValidateOptions(char* options)
{
	if ('0' <= options[0] && options[0] <= '2' && options[1] == '\n' && options[2] == '\0')
	{
		return options[0] - '0';
	}

	return -1;
}

/*
 * Parses user input of up to three rotors indices separated by spaces without
 * duplicates.
 * param rotorIndicesLine user input of rotor indices separated by spaces
 * Returns pointer to array of user selections, end of user selection array is
 *  denoted with -1.
 */
int* ParseRotorIndices(char* rotorIndicesLine)
{
	int i = 0;
	int count = 0;
	int getInt = 1;
	int* config = malloc((MaxRotors + 1) * sizeof(int));

	if (rotorIndicesLine[0] == '\n' && rotorIndicesLine[1] == '\0')
	{
		printf("%s", NoRotorsMsg);
		free(config);
		return NULL;
	}

	for (i = 0; rotorIndicesLine[i] != '\0' && i < MaxInputLength; i++)
	{
		if (getInt == 1 && '0' <= rotorIndicesLine[i] && rotorIndicesLine[i] <= '9')
		{
			if (count >= MaxRotors)
			{
				printf("%s", MoreThanAllowed);
				free(config);
				return NULL;
			}
			config[count] = rotorIndicesLine[i] - '0';
			count++;
			getInt = 0;
		}
		else if (getInt == 0 && rotorIndicesLine[i] == ' ')
		{
			getInt = 1;
		}
		else if (rotorIndicesLine[i] == '\n')
		{
			continue;
		}
		else
		{
			printf("%s", InvalidRotorMsg);
			free(config);
			return NULL;
		}
	}

	for (int j=0; j < count; j++)
	{
		for (int k=j+1; k < count; k++)
		{
			if (config[j] == config[k])
			{
				printf("%s", DupRotorMsg);
				free(config);
				return NULL;
			}
		}
	}

	//mark end of configuration array with -1
	config[count] = -1;

	return config;
}

/*
 * Counts number of rotors selected
 * param rotorIndices array of selected rotor indices
 * returns count of rotor indices
 */
int GetNumRotors(int* rotorIndices)
{
	int numRotors = 0;
	while (rotorIndices[numRotors] != -1 && numRotors < MaxRotors)
	{
		numRotors++;
	}

	return numRotors;
}

/*
 * Allocates space for 2d-array of selected rotors in memory.
 * Caller is responsible for freeing memory when done.
 * param rotorIndices array of selected rotors
 * returns 2d-array of integers representing the position of each character of
 *  the rotor in English alphabet
 */
int** SetUpRotors(int* rotorIndices)
{
	int** rotors = malloc(MaxRotors * sizeof(int*));
	char* curRotor;

	for (int i=0; i < MaxRotors; i++)
	{
		rotors[i] = malloc(AlphabetSize * sizeof(int));
		curRotor = RotorConstants[rotorIndices[i]];
		for (int j=0; j < AlphabetSize; j++)
		{
			rotors[i][j] = curRotor[j] - 'A';
		}
	}

	return rotors;
}

/*
 * Prints rotor configuration to screen.
 * param rotorConfig pointer to 2d-array of rotor configuration
 * param numRows the number of rotors in the array
 * param numCols the number of columns in the array
 */
void DisplayRotorConfiguration(int** rotorConfig, int numRows, int numCols)
{
	printf("%s", RotorConfigurationHeader);
	for (int i=0; i < numRows; i++)
	{
		for (int j=0; j < numCols; j++)
		{ 
			printf("%c ", rotorConfig[i][j] + 'A');
		}
		printf("\n");
	}
	printf("%s", RotorConfigurationFooter);
	return;
}

/*
 * Validates user input for number of rotations
 * param numRotationsInput user input for number of rotations
 * returns number of rotations selected if valid, -1 otherwise.
 */
int ValidateNumRotations(char* numRotationsInput)
{
	if (numRotationsInput[0] == '\n' && numRotationsInput[1] == '\0')
	{
		return -1;
	}
	
	int output = 0;
	for (int i=0; numRotationsInput[i] != '\0'; i++)
	{
		if ('0' <= numRotationsInput[i] && numRotationsInput[i] <= '9')
		{
			output *= 10;
			output += numRotationsInput[i] - '0';
			if (output > 25)
			{
				printf("%s", InvalidNumRotations);
				return -1;
			}
		}
		else if (numRotationsInput[i] == '\n')
		{
			continue;
		}
		else
		{
			printf("%s", InvalidNumRotations);
			return -1;
		}
	}

	return output;
}

/*
 * Performs the selected rotation on the rotor set.
 * Allocates memory for the rotated rotor configuration. 
 * Caller is responsible for freeing memory when done.
 * param numRotations the number of rotations to perform on each rotor
 * param rotorSet 2d-array of rotor configuration
 * param numRows the number of rows in rotor configuration
 * param numCols the number of columns in rotor configuration
 * returns 2d-array of rotor configuration after rotation
 */
int** RotateRotors(int numRotations, int** rotorSet, int numRows, int numCols)
{
	if (numRotations == 0)
	{
		return rotorSet;
	}

	int swapArray[numCols];

	for (int i=0; i < numRows; i++)
	{
		for (int j=0; j < numCols; j++)
		{
			swapArray[j] = rotorSet[i][(j + numCols - numRotations) % numCols];
		}
		for (int j=0; j < numCols; j++)
		{
			rotorSet[i][j] = swapArray[j];
		}
	}

	return rotorSet;
}

/*
 * Encrypts message using the provided rotor configuration
 * param message User input message to encrypt. Converts alphabetic characters
 * in message to upper case
 * param rotorSet 2d-array of rotor configuration to use for encryption
 * param numRows the number of rows in the configuration array
 * param numCols the number of columns in the configuration array
 * returns pointer to encrypted message in memory which must be freed when no
 * longer needed
 */
char* Encrypt(char* message, int** rotorSet, int numRows, int numCols)
{
	int rotorIndex;

	int msgLen = 0;
	while (message[msgLen++] != '\0')
	{
		if ('a' <= message[msgLen] && message[msgLen] <= 'z')
		{
			message[msgLen] = message[msgLen] - 'a' + 'A';
		}
	}

	char copyMsg[msgLen];
	strcpy(copyMsg, message);

	for (int i=0; i < numRows; i++)
	{
		for (int j=0; copyMsg[j] != '\0'; j++)
		{
			if ('A' <= copyMsg[j] && copyMsg[j] <= 'Z')
			{
				rotorIndex = copyMsg[j] - 'A';
				copyMsg[j] = rotorSet[i][rotorIndex] + 'A';  
			}
		}
	}

	char* outMsg = malloc(msgLen * sizeof(char));
	strcpy(outMsg, copyMsg);

	return outMsg;
}

/*
 * Dencrypts message using the rotor configuration
 * param message User input message to decrypt. Converts alphabetic characters
 * in message to upper case
 * param rotorSet 2d-array of rotor configuration to use for encryption
 * param numRows the number of rows in the configuration array
 * param numCols the number of columns in the configuration array
 * returns pointer to decrypted message in memory which must be freed when no
 * longer needed
 */
char* Decrypt(char* message, int** rotorSet, int numRows, int numCols)
{
	int rotorVal;
	int rotorIdx;

	int msgLen = 0;
	while(message[msgLen++] != '\0')
	{
		if ('a' <= message[msgLen] && message[msgLen] <= 'z')
		{
			message[msgLen] = message[msgLen] - 'a' + 'A';
		}
	}

	char copyMsg[msgLen];
	strcpy(copyMsg, message);

	for (int i=0; i < numRows; i++)
	{
		for (int j=0; copyMsg[j] != '\0' ; j++)
		{
			if (copyMsg[j] < 'A'  && copyMsg[j] <= 'z')
			{
				continue;
			}
			rotorIdx = 0;
			rotorVal = copyMsg[j] - 'A';
			while (rotorIdx < numCols && rotorSet[i][rotorIdx] != rotorVal)
			{
				rotorIdx++;
			}
			if (rotorIdx == numCols)
			{
				printf("Letter %c not found in rotor %d\n", message[j], i);
				return NULL;
			}
			copyMsg[j] = 'A' + rotorIdx; 
		}
	}

	char* outMsg = malloc(msgLen * sizeof(char));
	strcpy(outMsg, copyMsg);

	return outMsg;
}
