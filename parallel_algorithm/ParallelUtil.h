#ifndef DOF_PARALLEL_PARALLEL_UTIL_H_
#define DOF_PARALLEL_PARALLEL_UTIL_H_

namespace dof {
namespace parallel {

template<typename TInt>
void GetPartionArgs(TInt totalLength, TInt myRank, TInt partsCount, TInt& myStart, TInt& myLength) {
    TInt groupSize = (TInt)(totalLength / partsCount);
    TInt remain = (TInt)(totalLength % partsCount);
    myStart = (myRank < remain) ? (groupSize * myRank + myRank) : (groupSize * myRank + remain);
    myLength = (myRank < remain) ? groupSize + 1 : groupSize;
}

} // namespace parallel
} // namespace dof

#endif //DOF_PARALLEL_PARALLEL_UTIL_H_
