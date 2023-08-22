#version 450

layout (local_size_x = 64, local_size_y = 1, local_size_z = 1) in;


struct Node {
    vec3 U;		//Position
	vec3 V;		//Velocity

	int enable;	//particle is deactivated
	uint index;
};

layout (std430, binding = 1) buffer NodeBuffer {
  Node nodes[];
};

uniform uint nodeCount;
uniform uint sqNodeCount;
uniform float iTime;

float erosionStrength = 0.9;

float rand(vec3 co)
{
	return -1.0 + fract(sin(dot(co.xy, vec2(12.9898 + co.z, 78.233))) * 43758.5453) * 2.0;
}

float linearRand(vec3 uv)
{
	vec3 iuv = floor(uv);
	vec3 fuv = fract(uv);

	float v1 = rand(iuv + vec3(0, 0, 0));
	float v2 = rand(iuv + vec3(1, 0, 0));
	float v3 = rand(iuv + vec3(0, 1, 0));
	float v4 = rand(iuv + vec3(1, 1, 0));

	float d1 = rand(iuv + vec3(0, 0, 1));
	float d2 = rand(iuv + vec3(1, 0, 1));
	float d3 = rand(iuv + vec3(0, 1, 1));
	float d4 = rand(iuv + vec3(1, 1, 1));

	return mix(mix(mix(v1, v2, fuv.x), mix(v3, v4, fuv.x), fuv.y),
			   mix(mix(d1, d2, fuv.x), mix(d3, d4, fuv.x), fuv.y),
			   fuv.z);
}

float linearRandFBM(vec3 uv)
{
	float c = (linearRand(uv * 1.0) * 32.0 +
			   linearRand(uv * 2.0) * 16.0 +
			   linearRand(uv * 4.0) * 8.0 +
			   linearRand(uv * 8.0) * 4.0) / 32.0;
	return c * 0.5 + 0.5;
}

