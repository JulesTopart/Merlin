layout(std430, binding = 0) buffer inDataBuffer {
    uint data[];
};

layout(std430, binding = 1) buffer prefixSumBuffer {
    uint globalPrefixSum[];
};

uniform uint size; //input size
#define wgSize 32 //WorkGroup size
