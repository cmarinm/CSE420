/*
Author: Cesar Marin

*/

#ifndef __CPU_PRED_GSHARE_PRED_HH__
#define __CPU_PRED_GSHARE_PRED_HH__

#include <vector>

#include "base/types.hh"
#include "cpu/pred/bpred_unit.hh"
#include "cpu/pred/sat_counter.hh"
#include "params/GsharePred.hh"

class GsharePred : public BPredUnit {
public:
  GsharePred(const GsharePredParams *params);
  void uncondBranch(ThreadID tid, Addr pc, void *&bp_history);
  bool lookup(ThreadID tid, Addr branch_addr, void *&bp_history);
  void btbUpdate(ThreadID tid, Addr branch_addr, void *&bp_history);
  void update(ThreadID tid, Addr branch_addr, bool taken, void *bp_history,
              bool squashed);
  void squash(ThreadID tid, void *bp_history);
  int hashtolocal(Addr pc);
  void globalupdate(bool taken);
  void updatelocal(bool taken, int localindex);
  unsigned getGHR(ThreadID tid, void *bp_history) const;

private:
  struct BPHistory {
#ifdef DEBUG
    BPHistory() { newCount++; }
    ~BPHistory() { newCount--; }

    static int newCount;
#endif
    unsigned *globalHistory;
    unsigned localHistoryIdx;
    unsigned localHistory;
    bool localPredTaken;
    bool globalPredTaken;
    bool globalUsed;
  };
  unsigned localCtrEntries; // number of entries in local reg
  unsigned globalCtrBits;   // number of bits in global reg
  unsigned int *localReg;  // local 2bit register,value will be 0-3, representing the 4
                  // states
  unsigned int *globalReg; // global shift register, values will be 0-1 shift,
};
#endif // __CPU_PRED_GSHARE_PRED_HH__
