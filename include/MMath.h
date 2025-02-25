#ifndef MMath_h
#define MMath_h

#include "MHead.h"
#include "TComplex.h"

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
  //                       pow(fValue / rhs.fValue * rhs.fError / rhs.fValue,
  //                       2)));
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

class MComplex {
public:
  MComplex() {
    re = 0;
    im = 0;
    re_err = 0;
    im_err = 0;
  }
  MComplex(double r, double i, double r_err, double i_err) {
    re = r;
    im = i;
    re_err = r_err;
    im_err = i_err;
  }
  MComplex(TComplex c, TComplex c_err) {
    re = c.Re();
    im = c.Im();
    re_err = c_err.Re();
    im_err = c_err.Im();
  }

  double Re() { return re; }
  double Im() { return im; }
  double ReErr() { return re_err; }
  double ImErr() { return im_err; }

  void SetRe(double r) { re = r; }
  void SetIm(double i) { im = i; }
  void SetReErr(double r_err) { re_err = r_err; }
  void SetImErr(double i_err) { im_err = i_err; }

  double Mag() { return sqrt(re * re + im * im); }
  double MagErr() {
    return sqrt(pow(re * re_err, 2) + pow(im * im_err, 2)) / Mag();
  }
  double Mag2() { return re * re + im * im; }
  double Mag2Err() { return sqrt(pow(re * re_err, 2) + pow(im * im_err, 2)); }
  double Phase() { return atan2(im, re); }
  double PhaseErr() {
    return sqrt(pow(re * im_err, 2) + pow(im * re_err, 2)) /
           (re * re + im * im);
  }

  MComplex operator^(double c) {
    double mag = pow(Mag(), c);
    double phase = Phase() * c;
    double mag_err = abs(c * pow(Mag(), c - 1) * MagErr());
    double phase_err = abs(c * PhaseErr());
    double re_err = mag_err * cos(phase) - mag * sin(phase) * phase_err;
    double im_err = mag_err * sin(phase) + mag * cos(phase) * phase_err;
    return MComplex(mag * cos(phase), mag * sin(phase), re_err, im_err);
  }
  MComplex &operator!() {
    im = -im;
    im_err = abs(im_err);
    return *this;
  }
  double operator()(int i) {
    if (i == 0) {
      return re;
    } else if (i == 1) {
      return im;
    } else if (i == 2) {
      return re_err;
    } else if (i == 3) {
      return im_err;
    } else {
      cout << "Error: MComplex::operator() (int i) - i must be 0, 1, 2, or 3."
           << endl;
      return 0;
    }
  }
  MComplex operator*(MComplex c) {
    return MComplex(re * c.Re() - im * c.Im(), re * c.Im() + im * c.Re(),
                    sqrt(pow(c.Re() * re_err, 2) + pow(c.Im() * im_err, 2) +
                         pow(re * c.ReErr(), 2) + pow(im * c.ImErr(), 2)),
                    sqrt(pow(c.Re() * im_err, 2) + pow(c.Im() * re_err, 2) +
                         pow(im * c.ReErr(), 2) + pow(re * c.ImErr(), 2)));
  }
  MComplex operator*(double c) {
    return MComplex(re * c, im * c, abs(re_err * c), abs(im_err * c));
  }
  MComplex operator/(MComplex c) {
    double err_re1 = re_err * c.Re() / c.Mag2();
    double err_re2 =
        c.ReErr() * re * (pow(c.Im(), 2) - pow(c.Re(), 2)) / c.Mag2();
    double err_re3 = im_err * c.Im() / c.Mag2();
    double err_re4 =
        c.ImErr() * im * (pow(c.Re(), 2) - pow(c.Im(), 2)) / c.Mag2();
    double err_re = sqrt(pow(err_re1, 2) + pow(err_re2, 2) + pow(err_re3, 2) +
                         pow(err_re4, 2));
    double err_im1 = im_err * c.Re() / c.Mag2();
    double err_im2 =
        c.ReErr() * im * (pow(c.Im(), 2) - pow(c.Re(), 2)) / c.Mag2();
    double err_im3 = re_err * c.Im() / c.Mag2();
    double err_im4 =
        c.ImErr() * re * (pow(c.Re(), 2) - pow(c.Im(), 2)) / c.Mag2();
    double err_im = sqrt(pow(err_im1, 2) + pow(err_im2, 2) + pow(err_im3, 2) +
                         pow(err_im4, 2));

    return MComplex(
        (re * c.Re() + im * c.Im()) / (c.Re() * c.Re() + c.Im() * c.Im()),
        (im * c.Re() - re * c.Im()) / (c.Re() * c.Re() + c.Im() * c.Im()),
        err_re, err_im);
  }
  MComplex operator/(double c) {
    return MComplex(re / c, im / c, abs(re_err / c), abs(im_err / c));
  }
  MComplex operator+(MComplex c) {
    return MComplex(re + c.Re(), im + c.Im(),
                    sqrt(re_err * re_err + c.ReErr() * c.ReErr()),
                    sqrt(im_err * im_err + c.ImErr() * c.ImErr()));
  }
  MComplex operator-(MComplex c) {
    return MComplex(re - c.Re(), im - c.Im(),
                    sqrt(re_err * re_err + c.ReErr() * c.ReErr()),
                    sqrt(im_err * im_err + c.ImErr() * c.ImErr()));
  }

