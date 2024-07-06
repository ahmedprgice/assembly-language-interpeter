w#include <iostream>
#include <cmath>
#include <vector>
#include <fstream>
#include <iomanip>

using namespace std;

// Constants for memory, register, and flags sizes
const int MEMORY_SIZE = 64;
const int REGISTER_SIZE = 7;
const int FLAGS_SIZE = 4;

// Initialize global variables
int memory[MEMORY_SIZE];
string registers[REGISTER_SIZE] = {"R0", "R1", "R2", "R3", "R4", "R5", "R6"};
bool flags[FLAGS_SIZE];
int counter = 1;
ofstream output;

// Class for MOV operations
class Operations
{
private:
    // Function to get the index of a register by name
    int getRegisterIndex(const string &registerName);

    // Function to get the value of a register by name
    int getRegisterValue(const string &registerName);

    // Function to update the value of a register by index
    void updateRegisterValue(int registerIndex, int newValue);

    void updateFlags(int &value);

    // Helper method to convert decimal to binary
    vector<int> decimalToBinary(int n);

    // Helper method to convert binary to decimal
    int binaryToDecimal(const vector<int> &binaryNum);

public:
    // Method to perform MOV operation
    void mov(const vector<string> &command);

    // Methods for arithmetic operations
    void performMathOperation(const vector<string> &command);

    // Methods for incrementing and decrementing
    void incrementAndDecrement(const vector<string> &command);

    // Methods for rotation and shift operations
    void rotateAndShift(const vector<string> &command);

    // Methods for input and output operations
    void input(const vector<string> &command);

    void output(const vector<string> &command);

    // Methods for store and load operations
    void store(const vector<string> &command);

    void load(const vector<string> &command);
};

// Function to update flags based on a value
void Operations::updateFlags(int &value)
{
    // Check if the value exceeds the maximum limit
    if (value > 255)
    {
        value = 0;
        flags[0] = flags[1] = flags[3] = 1; // Set Carry Flag (CF), Overflow Flag (OF), and Zero Flag (ZF)
    }
    // Check if the value is negative
    else if (value < 0)
    {
        value = 255;
        flags[2] = 1; // Set Underflow Flag (UF)
    }
    // Check if the value is zero
    else if (value == 0)
        flags[3] = 1; // Set Zero Flag (ZF)
}

// Function to get the index of a register from its name
int Operations::getRegisterIndex(const string &registerName)
{
    // Iterate through the registers to find the index corresponding to the given register name
    for (int i = 0; i < REGISTER_SIZE; i++)
        if (registerName == "R" + to_string(i))
            return i;
    return -1; // Return -1 if the register name is not found
}

// Function to get the value of a register or a constant
int Operations::getRegisterValue(const string &operand)
{
    // Check if the operand is a register
    if (operand.substr(0, 1) == "R")
    {
        int registerIndex = stoi(operand.substr(1));
        return stoi(registers[registerIndex]); // Return the value stored in the specified register
    }
    return stoi(operand); // Return the constant value if the operand is not a register
}

// Function to update the value of a register
void Operations::updateRegisterValue(int registerIndex, int newValue)
{
    registers[registerIndex] = to_string(newValue); // Update the value of the specified register
}

// Move operation implementation within the Operations class
void Operations::mov(const vector<string> &command)
{
    int destinationIndex = getRegisterIndex(command[2]);

    // Check if the destination register is valid
    if (destinationIndex != -1)
    {
        int sourceValue = 0;
        int sourceIndex = getRegisterIndex(command[1]);

        // Retrieve the source value based on the source operand
        if (sourceIndex != -1)
        {
            sourceValue = stoi(registers[sourceIndex]); // Source is another register
        }
        else if (command[1][0] == '[')
        {
            int memoryLocation = stoi(registers[command[1][2] - '0']);

            // Check if the memory location is within bounds
            if (memoryLocation >= 0 && memoryLocation < MEMORY_SIZE)
                sourceValue = memory[memoryLocation]; // Source is a memory address
        }
        else
            sourceValue = stoi(command[1]); // Source is a constant value

        // Update flags and the destination register with the source value
        updateFlags(sourceValue);
        updateRegisterValue(destinationIndex, sourceValue);
    }
    else
    {
        // Invalid destination register
        cerr << "Error: Invalid destination register." << endl;
    }
}

