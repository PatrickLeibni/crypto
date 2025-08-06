const fs = require('fs');
const path = require('path');

// 读取输入文件（从outputs目录）
const inputPath = path.join(__dirname, '..', 'outputs', 'input.json');
const input = JSON.parse(fs.readFileSync(inputPath, 'utf8'));

// 创建witness数据
const witness = {
    "preimage": input.preimage,
    "hash": input.hash
};

// 将witness数据写入build目录
const witnessJsonPath = path.join(__dirname, '..', 'build', 'witness.json');
fs.writeFileSync(witnessJsonPath, JSON.stringify(witness, null, 2));

console.log('Witness JSON file generated:', witnessJsonPath);

// 使用snarkjs生成.wtns文件
const { execSync } = require('child_process');
const wasmPath = path.join(__dirname, '..', 'build', 'poseidon2_accurate.wasm');
const wtnsPath = path.join(__dirname, '..', 'build', 'witness.wtns');

try {
    console.log('Generating .wtns file...');
    execSync(`npx snarkjs wtns calculate ${wasmPath} ${witnessJsonPath} ${wtnsPath}`, {
        stdio: 'inherit',
        cwd: path.join(__dirname, '..')
    });
    console.log('✅ Witness .wtns file generated:', wtnsPath);
} catch (error) {
    console.error('❌ Error generating .wtns file:', error.message);
    console.log('Note: You may need to compile the circuit first');
} 