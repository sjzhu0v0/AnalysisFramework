#ifndef MMath_h
#define MMath_h

#include "MHead.h"

class MDouble {
public:
  double fValue;
  double fError;

  MDouble() {
    fValue = 0;
    fError = 0;
  }

  MDouble(double value, double error) {
    fValue = value;
    fError = error;
  }

  ~MDouble() {}

  void Print() { cout << fValue << " +/- " << fError << endl; }

  MDouble operator+(MDouble rhs) {
    return MDouble(fValue + rhs.fValue,
                   sqrt(fError * fError + rhs.fError * rhs.fError));
  }
  MDouble operator-(MDouble rhs) {
    return MDouble(fValue - rhs.fValue,
                   sqrt(fError * fError + rhs.fError * rhs.fError));
  }
  MDouble operator*(MDouble rhs) {
    return MDouble(
        fValue * rhs.fValue,
        fValue * rhs.fValue *
            sqrt(pow(fError / fValue, 2) + pow(rhs.fError / rhs.fValue, 2)));
  }
  MDouble operator/(MDouble rhs) {
    return MDouble(
        fValue / rhs.fValue,
        fValue / rhs.fValue *
            sqrt(pow(fError / fValue, 2) + pow(rhs.fError / rhs.fValue, 2)));
  }
  // MDouble operator*(MDouble rhs) {
  //   return MDouble(fValue * rhs.fValue, sqrt(pow(fError * rhs.fValue, 2) +
  //                                            pow(fValue * rhs.fError, 2)));
  // }
  // MDouble operator/(MDouble rhs) {
  //   return MDouble(fValue / rhs.fValue,
  //                  sqrt(pow(fError / rhs.fValue, 2) +
  //                       pow(fValue / rhs.fValue * rhs.fError / rhs.fValue, 2)));
  // }
  MDouble operator+(double rhs) { return MDouble(fValue + rhs, fError); }
  MDouble operator-(double rhs) { return MDouble(fValue - rhs, fError); }
  MDouble operator*(double rhs) { return MDouble(fValue * rhs, fError * rhs); }
  MDouble operator/(double rhs) { return MDouble(fValue / rhs, fError / rhs); }

  MDouble operator+=(MDouble rhs) {
    fValue += rhs.fValue;
    fError = sqrt(fError * fError + rhs.fError * rhs.fError);
    return *this;
  }
  MDouble operator-=(MDouble rhs) {
    fValue -= rhs.fValue;
    fError = sqrt(fError * fError + rhs.fError * rhs.fError);
    return *this;
  }
  MDouble operator*=(MDouble rhs) {
    fValue *= rhs.fValue;
    fError = fValue * rhs.fValue *
             sqrt(pow(fError / fValue, 2) + pow(rhs.fError / rhs.fValue, 2));
    return *this;
  }
  MDouble operator/=(MDouble rhs) {
    fValue /= rhs.fValue;
    fError = fValue / rhs.fValue *
             sqrt(pow(fError / fValue, 2) + pow(rhs.fError / rhs.fValue, 2));
    return *this;
  }
  MDouble operator+=(double rhs) {
    fValue += rhs;
    return *this;
  }
  MDouble operator-=(double rhs) {
    fValue -= rhs;
    return *this;
  }
  MDouble operator*=(double rhs) {
    fValue *= rhs;
    fError *= rhs;
    return *this;
  }
  MDouble operator/=(double rhs) {
    fValue /= rhs;
    fError /= rhs;
    return *this;
  }
};

#endif