const uint packedBunny[1024] = uint[1024](0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 917504u, 917504u, 917504u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 1966080u, 12531712u, 16742400u, 16742400u, 16723968u, 16711680u, 8323072u, 4128768u, 2031616u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 6144u, 2063360u, 16776704u, 33553920u, 33553920u, 33553920u, 33553920u, 33520640u, 16711680u, 8323072u, 8323072u, 2031616u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 268435456u, 402653184u, 134217728u, 201326592u, 67108864u, 0u, 0u, 7168u, 2031104u, 16776960u, 33554176u, 33554176u, 33554304u, 33554176u, 33554176u, 33554176u, 33553920u, 16744448u, 8323072u, 4128768u, 1572864u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 805306368u, 939524096u, 402653184u, 478150656u, 260046848u, 260046848u, 260046848u, 125832192u, 130055680u, 67108608u, 33554304u, 33554304u, 33554304u, 33554304u, 33554304u, 33554304u, 33554304u, 33554176u, 16776704u, 8355840u, 4128768u, 917504u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 805306368u, 1056964608u, 1056964608u, 528482304u, 528482304u, 260046848u, 260046848u, 260046848u, 130039296u, 130154240u, 67108739u, 67108807u, 33554375u, 33554375u, 33554370u, 33554368u, 33554368u, 33554304u, 33554304u, 16776960u, 8330240u, 4128768u, 393216u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 939524096u, 1040187392u, 1040187392u, 520093696u, 251658240u, 251658240u, 260046848u, 125829120u, 125829120u, 130088704u, 63045504u, 33554375u, 33554375u, 33554375u, 33554407u, 33554407u, 33554370u, 33554370u, 33554374u, 33554310u, 16776966u, 4144642u, 917504u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 15360u, 130816u, 262017u, 4194247u, 33554383u, 67108847u, 33554415u, 33554407u, 33554407u, 33554375u, 33554375u, 33554318u, 2031502u, 32262u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 31744u, 130816u, 262019u, 2097151u, 134217727u, 134217727u, 67108863u, 33554415u, 33554407u, 33554415u, 33554383u, 2097102u, 982926u, 32262u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 31744u, 130816u, 524263u, 117964799u, 127926271u, 134217727u, 67108863u, 16777215u, 4194303u, 4194303u, 2097151u, 1048574u, 65422u, 16134u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 3u, 31751u, 130951u, 524287u, 252182527u, 261095423u, 261095423u, 59768830u, 2097150u, 1048574u, 1048575u, 262143u, 131070u, 65534u, 16134u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 7u, 31751u, 130959u, 503840767u, 520617982u, 529530879u, 261095423u, 1048575u, 1048574u, 1048574u, 524286u, 524287u, 131070u, 65534u, 16134u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 3u, 1799u, 32527u, 134348750u, 1040449534u, 1057488894u, 520617982u, 51380223u, 1048575u, 1048575u, 524287u, 524287u, 524287u, 131070u, 65534u, 15886u, 6u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 1536u, 3968u, 8175u, 65535u, 1006764030u, 1040449534u, 1057488894u, 50855934u, 524286u, 524286u, 524287u, 524287u, 524286u, 262142u, 131070u, 65534u, 32270u, 14u, 6u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 3968u, 8160u, 8191u, 805371903u, 2080505854u, 2114191358u, 101187582u, 34078718u, 524286u, 524286u, 524286u, 524286u, 524286u, 524286u, 262142u, 131070u, 32766u, 8078u, 3590u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 8128u, 8176u, 16383u, 2013331455u, 2080505854u, 235143166u, 101187582u, 524286u, 1048574u, 1048574u, 1048574u, 1048574u, 524286u, 524286u, 262142u, 131070u, 32766u, 16382u, 8070u, 1024u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 8160u, 8184u, 1879064574u, 2013331455u, 470024190u, 67371006u, 524286u, 1048574u, 1048574u, 1048574u, 1048574u, 1048574u, 1048574u, 524286u, 524286u, 262142u, 65534u, 16382u, 8160u, 1024u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 8128u, 8184u, 805322750u, 402718719u, 134479870u, 524286u, 524286u, 1048574u, 1048574u, 1048574u, 1048574u, 1048574u, 1048574u, 1048574u, 524286u, 262142u, 65534u, 16382u, 16368u, 1792u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 3968u, 8184u, 16382u, 131071u, 262142u, 524286u, 1048574u, 1048574u, 1048574u, 1048574u, 1048574u, 1048574u, 1048574u, 1048574u, 524286u, 262142u, 65534u, 16382u, 16368u, 1792u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 1792u, 8184u, 16380u, 65535u, 262143u, 524286u, 524286u, 1048574u, 1048574u, 1048575u, 1048574u, 1048574u, 1048574u, 1048574u, 524286u, 262142u, 65534u, 16376u, 16368u, 1792u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 8176u, 16376u, 32767u, 262143u, 524286u, 1048574u, 1048574u, 1048575u, 1048575u, 1048575u, 1048575u, 1048574u, 1048574u, 524286u, 262142u, 32766u, 16376u, 8176u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 4032u, 8184u, 32766u, 262142u, 524286u, 524286u, 1048575u, 1048574u, 1048574u, 1048574u, 1048574u, 1048574u, 1048574u, 524286u, 262142u, 32766u, 16376u, 8176u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 384u, 8184u, 32766u, 131070u, 262142u, 524286u, 1048575u, 1048574u, 1048574u, 1048574u, 1048574u, 1048574u, 524286u, 524286u, 131070u, 32766u, 16368u, 1920u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 4080u, 32764u, 65534u, 262142u, 524286u, 524286u, 524286u, 1048574u, 1048574u, 524286u, 524286u, 524286u, 262142u, 131070u, 32764u, 8160u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 256u, 16376u, 32760u, 131068u, 262140u, 262142u, 524286u, 524286u, 524286u, 524286u, 524286u, 262142u, 131070u, 65532u, 16368u, 3840u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 3968u, 32752u, 65528u, 131068u, 262142u, 262142u, 262142u, 262142u, 262142u, 262142u, 262140u, 131064u, 32752u, 7936u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 8064u, 32736u, 65528u, 131070u, 131070u, 131070u, 131070u, 131070u, 131070u, 65532u, 32752u, 8160u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 3456u, 16376u, 32764u, 65534u, 65534u, 65534u, 32766u, 32764u, 16380u, 4048u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 48u, 2680u, 8188u, 8188u, 8188u, 8188u, 4092u, 120u, 16u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 120u, 248u, 508u, 508u, 508u, 248u, 240u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 96u, 240u, 504u, 504u, 504u, 240u, 96u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 224u, 224u, 224u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u);


