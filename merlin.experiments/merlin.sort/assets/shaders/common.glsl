#version 460
layout(std430) buffer inDataBuffer {
    uint inData[];
};

layout(std430) buffer outDataBuffer {
    uint outData[];
};

layout(std430) buffer prefixSumBuffer {
    uint globalPrefixSum[];
};

layout(std430) buffer compactSumBuffer {
    uint compactPrefixSum[];
};

const uint dataSize = 128; //data size

const uint blockSize = uint(floor(log2(dataSize)));
const uint blocks = (dataSize + blockSize - 1) / blockSize;

