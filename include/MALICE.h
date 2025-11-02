#ifndef MALICE_H
#define MALICE_H
#include "MRootIO.h"
#include "TMath.h"
#include "vector"
#include <ROOT/RDataFrame.hxx>

using std::vector;

namespace MALICE {
enum EventSelectionFlags {
  kIsBBV0A = 0, // cell-averaged time in V0A in beam-beam window
  kIsBBV0C, // cell-averaged time in V0C in beam-beam window (for Run 2 only)
  kIsBBFDA, // cell-averaged time in FDA (or AD in Run2) in beam-beam window
  kIsBBFDC, // cell-averaged time in FDC (or AD in Run2) in beam-beam window
  kIsBBT0A, // cell-averaged time in T0A in beam-beam window
  kIsBBT0C, // cell-averaged time in T0C in beam-beam window
  kNoBGV0A, // cell-averaged time in V0A in beam-gas window
  kNoBGV0C, // cell-averaged time in V0C in beam-gas window (for Run 2 only)
  kNoBGFDA, // cell-averaged time in FDA (AD in Run2) in beam-gas window
  kNoBGFDC, // cell-averaged time in FDC (AD in Run2) in beam-gas window
  kNoBGT0A, // cell-averaged time in T0A in beam-gas window
  kNoBGT0C, // cell-averaged time in T0C in beam-gas window
  kIsBBZNA, // time in common ZNA channel in beam-beam window
  kIsBBZNC, // time in common ZNC channel in beam-beam window
  kIsBBZAC, // time in ZNA and ZNC in beam-beam window - circular cut in ZNA-ZNC
            // plane
  kNoBGZNA, // time in common ZNA channel is outside of beam-gas window
  kNoBGZNC, // time in common ZNC channel is outside of beam-gas window
  kNoV0MOnVsOfPileup, // no out-of-bunch pileup according to online-vs-offline
                      // VOM correlation
  kNoSPDOnVsOfPileup, // no out-of-bunch pileup according to online-vs-offline
                      // SPD correlation
  kNoV0Casymmetry, // no beam-gas according to correlation of V0C multiplicities
                   // in V0C3 and V0C012
  kIsGoodTimeRange,  // good time range
  kNoIncompleteDAQ,  // complete event according to DAQ flags
  kNoTPCLaserWarmUp, // no TPC laser warm-up event (used in Run 1)
  kNoTPCHVdip,       // no TPC HV dip
  kNoPileupFromSPD,  // no pileup according to SPD vertexer
  kNoV0PFPileup,     // no out-of-bunch pileup according to V0 past-future info
  kNoSPDClsVsTklBG,  // no beam-gas according to cluster-vs-tracklet correlation
  kNoV0C012vsTklBG,  // no beam-gas according to V0C012-vs-tracklet correlation
  kNoInconsistentVtx,  // no inconsistency in SPD and Track vertices
  kNoPileupInMultBins, // no pileup according to multiplicity-differential
                       // pileup checks
  kNoPileupMV,         // no pileup according to multi-vertexer
  kNoPileupTPC,        // no pileup in TPC
  kIsTriggerTVX, // FT0 vertex (acceptable FT0C-FT0A time difference) at trigger
                 // level
  kIsINT1,       // SPDGFO >= 1 || V0A || V0C
  kNoITSROFrameBorder, // bunch crossing is far from ITS RO Frame border
  kNoTimeFrameBorder,  // bunch crossing is far from Time Frame borders
  kNoSameBunchPileup,  // reject collisions in case of pileup with another
                       // collision in the same foundBC
  kIsGoodZvtxFT0vsPV,  // small difference between z-vertex from PV and from FT0
  kIsVertexITSTPC,     // at least one ITS-TPC track (reject vertices built from
                       // ITS-only tracks)
  kIsVertexTOFmatched, // at least one of vertex contributors is matched to TOF
  kIsVertexTRDmatched, // at least one of vertex contributors is matched to TRD
  kNoCollInTimeRangeNarrow,   // no other collisions in specified time range
                              // (narrower than Strict)
  kNoCollInTimeRangeStrict,   // no other collisions in specified time range
  kNoCollInTimeRangeStandard, // no other collisions in specified time range
                              // with per-collision multiplicity above threshold
  kNoCollInRofStrict,         // no other collisions in this Readout Frame
  kNoCollInRofStandard,       // no other collisions in this Readout Frame with
                              // per-collision multiplicity above threshold
  kNoHighMultCollInPrevRof,   // veto an event if FT0C amplitude in previous ITS
                              // ROF is above threshold
  kIsGoodITSLayer3, // number of inactive chips on ITS layer 3 is below maximum
                    // allowed value
  kIsGoodITSLayer0123, // numbers of inactive chips on ITS layers 0-3 are below
                       // maximum allowed values
  kIsGoodITSLayersAll, // numbers of inactive chips on all ITS layers are below
                       // maximum allowed values
  kNsel                // counter
};

TString name_eventselection[EventSelectionFlags::kNsel] = {
    "kIsBBV0A",
    "kIsBBV0C",
    "kIsBBFDA",
    "kIsBBFDC",
    "kIsBBT0A",
    "kIsBBT0C",
    "kNoBGV0A",
    "kNoBGV0C",
    "kNoBGFDA",
    "kNoBGFDC",
    "kNoBGT0A",
    "kNoBGT0C",
    "kIsBBZNA",
    "kIsBBZNC",
    "kIsBBZAC",
    "kNoBGZNA",
    "kNoBGZNC",
    "kNoV0MOnVsOfPileup",
    "kNoSPDOnVsOfPileup",
    "kNoV0Casymmetry",
    "kIsGoodTimeRange",
    "kNoIncompleteDAQ",
    "kNoTPCLaserWarmUp",
    "kNoTPCHVdip",
    "kNoPileupFromSPD",
    "kNoV0PFPileup",
    "kNoSPDClsVsTklBG",
    "kNoV0C012vsTklBG",
    "kNoInconsistentVtx",
    "kNoPileupInMultBins",
    "kNoPileupMV",
    "kNoPileupTPC",
    "kIsTriggerTVX",
    "kIsINT1",
    "kNoITSROFrameBorder",
    "kNoTimeFrameBorder",
    "kNoSameBunchPileup",
    "kIsGoodZvtxFT0vsPV",
    "kIsVertexITSTPC",
    "kIsVertexTOFmatched",
    "kIsVertexTRDmatched",
    "kNoCollInTimeRangeNarrow",
    "kNoCollInTimeRangeStrict",
    "kNoCollInTimeRangeStandard",
    "kNoCollInRofStrict",
    "kNoCollInRofStandard",
    "kNoHighMultCollInPrevRof",
    "kIsGoodITSLayer3",
    "kIsGoodITSLayer0123",
    "kIsGoodITSLayersAll"}; // counter

bool doEventHaveTrigger(const Long64_t &eventSelectionMask, const int &flag) {
  return eventSelectionMask & (1 << flag);
}

vector<double> triggermap(Long64_t fSelection) {
  // EventSelectionFlags::kNsel
  vector<double> trigger_map;
  for (int i = 0; i < EventSelectionFlags::kNsel; i++) {
    if ((fSelection >> i) & 1)
      trigger_map.push_back(i);
  }
  // if (trigger_map.size() == 0)
  //   trigger_map.push_back(-1);
  return trigger_map;
}

ROOT::RVec<double> triggermapRVec(Long64_t fSelection) {
  // EventSelectionFlags::kNsel
  ROOT::RVec<double> trigger_map;
  for (int i = 0; i < EventSelectionFlags::kNsel; i++) {
    if ((fSelection >> i) & 1)
      trigger_map.push_back(i);
  }
  trigger_map.push_back(kNsel);
  // if (trigger_map.size() == 0)
  //   trigger_map.push_back(-1);
  return trigger_map;
}

bool IsntSPDPileup(Long64_t fSelection) {
  return (fSelection >> EventSelectionFlags::kNoPileupFromSPD) & 1;
}

bool IsntTPCPileup(Long64_t fSelection) {
  return (fSelection >> EventSelectionFlags::kNoPileupTPC) & 1;
}

bool IsntSameBunchPileup_NoSlot(Long64_t fSelection) {
  return (fSelection >> EventSelectionFlags::kNoSameBunchPileup) & 1;
}
bool IsntSameBunchPileup(unsigned int, Long64_t fSelection) {
  return (fSelection >> EventSelectionFlags::kNoSameBunchPileup) & 1;
}

// kNoITSROFrameBorder
bool IsntITSROFrameBorder(Long64_t fSelection) {
  return (fSelection >> EventSelectionFlags::kNoITSROFrameBorder) & 1;
}

// kNoTimeFrameBorder
bool IsntTimeFrameBorder(Long64_t fSelection) {
  return (fSelection >> EventSelectionFlags::kNoTimeFrameBorder) & 1;
}

// kIsTriggerTVX
bool IsTriggerTVX(Long64_t fSelection) {
  return (fSelection >> EventSelectionFlags::kIsTriggerTVX) & 1;
}

TH1D *h1_runInfo = nullptr;

double EventNumberMinbias(
    int run,
    TString path_file = "/home/szhu/work/alice/analysis/InfoRun/"
                        "runInfo22_DQ.root:bc-selection-task/hCounterTVX") {
  if (h1_runInfo == nullptr) {
    h1_runInfo = MRootIO::GetTH1D(path_file);
  }
  for (int i = 1; i <= h1_runInfo->GetNbinsX(); i++) {
    TString name_run = h1_runInfo->GetXaxis()->GetBinLabel(i);
    if (name_run.Atoi() == run) {
      return h1_runInfo->GetBinContent(i);
    }
  }
  return -1;
}
}; // namespace MALICE

