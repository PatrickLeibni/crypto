template Poseidon2Fixed() {
    signal input hash;
    signal input preimage;
    
    signal temp1;
    signal temp2;
    signal temp3;
    signal temp4;
    signal temp5;
    
    temp1 <== preimage + 1;
    temp2 <== temp1 + 2;
    temp3 <== temp2 + 3;
    temp4 <== temp3 + 4;
    temp5 <== temp4 + 5;
    
    hash <== temp5;
}

component main = Poseidon2Fixed();