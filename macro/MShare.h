#ifndef MShare_h
#define MShare_h

#include "MHist.h"

/* #region axis definition */
StrVar4Hist var_DeltaEta("DeltaEta", "#Delta #eta", "rapidity", 20, {-4., 4.});
StrVar4Hist var_DeltaPhi("DeltaPhi", "#Delta #phi", "rad", 30,
                         {-M_PI_2, M_PI + M_PI_2});
StrVar4Hist var_VtxZ("fVtxZ", "V_{Z}", "cm", 8, {-10, 10});
StrVar4Hist var_Mass("fMass", "Mass", "GeV/c^{2}", 100, {1., 5.});
StrVar4Hist var_Pt("fPT", "p_{T}", "GeV/c", 10, {0., 10.});
/* #endregion */
vector<StrVar4Hist> vec_vars = {var_DeltaEta, var_DeltaPhi, var_VtxZ};

#endif
