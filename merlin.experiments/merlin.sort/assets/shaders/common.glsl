#version 460
layout(std430) buffer inDataBuffer {
    uint data[];
};

layout(std430) buffer prefixSumBuffer {
    uint globalPrefixSum[];
};

layout(std430) buffer compactSumBuffer {
    uint compactPrefixSum[];
};

const uint dataSize = 128; //data size
const uint wgSize = 1; //WorkGroup size

const uint blockSize = uint(floor(log2(dataSize)));
const uint blocks = (dataSize + blockSize - 1) / blockSize;

layout(local_size_x = wgSize) in;