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
Poseidon2Fixed
*/
void Poseidon2Fixed_6447a4db301c823c(Circom_CalcWit *ctx, int __cIdx) {
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
    int _temp1_sigIdx_;
    int _temp2_sigIdx_;
    int _temp3_sigIdx_;
    int _temp4_sigIdx_;
    int _temp5_sigIdx_;
    int _hash_sigIdx_;
    _preimage_sigIdx_ = ctx->getSignalOffset(__cIdx, 0x685f73f30e97244bLL /* preimage */);
    _temp1_sigIdx_ = ctx->getSignalOffset(__cIdx, 0x23c14e48e17e5042LL /* temp1 */);
    _temp2_sigIdx_ = ctx->getSignalOffset(__cIdx, 0x23c14d48e17e4e8fLL /* temp2 */);
    _temp3_sigIdx_ = ctx->getSignalOffset(__cIdx, 0x23c14c48e17e4cdcLL /* temp3 */);
    _temp4_sigIdx_ = ctx->getSignalOffset(__cIdx, 0x23c14b48e17e4b29LL /* temp4 */);
    _temp5_sigIdx_ = ctx->getSignalOffset(__cIdx, 0x23c14a48e17e4976LL /* temp5 */);
    _hash_sigIdx_ = ctx->getSignalOffset(__cIdx, 0x2e3d9ecc741a7811LL /* hash */);
    /* signal input hash */
    /* signal input preimage */
    /* signal temp1 */
    /* signal temp2 */
    /* signal temp3 */
    /* signal temp4 */
    /* signal temp5 */
    /* temp1 <== preimage + 1 */
    ctx->multiGetSignal(__cIdx, __cIdx, _preimage_sigIdx_, _sigValue, 1);
    Fr_add(_tmp, _sigValue, (ctx->circuit->constants + 1));
    ctx->setSignal(__cIdx, __cIdx, _temp1_sigIdx_, _tmp);
    /* temp2 <== temp1 + 2 */
    ctx->multiGetSignal(__cIdx, __cIdx, _temp1_sigIdx_, _sigValue_1, 1);
    Fr_add(_tmp_1, _sigValue_1, (ctx->circuit->constants + 2));
    ctx->setSignal(__cIdx, __cIdx, _temp2_sigIdx_, _tmp_1);
    /* temp3 <== temp2 + 3 */
    ctx->multiGetSignal(__cIdx, __cIdx, _temp2_sigIdx_, _sigValue_2, 1);
    Fr_add(_tmp_2, _sigValue_2, (ctx->circuit->constants + 3));
    ctx->setSignal(__cIdx, __cIdx, _temp3_sigIdx_, _tmp_2);
    /* temp4 <== temp3 + 4 */
    ctx->multiGetSignal(__cIdx, __cIdx, _temp3_sigIdx_, _sigValue_3, 1);
    Fr_add(_tmp_3, _sigValue_3, (ctx->circuit->constants + 4));
    ctx->setSignal(__cIdx, __cIdx, _temp4_sigIdx_, _tmp_3);
    /* temp5 <== temp4 + 5 */
    ctx->multiGetSignal(__cIdx, __cIdx, _temp4_sigIdx_, _sigValue_4, 1);
    Fr_add(_tmp_4, _sigValue_4, (ctx->circuit->constants + 5));
    ctx->setSignal(__cIdx, __cIdx, _temp5_sigIdx_, _tmp_4);
    /* hash <== temp5 */
    ctx->multiGetSignal(__cIdx, __cIdx, _temp5_sigIdx_, _sigValue_5, 1);
    ctx->setSignal(__cIdx, __cIdx, _hash_sigIdx_, _sigValue_5);
    ctx->finished(__cIdx);
}
// Function Table
Circom_ComponentFunction _functionTable[1] = {
     Poseidon2Fixed_6447a4db301c823c
};
