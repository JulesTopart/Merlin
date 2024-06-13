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

uniform uint dataSize;
uniform uint blockSize;
uniform uint blocks;

