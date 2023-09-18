#define n 1000 //Number of data
#define wgSize 32 //WorkGroup size

layout(std430, binding = 0) buffer dataBuffer {
    uint data[];
};

layout(std430, binding = 1) buffer prefixSumBuffer {
    uint globalPrefixSum[10];
};