  MComplex &operator*=(MComplex c) {
    double re_temp = re;
    re = re * c.Re() - im * c.Im();
    im = re_temp * c.Im() + im * c.Re();
    re_err = sqrt(pow(c.Re() * re_err, 2) + pow(c.Im() * im_err, 2) +
                  pow(re_temp * c.ReErr(), 2) + pow(im * c.ImErr(), 2));
    im_err = sqrt(pow(c.Re() * im_err, 2) + pow(c.Im() * re_err, 2) +
                  pow(im * c.ReErr(), 2) + pow(re_temp * c.ImErr(), 2));
    return *this;
  }
  MComplex &operator*=(double c) {
    re *= c;
    im *= c;
    re_err = abs(re_err * c);
    im_err = abs(im_err * c);
    return *this;
  }
  MComplex &operator/=(MComplex c) {
    double err_re1 = re_err * c.Re() / c.Mag2();
    double err_re2 =
        c.ReErr() * re * (pow(c.Im(), 2) - pow(c.Re(), 2)) / c.Mag2();
    double err_re3 = im_err * c.Im() / c.Mag2();
    double err_re4 =
        c.ImErr() * im * (pow(c.Re(), 2) - pow(c.Im(), 2)) / c.Mag2();
    re_err = sqrt(pow(err_re1, 2) + pow(err_re2, 2) + pow(err_re3, 2) +
                  pow(err_re4, 2));
    double err_im1 = im_err * c.Re() / c.Mag2();
    double err_im2 =
        c.ReErr() * im * (pow(c.Im(), 2) - pow(c.Re(), 2)) / c.Mag2();
    double err_im3 = re_err * c.Im() / c.Mag2();
    double err_im4 =
        c.ImErr() * re * (pow(c.Re(), 2) - pow(c.Im(), 2)) / c.Mag2();
    im_err = sqrt(pow(err_im1, 2) + pow(err_im2, 2) + pow(err_im3, 2) +
                  pow(err_im4, 2));

    double re_temp = re;
    re = (re * c.Re() + im * c.Im()) / (c.Re() * c.Re() + c.Im() * c.Im());
    im = (im * c.Re() - re_temp * c.Im()) / (c.Re() * c.Re() + c.Im() * c.Im());
    return *this;
  }
  MComplex &operator/=(double c) {
    re /= c;
    im /= c;
    re_err = abs(re_err / c);
    im_err = abs(im_err / c);
    return *this;
  }
  MComplex &operator+=(MComplex c) {
    re += c.Re();
    im += c.Im();
    re_err = sqrt(re_err * re_err + c.ReErr() * c.ReErr());
    im_err = sqrt(im_err * im_err + c.ImErr() * c.ImErr());
    return *this;
  }
  MComplex &operator-=(MComplex c) {
    re -= c.Re();
    im -= c.Im();
    re_err = sqrt(re_err * re_err + c.ReErr() * c.ReErr());
    im_err = sqrt(im_err * im_err + c.ImErr() * c.ImErr());
    return *this;
  }

private:
  double re;
  double im;
  double re_err;
  double im_err;
};

MComplex GetMComplexFromHist(TH1 *hist_re, TH1 *hist_im, int i_bin) {
  double re = hist_re->GetBinContent(i_bin);
  double im = hist_im->GetBinContent(i_bin);
  double re_err = hist_re->GetBinError(i_bin);
  double im_err = hist_im->GetBinError(i_bin);
  return MComplex(re, im, re_err, im_err);
}

double GetMeanFormVecDouble(std::vector<double> vec) {
  double sum = 0;
  for (int i = 0; i < vec.size(); i++) {
    sum += vec[i];
  }
  return sum / vec.size();
}

double GetStdDevFormVecDouble(std::vector<double> vec) {
  double mean = GetMeanFormVecDouble(vec);
  double sum = 0;
  for (int i = 0; i < vec.size(); i++) {
    sum += pow(vec[i] - mean, 2);
  }
  return sqrt(sum / vec.size() / (vec.size() - 1));
}

#endif