// Addition operation implementation within the Operations class
void Operations::performMathOperation(const vector<string> &command)
{
    int destinationIndex = getRegisterIndex(command[2]);

    // Check if the destination register is valid
    if (destinationIndex != -1)
    {
        int currentRegisterValue = stoi(registers[destinationIndex]); // Retrieve the current value of the destination register
        int sourceValue = getRegisterValue(command[1]);               // Retrieve the value of the source operand

        // Variable to store the result of the mathematical operation
        int result;

        // Perform the specified arithmetic operation based on the command
        if (command[0] == "ADD")
        {
            result = currentRegisterValue + sourceValue;
        }
        else if (command[0] == "SUB")
        {
            result = currentRegisterValue - sourceValue;
        }
        else if (command[0] == "MUL")
        {
            result = currentRegisterValue * sourceValue;
        }
        else if (command[0] == "DIV")
        {
            if (sourceValue != 0)
            {
                result = currentRegisterValue / sourceValue;
            }
        }

        // Update flags based on the result of the operation
        updateFlags(result);

        // Update the destination register with the result of the operation
        updateRegisterValue(destinationIndex, result);
    }
    else
    {
        // Invalid destination register
        cerr << "Error: Invalid destination register." << endl;
    }
}

void Operations::incrementAndDecrement(const vector<string> &command)
{
    // Get the index of the register specified in the command
    int registerIndex = getRegisterIndex(command[1]);

    // Check if the specified register is valid
    if (registerIndex != -1)
    {
        // Retrieve the current value of the register
        int result = stoi(registers[registerIndex]);

        // Perform the increment or decrement based on the command
        if (command[0] == "INC")
            result++; // Increment the value
        else if (command[0] == "DEC")
            result--; // Decrement the value

        // Update flags based on the result of the operation
        updateFlags(result);

        // Update the register with the result of the operation
        updateRegisterValue(registerIndex, result);
    }
    else
    {
        // Invalid register specified in the command
        cerr << "Error: Invalid register specified." << endl;
    }
}


// Convert decimal number to binary representation
vector<int> Operations::decimalToBinary(int decimalNumber)
{
    vector<int> binaryRepresentation(8, 0); // Initialize a vector to store the binary representation, assuming 8 bits

    int i = 0;

    // Extract binary digits by dividing the decimal number by 2
    while (decimalNumber > 0 && i < 8)
    {
        binaryRepresentation[i] = decimalNumber % 2; // Store the remainder (binary digit) in the vector
        decimalNumber = decimalNumber / 2; // Divide the decimal number by 2 to move to the next bit
        i++; // Move to the next position in the binary representation vector
    }

    return binaryRepresentation; // Return the vector containing the binary representation
}


// Convert binary representation to decimal value
int Operations::binaryToDecimal(const vector<int> &binaryRepresentation)
{
    int decimalValue = 0, i = 0;

    // Iterate through each bit in the binary representation
    for (int bit : binaryRepresentation)
    {
        // Calculate the decimal value by multiplying each bit by 2 raised to the corresponding power
        decimalValue += bit * pow(2, i);
        ++i; // Move to the next power of 2
    }

    return decimalValue; // Return the final decimal value
}

