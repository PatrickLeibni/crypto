#include "circom.hpp"
#include "calcwit.hpp"
#define NSignals 23
#define NComponents 2
#define NOutputs 0
#define NInputs 2
#define NVars 3
#define NPublic 2
#define __P__ "21888242871839275222246405745257275088548364400416034343698204186575808495617"

/*
Poseidon2Circuit
*/
void Poseidon2Circuit_f36fc399b1f610dd(Circom_CalcWit *ctx, int __cIdx) {
    FrElement _sigValue[1];
    FrElement _sigValue_1[1];
    int _compIdx;
    int _hash_sigIdx_;
    int _hash_sigIdx__1;
    int _compIdx_1;
    int _preimage_sigIdx_;
    int _preimage_sigIdx__1;
    _hash_sigIdx__1 = ctx->getSignalOffset(__cIdx, 0x2e3d9ecc741a7811LL /* hash */);
    _preimage_sigIdx__1 = ctx->getSignalOffset(__cIdx, 0x685f73f30e97244bLL /* preimage */);
    /* signal input hash */
    /* signal input preimage */
    /* component poseidon2 = Poseidon2Hash() */
    /* poseidon2.hash <== hash */
    _compIdx = ctx->getSubComponentOffset(__cIdx, 0x724e2c0915446252LL /* poseidon2 */);
    _hash_sigIdx_ = ctx->getSignalOffset(_compIdx, 0x2e3d9ecc741a7811LL /* hash */);
    ctx->multiGetSignal(__cIdx, __cIdx, _hash_sigIdx__1, _sigValue, 1);
    ctx->setSignal(__cIdx, _compIdx, _hash_sigIdx_, _sigValue);
    /* poseidon2.preimage <== preimage */
    _compIdx_1 = ctx->getSubComponentOffset(__cIdx, 0x724e2c0915446252LL /* poseidon2 */);
    _preimage_sigIdx_ = ctx->getSignalOffset(_compIdx_1, 0x685f73f30e97244bLL /* preimage */);
    ctx->multiGetSignal(__cIdx, __cIdx, _preimage_sigIdx__1, _sigValue_1, 1);
    ctx->setSignal(__cIdx, _compIdx_1, _preimage_sigIdx_, _sigValue_1);
    ctx->finished(__cIdx);
}
/*
Poseidon2Hash
*/
void Poseidon2Hash_ab8f36509768f5a6(Circom_CalcWit *ctx, int __cIdx) {
    FrElement _sigValue[1];
    FrElement _sigValue_1[1];
    FrElement _tmp[1];
    FrElement _sigValue_2[1];
    FrElement _tmp_1[1];
    FrElement _sigValue_3[1];
    FrElement _tmp_2[1];
    FrElement _sigValue_4[1];
    FrElement _tmp_3[1];
    FrElement _sigValue_5[1];
    FrElement _tmp_4[1];
    FrElement _sigValue_6[1];
    FrElement _tmp_5[1];
    FrElement _sigValue_7[1];
    FrElement _tmp_6[1];
    FrElement _sigValue_8[1];
    FrElement _tmp_7[1];
    FrElement _sigValue_9[1];
    FrElement _tmp_8[1];
    FrElement _sigValue_10[1];
    FrElement _tmp_9[1];
    FrElement _sigValue_11[1];
    FrElement _tmp_10[1];
    FrElement _sigValue_12[1];
    FrElement _tmp_11[1];
    FrElement _sigValue_13[1];
    FrElement _tmp_12[1];
    FrElement _sigValue_14[1];
    FrElement _tmp_13[1];
    FrElement _sigValue_15[1];
    FrElement _tmp_14[1];
    FrElement _sigValue_16[1];
    FrElement _tmp_15[1];
    FrElement _sigValue_17[1];
    FrElement _tmp_16[1];
    FrElement _sigValue_18[1];
    FrElement _sigValue_19[1];
    FrElement _tmp_17[1];
    FrElement _sigValue_20[1];
    FrElement _tmp_18[1];
    FrElement sum[1];
    FrElement _sigValue_21[1];
    FrElement _sigValue_22[1];
    FrElement _tmp_19[1];
    FrElement _sigValue_23[1];
    FrElement _tmp_20[1];
    FrElement product[1];
    FrElement _sigValue_24[1];
    int _preimage_sigIdx_;
    int _state_sigIdx_;
    int _offset;
    int _offset_1;
    int _offset_2;
    int _offset_3;
    int _round1_sigIdx_;
    int _offset_4;
    int _offset_5;
    int _offset_6;
    int _offset_7;
    int _offset_8;
    int _offset_9;
    int _round2_sigIdx_;
    int _offset_10;
    int _offset_11;
    int _offset_12;
    int _offset_13;
    int _offset_14;
    int _offset_15;
    int _round3_sigIdx_;
    int _offset_16;
    int _offset_17;
    int _offset_18;
    int _offset_19;
    int _offset_20;
    int _offset_21;
    int _round4_sigIdx_;
    int _offset_22;
    int _offset_23;
    int _offset_24;
    int _offset_25;
    int _offset_26;
    int _offset_27;
    int _round5_sigIdx_;
    int _offset_28;
    int _offset_29;
    int _offset_30;
    int _offset_31;
    int _offset_32;
    int _offset_33;
    int _offset_34;
    int _offset_35;
    int _offset_37;
    int _offset_38;
    int _offset_39;
    int _offset_41;
    int _hash_sigIdx_;
    Circom_Sizes _sigSizes_state;
    Circom_Sizes _sigSizes_round1;
    Circom_Sizes _sigSizes_round2;
    Circom_Sizes _sigSizes_round3;
    Circom_Sizes _sigSizes_round4;
    Circom_Sizes _sigSizes_round5;
    _preimage_sigIdx_ = ctx->getSignalOffset(__cIdx, 0x685f73f30e97244bLL /* preimage */);
    _state_sigIdx_ = ctx->getSignalOffset(__cIdx, 0xee63aaad45b1b116LL /* state */);
    _round1_sigIdx_ = ctx->getSignalOffset(__cIdx, 0x6e996a003d10635eLL /* round1 */);
    _round2_sigIdx_ = ctx->getSignalOffset(__cIdx, 0x6e9969003d1061abLL /* round2 */);
    _round3_sigIdx_ = ctx->getSignalOffset(__cIdx, 0x6e9968003d105ff8LL /* round3 */);
    _round4_sigIdx_ = ctx->getSignalOffset(__cIdx, 0x6e996f003d106bddLL /* round4 */);
    _round5_sigIdx_ = ctx->getSignalOffset(__cIdx, 0x6e996e003d106a2aLL /* round5 */);
    _hash_sigIdx_ = ctx->getSignalOffset(__cIdx, 0x2e3d9ecc741a7811LL /* hash */);
    _sigSizes_state = ctx->getSignalSizes(__cIdx, 0xee63aaad45b1b116LL /* state */);
    _sigSizes_round1 = ctx->getSignalSizes(__cIdx, 0x6e996a003d10635eLL /* round1 */);
    _sigSizes_round2 = ctx->getSignalSizes(__cIdx, 0x6e9969003d1061abLL /* round2 */);
    _sigSizes_round3 = ctx->getSignalSizes(__cIdx, 0x6e9968003d105ff8LL /* round3 */);
    _sigSizes_round4 = ctx->getSignalSizes(__cIdx, 0x6e996f003d106bddLL /* round4 */);
    _sigSizes_round5 = ctx->getSignalSizes(__cIdx, 0x6e996e003d106a2aLL /* round5 */);
    /* signal input hash */
    /* signal input preimage */
    /* signal state[3] */
    /* signal round1[3] */
    /* signal round2[3] */
    /* signal round3[3] */
    /* signal round4[3] */
    /* signal round5[3] */
    /* state[0] <== preimage */
    ctx->multiGetSignal(__cIdx, __cIdx, _preimage_sigIdx_, _sigValue, 1);
    _offset = _state_sigIdx_;
    ctx->setSignal(__cIdx, __cIdx, _offset, _sigValue);
    /* state[1] <== preimage + 1 */
    ctx->multiGetSignal(__cIdx, __cIdx, _preimage_sigIdx_, _sigValue_1, 1);
    Fr_add(_tmp, _sigValue_1, (ctx->circuit->constants + 1));
    _offset_1 = _state_sigIdx_ + 1*_sigSizes_state[1];
    ctx->setSignal(__cIdx, __cIdx, _offset_1, _tmp);
    /* state[2] <== preimage + 2 */
    ctx->multiGetSignal(__cIdx, __cIdx, _preimage_sigIdx_, _sigValue_2, 1);
    Fr_add(_tmp_1, _sigValue_2, (ctx->circuit->constants + 2));
    _offset_2 = _state_sigIdx_ + 2*_sigSizes_state[1];
    ctx->setSignal(__cIdx, __cIdx, _offset_2, _tmp_1);
    /* round1[0] <== state[0] + 1 */
    _offset_3 = _state_sigIdx_;
    ctx->multiGetSignal(__cIdx, __cIdx, _offset_3, _sigValue_3, 1);
    Fr_add(_tmp_2, _sigValue_3, (ctx->circuit->constants + 1));
    _offset_4 = _round1_sigIdx_;
    ctx->setSignal(__cIdx, __cIdx, _offset_4, _tmp_2);
    /* round1[1] <== state[1] + 2 */
    _offset_5 = _state_sigIdx_ + 1*_sigSizes_state[1];
    ctx->multiGetSignal(__cIdx, __cIdx, _offset_5, _sigValue_4, 1);
    Fr_add(_tmp_3, _sigValue_4, (ctx->circuit->constants + 2));
    _offset_6 = _round1_sigIdx_ + 1*_sigSizes_round1[1];
    ctx->setSignal(__cIdx, __cIdx, _offset_6, _tmp_3);
    /* round1[2] <== state[2] + 3 */
    _offset_7 = _state_sigIdx_ + 2*_sigSizes_state[1];
    ctx->multiGetSignal(__cIdx, __cIdx, _offset_7, _sigValue_5, 1);
    Fr_add(_tmp_4, _sigValue_5, (ctx->circuit->constants + 3));
    _offset_8 = _round1_sigIdx_ + 2*_sigSizes_round1[1];
    ctx->setSignal(__cIdx, __cIdx, _offset_8, _tmp_4);
    /* round2[0] <== round1[0] + 4 */
    _offset_9 = _round1_sigIdx_;
    ctx->multiGetSignal(__cIdx, __cIdx, _offset_9, _sigValue_6, 1);
    Fr_add(_tmp_5, _sigValue_6, (ctx->circuit->constants + 4));
    _offset_10 = _round2_sigIdx_;
    ctx->setSignal(__cIdx, __cIdx, _offset_10, _tmp_5);
    /* round2[1] <== round1[1] + 5 */
    _offset_11 = _round1_sigIdx_ + 1*_sigSizes_round1[1];
    ctx->multiGetSignal(__cIdx, __cIdx, _offset_11, _sigValue_7, 1);
    Fr_add(_tmp_6, _sigValue_7, (ctx->circuit->constants + 5));
    _offset_12 = _round2_sigIdx_ + 1*_sigSizes_round2[1];
    ctx->setSignal(__cIdx, __cIdx, _offset_12, _tmp_6);
    /* round2[2] <== round1[2] + 6 */
    _offset_13 = _round1_sigIdx_ + 2*_sigSizes_round1[1];
    ctx->multiGetSignal(__cIdx, __cIdx, _offset_13, _sigValue_8, 1);
    Fr_add(_tmp_7, _sigValue_8, (ctx->circuit->constants + 6));
    _offset_14 = _round2_sigIdx_ + 2*_sigSizes_round2[1];
    ctx->setSignal(__cIdx, __cIdx, _offset_14, _tmp_7);
    /* round3[0] <== round2[0] + 7 */
    _offset_15 = _round2_sigIdx_;
    ctx->multiGetSignal(__cIdx, __cIdx, _offset_15, _sigValue_9, 1);
    Fr_add(_tmp_8, _sigValue_9, (ctx->circuit->constants + 7));
    _offset_16 = _round3_sigIdx_;
    ctx->setSignal(__cIdx, __cIdx, _offset_16, _tmp_8);
    /* round3[1] <== round2[1] + 8 */
    _offset_17 = _round2_sigIdx_ + 1*_sigSizes_round2[1];
    ctx->multiGetSignal(__cIdx, __cIdx, _offset_17, _sigValue_10, 1);
    Fr_add(_tmp_9, _sigValue_10, (ctx->circuit->constants + 8));
    _offset_18 = _round3_sigIdx_ + 1*_sigSizes_round3[1];
    ctx->setSignal(__cIdx, __cIdx, _offset_18, _tmp_9);
    /* round3[2] <== round2[2] + 9 */
    _offset_19 = _round2_sigIdx_ + 2*_sigSizes_round2[1];
    ctx->multiGetSignal(__cIdx, __cIdx, _offset_19, _sigValue_11, 1);
    Fr_add(_tmp_10, _sigValue_11, (ctx->circuit->constants + 9));
    _offset_20 = _round3_sigIdx_ + 2*_sigSizes_round3[1];
    ctx->setSignal(__cIdx, __cIdx, _offset_20, _tmp_10);
    /* round4[0] <== round3[0] + 10 */
    _offset_21 = _round3_sigIdx_;
    ctx->multiGetSignal(__cIdx, __cIdx, _offset_21, _sigValue_12, 1);
    Fr_add(_tmp_11, _sigValue_12, (ctx->circuit->constants + 10));
    _offset_22 = _round4_sigIdx_;
    ctx->setSignal(__cIdx, __cIdx, _offset_22, _tmp_11);
    /* round4[1] <== round3[1] + 11 */
    _offset_23 = _round3_sigIdx_ + 1*_sigSizes_round3[1];
    ctx->multiGetSignal(__cIdx, __cIdx, _offset_23, _sigValue_13, 1);
    Fr_add(_tmp_12, _sigValue_13, (ctx->circuit->constants + 11));
    _offset_24 = _round4_sigIdx_ + 1*_sigSizes_round4[1];
    ctx->setSignal(__cIdx, __cIdx, _offset_24, _tmp_12);
    /* round4[2] <== round3[2] + 12 */
    _offset_25 = _round3_sigIdx_ + 2*_sigSizes_round3[1];
    ctx->multiGetSignal(__cIdx, __cIdx, _offset_25, _sigValue_14, 1);
    Fr_add(_tmp_13, _sigValue_14, (ctx->circuit->constants + 12));
    _offset_26 = _round4_sigIdx_ + 2*_sigSizes_round4[1];
    ctx->setSignal(__cIdx, __cIdx, _offset_26, _tmp_13);
    /* round5[0] <== round4[0] + 13 */
    _offset_27 = _round4_sigIdx_;
    ctx->multiGetSignal(__cIdx, __cIdx, _offset_27, _sigValue_15, 1);
    Fr_add(_tmp_14, _sigValue_15, (ctx->circuit->constants + 13));
    _offset_28 = _round5_sigIdx_;
    ctx->setSignal(__cIdx, __cIdx, _offset_28, _tmp_14);
    /* round5[1] <== round4[1] + 14 */
    _offset_29 = _round4_sigIdx_ + 1*_sigSizes_round4[1];
    ctx->multiGetSignal(__cIdx, __cIdx, _offset_29, _sigValue_16, 1);
    Fr_add(_tmp_15, _sigValue_16, (ctx->circuit->constants + 14));
    _offset_30 = _round5_sigIdx_ + 1*_sigSizes_round5[1];
    ctx->setSignal(__cIdx, __cIdx, _offset_30, _tmp_15);
    /* round5[2] <== round4[2] + 15 */
    _offset_31 = _round4_sigIdx_ + 2*_sigSizes_round4[1];
    ctx->multiGetSignal(__cIdx, __cIdx, _offset_31, _sigValue_17, 1);
    Fr_add(_tmp_16, _sigValue_17, (ctx->circuit->constants + 15));
    _offset_32 = _round5_sigIdx_ + 2*_sigSizes_round5[1];
    ctx->setSignal(__cIdx, __cIdx, _offset_32, _tmp_16);
    /* var sum = round5[0] + round5[1] + round5[2] */
    _offset_33 = _round5_sigIdx_;
    ctx->multiGetSignal(__cIdx, __cIdx, _offset_33, _sigValue_18, 1);
    _offset_34 = _round5_sigIdx_ + 1*_sigSizes_round5[1];
    ctx->multiGetSignal(__cIdx, __cIdx, _offset_34, _sigValue_19, 1);
    Fr_add(_tmp_17, _sigValue_18, _sigValue_19);
    _offset_35 = _round5_sigIdx_ + 2*_sigSizes_round5[1];
    ctx->multiGetSignal(__cIdx, __cIdx, _offset_35, _sigValue_20, 1);
    Fr_add(_tmp_18, _tmp_17, _sigValue_20);
    Fr_copyn(sum, _tmp_18, 1);
    /* var product = round5[0] * round5[1] * round5[2] */
    _offset_37 = _round5_sigIdx_;
    ctx->multiGetSignal(__cIdx, __cIdx, _offset_37, _sigValue_21, 1);
    _offset_38 = _round5_sigIdx_ + 1*_sigSizes_round5[1];
    ctx->multiGetSignal(__cIdx, __cIdx, _offset_38, _sigValue_22, 1);
    Fr_mul(_tmp_19, _sigValue_21, _sigValue_22);
    _offset_39 = _round5_sigIdx_ + 2*_sigSizes_round5[1];
    ctx->multiGetSignal(__cIdx, __cIdx, _offset_39, _sigValue_23, 1);
    Fr_mul(_tmp_20, _tmp_19, _sigValue_23);
    Fr_copyn(product, _tmp_20, 1);
    /* hash <== round5[0] */
    _offset_41 = _round5_sigIdx_;
    ctx->multiGetSignal(__cIdx, __cIdx, _offset_41, _sigValue_24, 1);
    ctx->setSignal(__cIdx, __cIdx, _hash_sigIdx_, _sigValue_24);
    ctx->finished(__cIdx);
}
// Function Table
Circom_ComponentFunction _functionTable[2] = {
     Poseidon2Circuit_f36fc399b1f610dd
    ,Poseidon2Hash_ab8f36509768f5a6
};