float sampleBunny(vec3 uvs)
{
    vec3 voxelUvs = max(vec3(0.0), min(uvs * vec3(32), vec3(32) - 1.0));
    uvec3 intCoord = uvec3(voxelUvs.x, voxelUvs.z, voxelUvs.y);
    uint arrayCoord = intCoord.x + intCoord.z * uint(32);

    if (uvs.x < -0.001 || uvs.y < -0.001 || uvs.z < -0.001 ||
      uvs.x > 1.001 || uvs.y > 1.001 || uvs.z > 1.001)
        return 0.0;


    uvec3 intCoord2 = min(intCoord + uvec3(1), uvec3(32 - 1));

    uint arrayCoord00 = intCoord.x + intCoord.z * uint(32);
    uint arrayCoord01 = intCoord.x + intCoord2.z * uint(32);
    uint arrayCoord10 = intCoord2.x + intCoord.z * uint(32);
    uint arrayCoord11 = intCoord2.x + intCoord2.z * uint(32);

    uint bunnyDepthData00 = packedBunny[arrayCoord00];
    uint bunnyDepthData01 = packedBunny[arrayCoord01];
    uint bunnyDepthData10 = packedBunny[arrayCoord10];
    uint bunnyDepthData11 = packedBunny[arrayCoord11];

    float voxel000 = (bunnyDepthData00 & (1u << intCoord.y)) > 0u ? 1.0 : 0.0;
    float voxel001 = (bunnyDepthData01 & (1u << intCoord.y)) > 0u ? 1.0 : 0.0;
    float voxel010 = (bunnyDepthData10 & (1u << intCoord.y)) > 0u ? 1.0 : 0.0;
    float voxel011 = (bunnyDepthData11 & (1u << intCoord.y)) > 0u ? 1.0 : 0.0;
    float voxel100 = (bunnyDepthData00 & (1u << intCoord2.y)) > 0u ? 1.0 : 0.0;
    float voxel101 = (bunnyDepthData01 & (1u << intCoord2.y)) > 0u ? 1.0 : 0.0;
    float voxel110 = (bunnyDepthData10 & (1u << intCoord2.y)) > 0u ? 1.0 : 0.0;
    float voxel111 = (bunnyDepthData11 & (1u << intCoord2.y)) > 0u ? 1.0 : 0.0;

    vec3 d = voxelUvs - vec3(intCoord);

    voxel000 = mix(voxel000, voxel100, d.y);
    voxel001 = mix(voxel001, voxel101, d.y);
    voxel010 = mix(voxel010, voxel110, d.y);
    voxel011 = mix(voxel011, voxel111, d.y);

    voxel000 = mix(voxel000, voxel010, d.x);
    voxel001 = mix(voxel001, voxel011, d.x);

    float voxel = mix(voxel000, voxel001, d.z);

    return voxel;
}


float getDensity(vec3 cubePos)
{
    float density = 1.0; //sampleBunny(cubePos);
    //return density; //disable noise
    if (density == 0.0) return 0.0; // makes things a tad bit faster
    vec3 noiseUV = cubePos;
    noiseUV += iTime*3.0 * vec3(1.0,0.0,0.0);
    density = density * max(0.0, 1.25*erosionStrength*linearRandFBM(noiseUV)*4.0-2.0); // more complex FBM noise
    return density;
}


void main(){
    //Get the actual node coordinate
    uint index = gl_GlobalInvocationID.x;
    vec3 uv = 0.015 * vec3(	(index % (32 * 32)) / 32,
				index % 32, 
				index / (32 * 32));

    vec3 col = uv;
    int w = 1;

    nodes[index].V.x = getDensity(uv);
    //if(nodes[index].V.x < 0.1) w = 0;

    nodes[index].enable = (w>0.0) ? 1 : 0;
    
}
