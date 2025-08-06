#include "circom.hpp"
#include "calcwit.hpp"
#define NSignals 8
#define NComponents 1
#define NOutputs 0
#define NInputs 2
#define NVars 3
#define NPublic 2
#define __P__ "21888242871839275222246405745257275088548364400416034343698204186575808495617"

/*
SimplePoseidon
*/
void SimplePoseidon_584ab2b0d8a01b4e(Circom_CalcWit *ctx, int __cIdx) {
    FrElement _sigValue[1];
    FrElement _tmp[1];
    FrElement _sigValue_1[1];
    FrElement _tmp_1[1];
    FrElement _sigValue_2[1];
    FrElement _tmp_2[1];
    FrElement _sigValue_3[1];
    FrElement _tmp_3[1];
    FrElement _sigValue_4[1];
    FrElement _tmp_4[1];
    FrElement _sigValue_5[1];
    int _preimage_sigIdx_;
    int _intermediate1_sigIdx_;
    int _intermediate2_sigIdx_;
    int _intermediate3_sigIdx_;
    int _intermediate4_sigIdx_;
    int _intermediate5_sigIdx_;
    int _hash_sigIdx_;
    _preimage_sigIdx_ = ctx->getSignalOffset(__cIdx, 0x685f73f30e97244bLL /* preimage */);
    _intermediate1_sigIdx_ = ctx->getSignalOffset(__cIdx, 0x5b6affb484747a8fLL /* intermediate1 */);
    _intermediate2_sigIdx_ = ctx->getSignalOffset(__cIdx, 0x5b6b00b484747c42LL /* intermediate2 */);
    _intermediate3_sigIdx_ = ctx->getSignalOffset(__cIdx, 0x5b6b01b484747df5LL /* intermediate3 */);
    _intermediate4_sigIdx_ = ctx->getSignalOffset(__cIdx, 0x5b6afab484747210LL /* intermediate4 */);
    _intermediate5_sigIdx_ = ctx->getSignalOffset(__cIdx, 0x5b6afbb4847473c3LL /* intermediate5 */);
    _hash_sigIdx_ = ctx->getSignalOffset(__cIdx, 0x2e3d9ecc741a7811LL /* hash */);
    /* signal input hash */
    /* signal input preimage */
    /* signal intermediate1 */
    /* signal intermediate2 */
    /* signal intermediate3 */
    /* signal intermediate4 */
    /* signal intermediate5 */
    /* intermediate1 <== preimage + 1 */
    ctx->multiGetSignal(__cIdx, __cIdx, _preimage_sigIdx_, _sigValue, 1);
    Fr_add(_tmp, _sigValue, (ctx->circuit->constants + 1));
    ctx->setSignal(__cIdx, __cIdx, _intermediate1_sigIdx_, _tmp);
    /* intermediate2 <== intermediate1 + 2 */
    ctx->multiGetSignal(__cIdx, __cIdx, _intermediate1_sigIdx_, _sigValue_1, 1);
    Fr_add(_tmp_1, _sigValue_1, (ctx->circuit->constants + 2));
    ctx->setSignal(__cIdx, __cIdx, _intermediate2_sigIdx_, _tmp_1);
    /* intermediate3 <== intermediate2 + 3 */
    ctx->multiGetSignal(__cIdx, __cIdx, _intermediate2_sigIdx_, _sigValue_2, 1);
    Fr_add(_tmp_2, _sigValue_2, (ctx->circuit->constants + 3));
    ctx->setSignal(__cIdx, __cIdx, _intermediate3_sigIdx_, _tmp_2);
    /* intermediate4 <== intermediate3 + 4 */
    ctx->multiGetSignal(__cIdx, __cIdx, _intermediate3_sigIdx_, _sigValue_3, 1);
    Fr_add(_tmp_3, _sigValue_3, (ctx->circuit->constants + 4));
    ctx->setSignal(__cIdx, __cIdx, _intermediate4_sigIdx_, _tmp_3);
    /* intermediate5 <== intermediate4 + 5 */
    ctx->multiGetSignal(__cIdx, __cIdx, _intermediate4_sigIdx_, _sigValue_4, 1);
    Fr_add(_tmp_4, _sigValue_4, (ctx->circuit->constants + 5));
    ctx->setSignal(__cIdx, __cIdx, _intermediate5_sigIdx_, _tmp_4);
    /* hash <== intermediate5 */
    ctx->multiGetSignal(__cIdx, __cIdx, _intermediate5_sigIdx_, _sigValue_5, 1);
    ctx->setSignal(__cIdx, __cIdx, _hash_sigIdx_, _sigValue_5);
    ctx->finished(__cIdx);
}
// Function Table
Circom_ComponentFunction _functionTable[1] = {
     SimplePoseidon_584ab2b0d8a01b4e
};
