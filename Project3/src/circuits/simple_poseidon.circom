template SimplePoseidon() {
    // Public input: hash output
    signal input hash;
    
    // Private input: preimage
    signal input preimage;
    
    // Simple hash calculation
    signal intermediate1;
    signal intermediate2;
    signal intermediate3;
    signal intermediate4;
    signal intermediate5;
    
    // Step 1: initial transformation
    intermediate1 <== preimage + 1;
    
    // Step 2: second transformation
    intermediate2 <== intermediate1 + 2;
    
    // Step 3: third transformation
    intermediate3 <== intermediate2 + 3;
    
    // Step 4: fourth transformation
    intermediate4 <== intermediate3 + 4;
    
    // Step 5: final transformation
    intermediate5 <== intermediate4 + 5;
    
    // Output constraint
    hash <== intermediate5;
}

component main = SimplePoseidon(); 