// Rotate operation implementation within the Operations class
void Operations::rotateAndShift(const vector<string> &command)
{
    // Get the index of the register specified in the command
    int registerIndex = getRegisterIndex(command[1]);

    // Check if the specified register is valid
    if (registerIndex != -1)
    {
        // Retrieve the current value of the register
        int registerValue = getRegisterValue(command[1]);
        vector<int> binaryRepresentation = decimalToBinary(registerValue);

        int amount = stoi(command[2]);
        int size = binaryRepresentation.size();
        vector<int> resultBinary(size, 0);

        if (command[0] == "ROL")
        {
            // Rotate Left (ROL): Circularly shift bits to the left
            amount = amount % size; // Ensure that the shift amount is within the size of the binary representation
            for (int j = 0; j < size; j++)
            {
                int newPosition = (j + amount) % size; // Calculate the new position after rotation
                resultBinary[newPosition] = binaryRepresentation[j];
            }
        }
        else if (command[0] == "ROR")
        {
            // Rotate Right (ROR): Circularly shift bits to the right
            amount = amount % size; // Ensure that the shift amount is within the size of the binary representation
            for (int j = 0; j < size; j++)
            {
                int newPosition = (j - amount + size) % size; // Calculate the new position after rotation
                resultBinary[newPosition] = binaryRepresentation[j];
            }
        }
        else if (command[0] == "SHL")
        {
            // Shift Left: Simply shift the bits to the left, filling with zeros
            for (int j = 0; j < size - amount; j++)
            {
                resultBinary[j + amount] = binaryRepresentation[j];
            }
        }
        else if (command[0] == "SHR")
        {
            // Shift Right: Simply shift the bits to the right, filling with zeros
            for (int j = 0; j < size - amount; j++)
            {
                resultBinary[j] = binaryRepresentation[j + amount];
            }
        }

        int resultDecimalValue = binaryToDecimal(resultBinary);

        // Update flags based on the result of the operation
        updateFlags(resultDecimalValue);

        // Update the register with the result of the operation
        updateRegisterValue(registerIndex, resultDecimalValue);
    }
    else
    {
        // Invalid register specified in the command
        cerr << "Error: Invalid register specified." << endl;
    }
}

// Input operation implementation within the Operations class
void Operations::input(const vector<string> &command)
{
    int destinationIndex = getRegisterIndex(command[1]);

    // Check if the destination register is valid
    if (destinationIndex != -1)
    {
        int inputValue = 0;
        cout << "User input => ";
        cin >> inputValue;

        // Update flags and the destination register with the user input
        updateFlags(inputValue);
        updateRegisterValue(destinationIndex, inputValue);
    }
    else
    {
        // Invalid destination register
        cerr << "Error: Invalid destination register." << endl;
    }
}

// Output operation implementation within the Operations class
void Operations::output(const vector<string> &command)
{
    int sourceValue = getRegisterValue(command[1]);
    cout << "Output screen: " << sourceValue << endl;
}

// Store operation implementation within the Operations class
void Operations::store(const vector<string> &command)
{
    int sourceRegisterIndex = getRegisterIndex(command[1]);

    // Check if the source register is valid
    if (sourceRegisterIndex != -1)
    {
        int sourceValue = stoi(registers[sourceRegisterIndex]);

        // Check if the destination is a memory address specified by a register
        if (command[2].find("[R") == 0 && command[2].back() == ']')
        {
            int registerIndex = stoi(command[2].substr(2, command[2].size() - 3));

            // Check if the register index is within bounds
            if (registerIndex >= 0 && registerIndex < MEMORY_SIZE)
                memory[stoi(registers[registerIndex])] = sourceValue;
        }
        // Check if the destination is a direct memory address
        else
        {
            int directMemoryAddress = stoi(command[2]);

            // Check if the direct memory address is within bounds
            if (directMemoryAddress >= 0 && directMemoryAddress < MEMORY_SIZE)
                memory[directMemoryAddress] = sourceValue;
        }
    }
    else
    {
        // Invalid destination register
        cerr << "Error: Invalid destination register." << endl;
    }
}

