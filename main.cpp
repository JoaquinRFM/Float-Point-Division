#include <iostream>
#include <vector>
#include <cmath>
using namespace std;

void getData(float& n1, float& n2) {
    cout << "Number 1: ";
    cin >> n1;
    cout << "Number 2: ";
    cin >> n2;
}

vector<bool> exponendToBinary(int exponent) {
    vector<bool> binaryExponent;
    int temp = exponent;
    while (temp > 0) {
        binaryExponent.insert(binaryExponent.begin(), (temp % 2));
        temp /= 2;
    }
    return binaryExponent;
}

vector<bool> significandToBinary(float significand) {
    vector<bool> binarySignificand(23, false);
    float temp = significand;
    for (int i = 0; i < 23; i++) {
        if (temp != 0) {
            temp *= 2;
            if (temp < 1) {
                binarySignificand[i] = false;
            } else {
                binarySignificand[i] = true;
                temp -= 1;
            }
        }
    }
    return binarySignificand;
}

vector<bool> mergeBinary(bool sign, vector<bool> binaryExponent, vector<bool> binarySignificand) {
    vector<bool> binaryMerged;
    binaryMerged.push_back(sign);
    for (int i = 0; i < binaryExponent.size(); i++) {
        binaryMerged.push_back(binaryExponent[i]);
    }
    for (int i = 0; i < binarySignificand.size(); i++) {
        binaryMerged.push_back(binarySignificand[i]);
    }
    return binaryMerged;
}

vector<bool> floatToBinary(float number) {
    vector<bool> binaryFloat(32, false);
    bool sign = number < 0;
    float absNumber = abs(number);
    if (absNumber == 0) {
        return binaryFloat;
    }
    if (absNumber < 1) {
        float temp = absNumber;
        int movement = 0;
        while (temp < 1) {
            temp *= 2;
            movement++;
        }
        float significand = temp - floor(temp);
        vector<bool> binarySignificand = significandToBinary(significand);
        vector<bool> binaryExponent = exponendToBinary(127 - movement);
        while (binaryExponent.size() < 8) {
            binaryExponent.insert(binaryExponent.begin(), 0);
        }
        binaryFloat = mergeBinary(sign, binaryExponent, binarySignificand);
        return binaryFloat;
    }
    int exponent = 0;
    float temp = absNumber;
    while (temp >= 2) {
        temp /= 2;
        exponent++;
    }
    float significand = absNumber / pow(2, exponent) - 1;
    vector<bool> binarySignificand = significandToBinary(significand);
    vector<bool> binaryExponent = exponendToBinary(127 + exponent);
    while (binaryExponent.size() < 8) {
        binaryExponent.insert(binaryExponent.begin(), 0);
    }
    binaryFloat = mergeBinary(sign, binaryExponent, binarySignificand);
    return binaryFloat;
}

int verify(vector<bool> binaryFloat) {
    for (int i = 1; i < 9; i++) {
        if (binaryFloat[i] == 0) {
            return 1; //Valor normalizado, no normalizado o 0
        }
    }
    for (int i = 9; i < 32; i++) {
        if (binaryFloat[i] == 1) {
            return 2; //Valor NaN
        }
    }
    return 3; //Valor Infinito
}

void printBinaryFloat(vector<bool> binaryFloat, float number) {
    cout << number << " en binario es: ";
    cout << binaryFloat[0] << " ";
    for (int i = 1; i < 9; i++) {
        cout << binaryFloat[i];
    }
    cout << " ";
    for (int i = 9; i < 32; i++) {
        cout << binaryFloat[i];
    }
    cout << endl;
}

int exponentToInt(const vector<bool>& binaryNumber) {
    int exponent = 0;
    for(int i = 1; i <= 8; i++) {
        exponent = (exponent << 1) | binaryNumber[i];
    }
    return exponent;
}

vector<bool> significandImplicit(vector<bool> binaryNumber){
    vector<bool> significand;
    significand.push_back(1);
    for (int i = 9; i < 32; i++){
        significand.push_back(binaryNumber[i]);
    }
    return significand;
}

vector<bool> divisionSignificand(vector<bool> significand1, vector<bool> significand2) {
    vector<bool> quotient(23, false);

    // Convert significands to decimal for division
    double s1 = 1.0; // Implicit 1
    double s2 = 1.0;
    double power = 0.5;

    for (int i = 1; i < significand1.size(); i++) {
        if (significand1[i]) s1 += power;
        power /= 2.0;
    }

    power = 0.5;
    for (int i = 1; i < significand2.size(); i++) {
        if (significand2[i]) s2 += power;
        power /= 2.0;
    }

    // Perform division
    double result = s1 / s2;

    // Handle normalization
    int shift = 0;
    while (result >= 2.0) {
        result /= 2.0;
        shift++;
    }
    while (result < 1.0 && result != 0) {
        result *= 2.0;
        shift--;
    }

    // Convert quotient back to binary
    if (result >= 1.0) result -= 1.0; // Remove implicit 1
    for (int i = 0; i < 23 && result > 0; i++) {
        result *= 2.0;
        if (result >= 1.0) {
            quotient[i] = true;
            result -= 1.0;
        }
    }

    return quotient;
}

vector<bool> divisionFloatPoint(float number1, vector<bool> binaryNumber1, float number2, vector<bool> binaryNumber2){
    bool sign1 = binaryNumber1[0];
    bool sign2 = binaryNumber2[0];
    bool finalSign = sign1 ^ sign2;
    int exponent1 = exponentToInt(binaryNumber1);
    int exponent2 = exponentToInt(binaryNumber2);
    vector<bool> finalExponent = exponendToBinary(exponent1 - exponent2 + 127);
    vector<bool> significand1 = significandImplicit(binaryNumber1);
    vector<bool> significand2 = significandImplicit(binaryNumber2);
    vector<bool> finalSignificand = divisionSignificand(significand1, significand2);
    vector<bool> resultBinary = mergeBinary(finalSign, finalExponent, finalSignificand);
    return resultBinary;
}

int main() {
    float number1, number2;
    getData(number1, number2);
    vector<bool> binaryNumber1 = floatToBinary(number1);
    printBinaryFloat(binaryNumber1, number1);
    vector<bool> binaryNumber2 = floatToBinary(number2);
    printBinaryFloat(binaryNumber2, number2);
    vector<bool> binaryResult = divisionFloatPoint(number1, binaryNumber1, number2, binaryNumber2);
    printBinaryFloat(binaryResult, 0);

}