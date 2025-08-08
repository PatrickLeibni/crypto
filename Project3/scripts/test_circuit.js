const { execSync } = require('child_process');
const fs = require('fs');
const path = require('path');

console.log(' 开始测试零知识证明系统...\n');

try {
    // 1. 验证系统基础功能
    console.log(' 步骤 1: 验证系统基础功能...');
    
    // 创建简单的测试输入
    const simpleInput = {
        "a": 5,
        "b": 3
    };
    fs.writeFileSync(path.join(__dirname, '../outputs/test_input.json'), JSON.stringify(simpleInput, null, 2));
    
    // 测试简单电路
    execSync('npx snarkjs wtns debug ../build/test.wasm ../outputs/test_input.json ../build/test_witness.wtns', { 
        stdio: 'inherit',
        cwd: __dirname 
    });
    console.log('✅ 基础系统验证成功\n');

    // 2. 使用Groth16协议生成证明
    console.log(' 步骤 2: 使用Groth16协议生成零知识证明...');
    execSync('npx snarkjs groth16 fullprove ../outputs/test_input.json ../build/test.wasm ../build/test_groth16.zkey ../outputs/test_groth16_proof.json ../outputs/test_groth16_public.json', { 
        stdio: 'inherit',
        cwd: __dirname 
    });
    console.log('✅ Groth16证明生成成功');

    // 3. 验证证明
    console.log(' 步骤 3: 验证Groth16证明...');
    execSync('npx snarkjs groth16 verify ../build/test_groth16_verification_key.json ../outputs/test_groth16_public.json ../outputs/test_groth16_proof.json', { 
        stdio: 'inherit',
        cwd: __dirname 
    });
    console.log('✅ Groth16证明验证成功\n');

    // 4. 显示证明信息
    console.log(' 步骤 4: 显示证明信息...');
    const proof = JSON.parse(fs.readFileSync(path.join(__dirname, '../outputs/test_groth16_proof.json'), 'utf8'));
    const publicInputs = JSON.parse(fs.readFileSync(path.join(__dirname, '../outputs/test_groth16_public.json'), 'utf8'));
    
    console.log(' 证明文件大小:', JSON.stringify(proof).length, '字节');
    console.log(' 公开输入:', publicInputs);
    console.log('✅ 证明信息显示完成\n');

    console.log(' 所有测试通过！零知识证明系统工作正常。');
    console.log(' 使用Groth16协议，证明生成和验证都成功完成！');

} catch (error) {
    console.error('❌ 测试失败:', error.message);
    console.log('\n 解决方案:');
    console.log('   - 使用 working_solution.js 脚本');
    console.log('   - 或者运行 node test_circuit_fixed.js');
    process.exit(1);
}
