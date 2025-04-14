#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>
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

vector<bool> floatToBinary(float& number) {
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
        return 3; //Valor Infinito
    }
}

void printBinaryFloat(vector<bool> binaryFloat, float number) {
    switch (verify(binaryFloat)) {
        case 1:
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
            break;
        case 2:
            cout << "Valor NaN" <<endl;
        break;
        case 3:
            if (binaryFloat[0]) {
                cout << "-";
            } else {
                cout << "+";
            }
        cout << "Infinito" << endl;
        break;
    }
}

int binaryToInteger(vector<bool> binaryFloat) {
    int integer = 0;
    for (int i = 8; i > 0; i--) {
        if (binaryFloat[i] == 1) {
            integer += pow(2, 8 - i);
        }
    }
    return integer;
}

vector<bool> significandConversion(vector<bool> binaryFloat, int exponent) {
    vector<bool> significandImplicit;
    // Bit implícito: 1 para normales, 0 para subnormales/cero
    bool implicitBit = (exponent != 0);
    significandImplicit.push_back(implicitBit);

    // Añadir los 23 bits de la mantisa (bits 9-31)
    for (int i = 9; i < 32; i++) {
        significandImplicit.push_back(binaryFloat[i]);
    }

    return significandImplicit;
}

void normalizeSubnormal(int& exponent, vector<bool>& significand) {
    if (exponent != 0) return; // Solo normalizar si es subnormal

    int shift = 0;
    // Encontrar el primer 1 en la mantisa
    for (size_t i = 1; i < significand.size(); i++) {
        if (significand[i]) {
            shift = i - 1;
            break;
        }
    }

    // Desplazar la mantisa
    for (size_t i = 1; i < significand.size() - shift; i++) {
        significand[i] = significand[i + shift];
    }
    for (size_t i = significand.size() - shift; i < significand.size(); i++) {
        significand[i] = false;
    }

    // Ajustar exponente
    exponent = 1 - shift;
}

bool isGreaterOrEqual(const vector<bool>& a, const vector<bool>& b) {
    if (a.size() != b.size()) return a.size() > b.size();
    for (size_t i = 0; i < a.size(); ++i) {
        if (a[i] && !b[i]) return true;
        if (!a[i] && b[i]) return false;
    }
    return true;
}

vector<bool> subtractMantissas(vector<bool> a, const vector<bool>& b) {
    bool borrow = false;
    for (int i = a.size()-1; i >= 0; --i) {
        bool b_val = i < b.size() ? b[i] : false;
        if (borrow) {
            if (a[i]) {
                a[i] = false;
                borrow = false;
            } else {
                a[i] = true;
            }
        }
        if (!a[i] && b_val) {
            borrow = true;
            a[i] = true;
        } else {
            a[i] = a[i] ^ b_val;
        }
    }
    return a;
}


vector<bool> divideSignificands(vector<bool> mantisa1, vector<bool> mantisa2) {
    vector<bool> resultado;
    vector<bool> acumulador(mantisa1.size(), false);

    // Asegurarnos que ambas mantisas tengan el mismo tamaño
    if (mantisa1.size() < mantisa2.size()) {
        mantisa1.insert(mantisa1.begin(), mantisa2.size() - mantisa1.size(), false);
    } else if (mantisa2.size() < mantisa1.size()) {
        mantisa2.insert(mantisa2.begin(), mantisa1.size() - mantisa2.size(), false);
    }

    // Agregar bits extra para precisión
    int bitsExtra = mantisa1.size();
    mantisa1.insert(mantisa1.end(), bitsExtra, false);

    // División bit a bit
    for (size_t i = 0; i < mantisa1.size(); ++i) {
        // Desplazar el acumulador a la izquierda
        acumulador.erase(acumulador.begin());
        acumulador.push_back(mantisa1[i]);

        // Comparar acumulador con divisor (mantisa2)
        if (isGreaterOrEqual(acumulador, mantisa2)) {
            // Restar mantisa2 del acumulador
            acumulador = subtractMantissas(acumulador, mantisa2);
            resultado.push_back(true);
        } else {
            resultado.push_back(false);
        }
    }

    // Normalizar el resultado (eliminar ceros a la izquierda)
    auto first_one = find(resultado.begin(), resultado.end(), true);
    if (first_one != resultado.end()) {
        resultado.erase(resultado.begin(), first_one);
    } else {
        return vector<bool>(mantisa1.size(), false); // Resultado cero
    }

    // Asegurar que el resultado tenga el tamaño correcto
    if (resultado.size() > mantisa1.size()) {
        resultado.resize(mantisa1.size());
    } else if (resultado.size() < mantisa1.size()) {
        resultado.insert(resultado.begin(), mantisa1.size() - resultado.size(), false);
    }

    return resultado;
}

