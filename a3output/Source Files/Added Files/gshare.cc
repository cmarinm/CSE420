/*
Author: Cesar Marin
Fu w me u know igotit
CSE 420 ghsare predictor

*/

#include "cpu/pred/gshare.hh"
#include "base/bitfield.hh"
#include "base/intmath.hh"
#include "base/logging.hh"
#include "base/trace.hh"
#include "debug/Fetch.hh"
#include <math.h>
// DONE
GsharePred::GsharePred(const GsharePredParams *params)
    : BPredUnit(params), localCtrEntries(params->localPredictorSize),
      globalCtrBits(params->globalPredictorSize) {
  localReg = new unsigned int[localCtrEntries];
  globalReg = new unsigned int[globalCtrBits];
}
// DONE
void GsharePred::uncondBranch(ThreadID tid, Addr pc, void *&bp_history) {
  int result;
  int localindex;
  localindex = hashtolocal(pc);
  result = localReg[localindex];
  // Create BPHistory and pass it back to be recorded
  BPHistory *history = new BPHistory;
  history->globalHistory = globalReg;
  history->localHistoryIdx = localindex;
  history->localHistory = result;
  bp_history = (void *)history;
  globalupdate(true);
  // update global history as taken
}
// DONE
void GsharePred::btbUpdate(ThreadID tid, Addr branch_addr, void *&bp_history) {
  // Update Global History to Not Taken (clear LSB)
  globalupdate(false);
  // Update Local History to Not Taken
  int localindex = hashtolocal(branch_addr);
  updatelocal(false, localindex);
}
// DONE
void GsharePred::updatelocal(bool taken, int localindex) {
  int result = localReg[localindex];
  switch (result) {
  case 3:
    if (!taken)
      localReg[localindex] = 2;
    break;
  case 2:
    if (taken)
      localReg[localindex] = 3;
    else
      localReg[localindex] = 0;
    break;
  case 1:
    if (taken)
      localReg[localindex] = 3;
    else
      localReg[localindex] = 0;
    break;
  case 0:
    if (taken)
      localReg[localindex] = 1;
    break;
  }
}
// DONE
bool GsharePred::lookup(ThreadID tid, Addr branch_addr, void *&bp_history) {
  int localindex = hashtolocal(branch_addr);
  int result = localReg[localindex];
  bool pred;
  if (result > 1) {
    pred = true;
  } else {
    pred = false;
  }

  // Create BPHistory and pass it back to be recorded.
  BPHistory *history = new BPHistory;
  history->globalHistory = globalReg;
  history->localHistoryIdx = localindex;
  history->localHistory = result;
  bp_history = (void *)history;

  return pred;
}
// DONE
void GsharePred::squash(ThreadID tid, void *bp_history) {
  BPHistory *history = static_cast<BPHistory *>(bp_history);

  // Restore global history to state prior to this branch.
  globalReg = history->globalHistory;
  // restore local history
  int localindex = history->localHistoryIdx;
  int value = history->localHistory;
  localReg[localindex] = value;

  delete history;
}
// DONE
void GsharePred::update(ThreadID tid, Addr branch_addr, bool taken,
                        void *bp_history, bool squashed) {

  BPHistory *history = static_cast<BPHistory *>(bp_history);
  int localindex = hashtolocal(branch_addr);

  // If this is a misprediction, restore the speculatively
  // updated state (global history register and local history)
  // and update again.
  if (squashed) {
    // restore global
    globalReg = history->globalHistory;
    // restore local
    localindex = history->localHistoryIdx;
    int value = history->localHistory;
    localReg[localindex] = value;
    // update globalReg
    globalupdate(taken);
    // update local
    updatelocal(taken, localindex);
    return;
  }

  // update both global and local
  globalupdate(taken);
  updatelocal(taken, localindex);
  delete history;
}
// DONE
int GsharePred::hashtolocal(Addr pc) {
  // turn global into into int
  int global = 0;
  for (int i = 0; i < globalCtrBits; i++) {
    if (globalReg[i] == 1)
      global += (int)pow(2, i);
  }
  // xor this new int with pc
  int index = global ^ pc;
  unsigned int hashval = (unsigned)index%localCtrEntries;
  index = (int)hashval;
  // if using 16 bits divide by 4. because 16 bits does not match 16384
  
  return index;
}
// DONE
void GsharePred::globalupdate(bool taken) {
  // shift in the new T/NT into global history
  int new_data;
  int i;
  if (taken)
    new_data = 1;
  else {
    new_data = 0;
  }

  for (i = 0; i < (globalCtrBits - 1); i++) {
    localReg[i] = localReg[i + 1];
  }
  localReg[i] = new_data;
}

unsigned GsharePred::getGHR(ThreadID tid, void *bp_history) const {
  unsigned int global = 0;
  for (int i = 0; i < globalCtrBits; i++) {
    if (globalReg[i] == 1)
      global += (int)pow(2, i);
  }
  return global;
}

GsharePred *GsharePredParams::create() { return new GsharePred(this); }
#ifdef DEBUG
int GsharePred::BPHistory::newCount = 0;
#endif
