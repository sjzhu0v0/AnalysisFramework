#define MRDF
#include "MHead.h"
#include "MHist.h"
#include "MRootIO.h"
#include <ROOT/RDataFrame.hxx>

// DECLARE_SOA_TABLE_STAGED(ReducedEvents, "REDUCEDEVENT", //!   Main event information table
//                          o2::soa::Index<>,
//                          reducedevent::Tag, bc::RunNumber,
//                          collision::PosX, collision::PosY, collision::PosZ, collision::NumContrib,
//                          collision::CollisionTime, collision::CollisionTimeRes);

// DECLARE_SOA_TABLE(ReducedEventsExtended, "AOD", "REEXTENDED", //!  Extended event information
//                   bc::GlobalBC, evsel::Alias, evsel::Selection, timestamp::Timestamp, cent::CentRun2V0M,
//                   mult::MultTPC, mult::MultFV0A, mult::MultFV0C, mult::MultFT0A, mult::MultFT0C,
//                   mult::MultFDDA, mult::MultFDDC, mult::MultZNA, mult::MultZNC, mult::MultTracklets, mult::MultNTracksPV,
//                   cent::CentFT0C);