namespace MALICE_RUN {
map<int, int> map_run22 = {
    {0, 523142},   {1, 523148},   {2, 523182},   {3, 523186},   {4, 523298},
    {5, 523306},   {6, 523308},   {7, 523309},   {8, 523397},   {9, 523399},
    {10, 523401},  {11, 523441},  {12, 523541},  {13, 523559},  {14, 523669},
    {15, 523671},  {16, 523677},  {17, 523728},  {18, 523731},  {19, 523779},
    {20, 523783},  {21, 523786},  {22, 523788},  {23, 523789},  {24, 523792},
    {25, 523797},  {26, 523821},  {27, 526463},  {28, 526465},  {29, 526466},
    {30, 526467},  {31, 526468},  {32, 526486},  {33, 526505},  {34, 526512},
    {35, 526525},  {36, 526526},  {37, 526528},  {38, 526559},  {39, 526596},
    {40, 526606},  {41, 526612},  {42, 526639},  {43, 526641},  {44, 526643},
    {45, 526647},  {46, 526649},  {47, 526713},  {48, 526714},  {49, 526715},
    {50, 526716},  {51, 526719},  {52, 526720},  {53, 526776},  {54, 526860},
    {55, 526865},  {56, 526886},  {57, 526938},  {58, 526963},  {59, 526964},
    {60, 526966},  {61, 526967},  {62, 526968},  {63, 527015},  {64, 527016},
    {65, 527028},  {66, 527031},  {67, 527033},  {68, 527034},  {69, 527038},
    {70, 527039},  {71, 527041},  {72, 527057},  {73, 527076},  {74, 527108},
    {75, 527109},  {76, 527228},  {77, 527237},  {78, 527240},  {79, 527259},
    {80, 527260},  {81, 527261},  {82, 527262},  {83, 527345},  {84, 527347},
    {85, 527349},  {86, 527446},  {87, 527518},  {88, 527523},  {89, 527690},
    {90, 527694},  {91, 527731},  {92, 527734},  {93, 527736},  {94, 527821},
    {95, 527825},  {96, 527826},  {97, 527828},  {98, 527848},  {99, 527850},
    {100, 527852}, {101, 527863}, {102, 527864}, {103, 527865}, {104, 527869},
    {105, 527871}, {106, 527895}, {107, 527898}, {108, 527899}, {109, 527902},
    {110, 527963}, {111, 527976}, {112, 527978}, {113, 527979}, {114, 528021},
    {115, 528026}, {116, 528036}, {117, 528093}, {118, 528094}, {119, 528097},
    {120, 528105}, {121, 528107}, {122, 528109}, {123, 528110}, {124, 528231},
    {125, 528232}, {126, 528233}, {127, 528263}, {128, 528266}, {129, 528292},
    {130, 528294}, {131, 528316}, {132, 528319}, {133, 528328}, {134, 528329},
    {135, 528330}, {136, 528332}, {137, 528336}, {138, 528347}, {139, 528359},
    {140, 528379}, {141, 528381}, {142, 528386}, {143, 528448}, {144, 528451},
    {145, 528461}, {146, 528463}, {147, 528529}, {148, 528530}, {149, 528531},
    {150, 528534}, {151, 528537}, {152, 528543}, {153, 528602}, {154, 528604},
    {155, 528617}, {156, 528781}, {157, 528782}, {158, 528783}, {159, 528784},
    {160, 528798}, {161, 528801}, {162, 529077}, {163, 529078}, {164, 529084},
    {165, 529088}, {166, 529115}, {167, 529116}, {168, 529117}, {169, 529128},
    {170, 529129}, {171, 529208}, {172, 529209}, {173, 529210}, {174, 529211},
    {175, 529235}, {176, 529237}, {177, 529242}, {178, 529248}, {179, 529252},
    {180, 529270}, {181, 529306}, {182, 529317}, {183, 529320}, {184, 529324},
    {185, 529338}, {186, 529341}, {187, 529450}, {188, 529452}, {189, 529454},
    {190, 529458}, {191, 529460}, {192, 529461}, {193, 529462}, {194, 529542},
    {195, 529552}, {196, 529554}, {197, 529662}, {198, 529663}, {199, 529664},
    {200, 529674}, {201, 529675}, {202, 529690}, {203, 529691}};

map<int, int> map_run24 = {
    {0, 550367},   {1, 550369},   {2, 550375},   {3, 550412},   {4, 550417},
    {5, 550421},   {6, 550424},   {7, 550425},   {8, 550439},   {9, 550630},
    {10, 550632},  {11, 550634},  {12, 550653},  {13, 550654},  {14, 550690},
    {15, 550691},  {16, 550707},  {17, 550711},  {18, 550728},  {19, 550731},
    {20, 550742},  {21, 550756},  {22, 550760},  {23, 550774},  {24, 550778},
    {25, 550781},  {26, 550784},  {27, 550819},  {28, 550824},  {29, 550843},
    {30, 550848},  {31, 550852},  {32, 550858},  {33, 550889},  {34, 550916},
    {35, 550997},  {36, 551005},  {37, 551007},  {38, 551008},  {39, 551013},
    {40, 551023},  {41, 551027},  {42, 551066},  {43, 551083},  {44, 551105},
    {45, 551107},  {46, 551122},  {47, 551127},  {48, 551149},  {49, 551219},
    {50, 551221},  {51, 551229},  {52, 551230},  {53, 551232},  {54, 551255},
    {55, 551257},  {56, 551260},  {57, 551272},  {58, 551290},  {59, 551296},
    {60, 551365},  {61, 551387},  {62, 551389},  {63, 551391},  {64, 551392},
    {65, 551394},  {66, 551398},  {67, 551418},  {68, 551427},  {69, 551463},
    {70, 551468},  {71, 551498},  {72, 551504},  {73, 551759},  {74, 551760},
    {75, 551761},  {76, 551780},  {77, 551843},  {78, 551856},  {79, 551874},
    {80, 551875},  {81, 551877},  {82, 551889},  {83, 551890},  {84, 551894},
    {85, 551921},  {86, 551922},  {87, 551923},  {88, 551924},  {89, 551925},
    {90, 551926},  {91, 551931},  {92, 551943},  {93, 551958},  {94, 551977},
    {95, 551979},  {96, 551982},  {97, 551983},  {98, 551989},  {99, 551992},
    {100, 551993}, {101, 551997}, {102, 552005}, {103, 552029}, {104, 552080},
    {105, 552102}, {106, 552103}, {107, 552138}, {108, 552139}, {109, 552140},
    {110, 552141}, {111, 552142}, {112, 552156}, {113, 552176}, {114, 552177},
    {115, 552178}, {116, 552179}, {117, 552197}, {118, 552198}, {119, 552200},
    {120, 552201}, {121, 552203}, {122, 552204}, {123, 552205}, {124, 552206},
    {125, 552283}, {126, 552285}, {127, 552304}, {128, 552340}, {129, 552341},
    {130, 552353}, {131, 552369}, {132, 552381}, {133, 552382}, {134, 552383},
    {135, 552384}, {136, 552400}, {137, 552401}, {138, 552402}, {139, 552403},
    {140, 553185}, {141, 553187}, {142, 553188}, {143, 553189}, {144, 553193},
    {145, 553219}, {146, 553225}, {147, 553250}, {148, 553253}, {149, 553255},
    {150, 553274}, {151, 553294}, {152, 553297}, {153, 553299}, {154, 553305},
    {155, 553486}, {156, 553512}, {157, 553530}, {158, 553536}, {159, 553555},
    {160, 553588}, {161, 553590}, {162, 553610}, {163, 553633}, {164, 553655},
    {165, 553660}, {166, 553663}, {167, 553700}, {168, 553702}, {169, 553739},
    {170, 553756}, {171, 553785}, {172, 553807}, {173, 553816}, {174, 553819},
    {175, 553821}, {176, 553824}, {177, 553825}, {178, 553844}, {179, 553862},
    {180, 553880}, {181, 553903}, {182, 554092}, {183, 554094}, {184, 554095},
    {185, 554097}, {186, 554098}, {187, 554194}, {188, 554198}, {189, 554201},
    {190, 554203}, {191, 554207}, {192, 554208}, {193, 554223}, {194, 554247},
    {195, 554261}, {196, 554293}, {197, 554295}, {198, 554316}, {199, 554322},
    {200, 554323}, {201, 554354}, {202, 554394}, {203, 554404}, {204, 554408},
    {205, 554413}, {206, 554427}, {207, 554462}, {208, 554471}, {209, 554494},
    {210, 554495}, {211, 554504}, {212, 554507}, {213, 554524}, {214, 554526},
    {215, 554538}, {216, 554558}, {217, 554564}, {218, 554569}, {219, 554588},
    {220, 554603}, {221, 554613}, {222, 554615}, {223, 554633}, {224, 554701},
    {225, 554703}, {226, 554714}, {227, 554728}, {228, 554732}, {229, 554736},
    {230, 554752}, {231, 554768}, {232, 554772}, {233, 554774}, {234, 554791},
    {235, 554808}, {236, 554835}, {237, 554837}, {238, 554873}, {239, 554880},
    {240, 554898}, {241, 554920}, {242, 554970}, {243, 554973}, {244, 554998},
    {245, 555020}, {246, 555022}, {247, 555023}, {248, 555047}, {249, 555071},
    {250, 555073}, {251, 555121}, {252, 555122}, {253, 555124}, {254, 555150},
    {255, 555152}, {256, 555156}, {257, 555160}, {258, 555166}, {259, 555172},
    {260, 555187}, {261, 555202}, {262, 555208}, {263, 555226}, {264, 555232},
    {265, 555254}, {266, 555259}, {267, 555267}, {268, 555270}, {269, 555308},
    {270, 555344}, {271, 555345}, {272, 555370}, {273, 555374}, {274, 555401},
    {275, 555408}, {276, 555411}, {277, 555431}, {278, 555435}, {279, 555443},
    {280, 555451}, {281, 555476}, {282, 555478}, {283, 555482}, {284, 555504},
    {285, 555540}, {286, 555543}, {287, 555546}, {288, 555575}, {289, 555591},
    {290, 555596}, {291, 555612}, {292, 555649}, {293, 555651}, {294, 555676},
    {295, 555693}, {296, 555695}, {297, 555705}, {298, 555707}, {299, 555722},
    {300, 555723}, {301, 555740}, {302, 555742}, {303, 555759}, {304, 555761},
    {305, 555763}, {306, 555790}, {307, 555798}, {308, 555801}, {309, 555850},
    {310, 555853}, {311, 555860}, {312, 555881}, {313, 555883}, {314, 555900},
    {315, 555917}, {316, 555933}, {317, 555958}, {318, 555960}, {319, 555965},
    {320, 555967}, {321, 555976}, {322, 556152}, {323, 556160}, {324, 556164},
    {325, 556182}, {326, 556210}, {327, 556218}, {328, 556237}, {329, 556248},
    {330, 556269}, {331, 556284}, {332, 556370}, {333, 556372}, {334, 556412},
    {335, 556437}, {336, 556454}, {337, 556461}, {338, 556482}, {339, 556485},
    {340, 556491}, {341, 556497}, {342, 556517}, {343, 556542}, {344, 556562},
    {345, 556639}, {346, 556640}, {347, 556641}, {348, 556662}, {349, 556664},
    {350, 556680}, {351, 556716}, {352, 556734}, {353, 556741}, {354, 556767},
    {355, 556816}, {356, 556834}, {357, 556872}, {358, 556889}, {359, 556907},
    {360, 556909}, {361, 556913}, {362, 556923}, {363, 556939}, {364, 556954},
    {365, 556958}, {366, 556979}, {367, 556981}, {368, 556997}, {369, 557012},
    {370, 557019}, {371, 557021}, {372, 557026}, {373, 557074}, {374, 557104},
    {375, 557112}, {376, 557118}, {377, 557119}, {378, 557138}, {379, 557149},
    {380, 557226}, {381, 557233}, {382, 557251}, {383, 557271}, {384, 557291},
    {385, 557299}, {386, 557321}, {387, 557336}, {388, 557339}, {389, 557350},
    {390, 557374}, {391, 557415}, {392, 557425}, {393, 557444}, {394, 557481},
    {395, 557482}, {396, 557509}, {397, 557531}, {398, 557547}, {399, 557613},
    {400, 557659}, {401, 557681}, {402, 557691}, {403, 557717}, {404, 557723},
    {405, 557726}, {406, 557744}, {407, 557749}, {408, 557862}, {409, 557876},
    {410, 557897}, {411, 557913}, {412, 557926}, {413, 558122}, {414, 558124},
    {415, 558126}, {416, 558150}, {417, 558153}, {418, 558155}, {419, 558179},
    {420, 558182}, {421, 558215}, {422, 558217}, {423, 558221}, {424, 558244},
    {425, 558247}, {426, 558273}, {427, 558275}, {428, 558284}, {429, 558288},
    {430, 558291}, {431, 558327}, {432, 558329}, {433, 558330}, {434, 558354},
    {435, 558369}, {436, 558383}, {437, 558387}, {438, 558390}, {439, 558406},
    {440, 558409}, {441, 558410}, {442, 558422}, {443, 558433}, {444, 558437},
    {445, 558449}, {446, 558482}, {447, 558535}, {448, 558551}, {449, 558602},
    {450, 558604}, {451, 558606}, {452, 558615}, {453, 558627}, {454, 558633},
    {455, 558656}, {456, 558676}, {457, 558685}, {458, 558726}, {459, 558744},
    {460, 558750}, {461, 558752}, {462, 558757}};
} // namespace MALICE_RUN

#endif // MALICE_H