vector<bool> divideMantissas(vector<bool> dividend, vector<bool> divisor) {
    vector<bool> result;
    vector<bool> remainder(dividend.size(), false);

    // Añadir bits extra para precisión
    dividend.insert(dividend.end(), dividend.size(), false);

    for (size_t i = 0; i < dividend.size(); ++i) {
        remainder.erase(remainder.begin());
        remainder.push_back(dividend[i]);

        if (isGreaterOrEqual(remainder, divisor)) {
            remainder = subtractMantissas(remainder, divisor);
            result.push_back(true);
        } else {
            result.push_back(false);
        }
    }

    // Normalizar resultado
    auto first_one = find(result.begin(), result.end(), true);
    if (first_one != result.end()) {
        result.erase(result.begin(), first_one);
    } else {
        return vector<bool>(24, false);
    }

    if (result.size() > 24) {
        result.resize(24);
    } else if (result.size() < 24) {
        result.insert(result.begin(), 24 - result.size(), false);
    }

    return result;
}

vector<bool> divisionFloatPoint(float number1, vector<bool> binaryNumber1, float number2, vector<bool> binaryNumber2) {
    // Extraer componentes
    bool sign1 = binaryNumber1[0];
    int exponent1 = 0;
    for (int i = 0; i < 8; i++) {
        if (binaryNumber1[1+i]) exponent1 += (1 << (7-i));
    }
    vector<bool> mantissa1(binaryNumber1.begin()+9, binaryNumber1.end());

    bool sign2 = binaryNumber2[0];
    int exponent2 = 0;
    for (int i = 0; i < 8; i++) {
        if (binaryNumber2[1+i]) exponent2 += (1 << (7-i));
    }
    vector<bool> mantissa2(binaryNumber2.begin()+9, binaryNumber2.end());

    // Casos especiales
    if (number2 == 0) {
        if (number1 == 0) return vector<bool>{0,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}; // NaN
        return vector<bool>{sign1^sign2,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}; // ±Inf
    }

    // Añadir bit implícito
    if (exponent1 != 0) mantissa1.insert(mantissa1.begin(), true);
    else mantissa1.insert(mantissa1.begin(), false);

    if (exponent2 != 0) mantissa2.insert(mantissa2.begin(), true);
    else mantissa2.insert(mantissa2.begin(), false);

    // Normalizar subnormales
    if (exponent1 == 0 && mantissa1[0] == false) {
        int shift = 0;
        for (size_t i = 1; i < mantissa1.size(); i++) {
            if (mantissa1[i]) {
                shift = i-1;
                break;
            }
        }
        rotate(mantissa1.begin(), mantissa1.begin()+shift+1, mantissa1.end());
        fill(mantissa1.end()-shift-1, mantissa1.end(), false);
        exponent1 = 1 - shift;
    }

    if (exponent2 == 0 && mantissa2[0] == false) {
        int shift = 0;
        for (size_t i = 1; i < mantissa2.size(); i++) {
            if (mantissa2[i]) {
                shift = i-1;
                break;
            }
        }
        rotate(mantissa2.begin(), mantissa2.begin()+shift+1, mantissa2.end());
        fill(mantissa2.end()-shift-1, mantissa2.end(), false);
        exponent2 = 1 - shift;
    }

    // Dividir mantisas
    vector<bool> resultMantissa = divideMantissas(mantissa1, mantissa2);

    // Calcular exponente resultante
    int resultExponent = exponent1 - exponent2 + 127;

    // Normalizar resultado
    if (!resultMantissa.empty() && resultMantissa[0] == false) {
        int shift = 0;
        for (size_t i = 1; i < resultMantissa.size(); i++) {
            if (resultMantissa[i]) {
                shift = i-1;
                break;
            }
        }
        rotate(resultMantissa.begin(), resultMantissa.begin()+shift+1, resultMantissa.end());
        fill(resultMantissa.end()-shift-1, resultMantissa.end(), false);
        resultExponent -= shift;
    }

    // Construir resultado
    vector<bool> result(32, false);
    result[0] = sign1 ^ sign2;

    for (int i = 0; i < 8; i++) {
        result[1+i] = (resultExponent >> (7-i)) & 1;
    }

    for (int i = 0; i < 23; i++) {
        if (i+1 < resultMantissa.size()) {
            result[9+i] = resultMantissa[i+1];
        }
    }

    return result;
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