// Load operation implementation within the LoadAndStore class
void Operations::load(const vector<string> &command)
{
    int destinationRegisterIndex = getRegisterIndex(command[1]);

    // Check if the destination register is valid
    if (destinationRegisterIndex != -1)
    {
        int memoryValue;

        // Check if the source is a memory address specified by a register
        if (command[2].find("[R") == 0 && command[2].back() == ']')
        {
            int registerIndex = stoi(command[2].substr(2, command[2].size() - 3));

            // Check if the register index is within bounds
            if (registerIndex >= 0 && registerIndex < MEMORY_SIZE)
                memoryValue = memory[stoi(registers[registerIndex])];
        }
        // Check if the source is a direct memory address
        else
        {
            int directMemoryAddress = stoi(command[2]);

            // Check if the direct memory address is within bounds
            if (directMemoryAddress >= 0 && directMemoryAddress < MEMORY_SIZE)
                memoryValue = memory[directMemoryAddress];
        }

        // Update the destination register with the value from memory
        registers[destinationRegisterIndex] = to_string(memoryValue);
    }
    else
    {
        // Invalid destination register
        cerr << "Error: Invalid destination register." << endl;
    }
}
// Execute operation based on the command type
void execute(vector<string> &command)
{
    Operations commands;

    // Check the command type and execute the corresponding operation
    if (command[0] == "MOV")
        commands.mov(command);
    else if (command[0] == "ADD" || command[0] == "SUB" || command[0] == "DIV" || command[0] == "MUL")
        commands.performMathOperation(command);
    else if (command[0] == "INC" || command[0] == "DEC")
        commands.incrementAndDecrement(command);
    else if (command[0] == "ROL" || command[0] == "ROR" || command[0] == "SHL" || command[0] == "SHR")
        commands.rotateAndShift(command);
    else if (command[0] == "IN")
        commands.input(command);
    else if (command[0] == "OUT")
        commands.output(command);
    else if (command[0] == "STORE")
        commands.store(command);
    else if (command[0] == "LOAD")
        commands.load(command);
    else
        cerr << "Error: Invalid command on line " << counter << endl;
}
// Display registers, including PC (Program Counter)
void displayRegisters()
{
    cout << setfill(' ') << setw(1) << "  0"
         << setfill(' ') << setw(5)
         << "1"
         << setfill(' ') << setw(5)
         << "2"
         << setfill(' ') << setw(5)
         << "3"
         << setfill(' ') << setw(5)
         << "4"
         << setfill(' ') << setw(5)
         << "5"
         << setfill(' ') << setw(5)
         << "6"
         << setfill(' ') << setw(5) << endl;

    cout << setw(0);

    cout << setfill('-') << setw(35) << "";
    cout << "          " << setfill('-') << setw(3) << "" << endl;

    cout << "|";
    for (int j = 0; j < REGISTER_SIZE; j++)
    {
        cout << setfill(' ') << setw(3) << registers[j] << " |";
    }

    cout << "      PC |" << counter << "|";
    counter++;
    cout << endl;

    cout << setfill('-') << setw(35) << "";
    cout << "          " << setfill('-') << setw(3) << "" << endl;
}

// Display flags (CF, OF, UF, ZF)
void displayFlags()
{
    cout << "  CF"
         << "   OF"
         << "   UF"
         << "   ZF" << endl;

    cout << setfill('-') << setw(20) << "" << endl;

    cout << "|";
    for (int i = 0; i < FLAGS_SIZE; i++)
        cout << " " << (flags[i] ? '1' : ' ') << " | ";
    cout << endl;

    cout << setfill('-') << setw(20) << "" << endl;

    // Reset flags after displaying
    for (int i = 0; i < FLAGS_SIZE; i++)
        flags[i] = 0;
}

// Display memory contents
void displayMemory()
{
    cout << "   0"
         << setfill(' ') << setw(7)
         << "1"
         << setfill(' ') << setw(7)
         << "2"
         << setfill(' ') << setw(7)
         << "3"
         << setfill(' ') << setw(7)
         << "4"
         << setfill(' ') << setw(7)
         << "5"
         << setfill(' ') << setw(7)
         << "6"
         << setfill(' ') << setw(7)
         << "7" << endl;

    cout << setfill('-') << setw(55) << "" << endl;

    for (int i = 0; i < MEMORY_SIZE; i++)
    {
        cout << setfill(' ') << setw(3);

        if (i % 8 == 0)
            cout << "|  ";

        if (memory[i] == 0)
            cout << " ";
        else
            cout << memory[i];

        cout << "  |"
             << " ";

        if ((i + 1) % 8 == 0)
        {
            cout << endl;
            cout << setfill('-') << setw(55) << "" << endl;
        }
    }

    cout << setfill(' ') << setw(37) << ""
         << "61"
         << " "
         << "    62"
         << " "
         << "    63";
    cout << endl
         << endl;
}






