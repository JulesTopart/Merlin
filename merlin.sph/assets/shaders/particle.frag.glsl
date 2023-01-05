#version 450

in vec3 position;
in vec3 offset;
in vec3 normal;
in vec3 color;
in vec2 texCoord;

out vec4 FragColor;

vec4 blend(vec4 col1, vec4 col2, float p)
{
 	vec4 temp = col1 * col1;
    vec4 temp2 = col2 * col2;
    return sqrt(mix(temp, temp2, p));
}

vec3 fromRedToGreen( float interpolant )
{
 	if( interpolant < 0.5 )
    {
       return vec3(1.0, 2.0 * interpolant, 0.0); 
    }
    else
    {
        return vec3(2.0 - 2.0 * interpolant, 1.0, 0.0 );
    }
}


vec3 fromGreenToBlue( float interpolant )
{
   	if( interpolant < 0.5 )
    {
       return vec3(0.0, 1.0, 2.0 * interpolant); 
    }
    else
    {
        return vec3(0.0, 2.0 - 2.0 * interpolant, 1.0 );
    }  
}

vec3 heat5( float interpolant )
{
    float invertedInterpolant = interpolant;
 	if( invertedInterpolant < 0.5 )
    {
        float remappedFirstHalf = 1.0 - 2.0 * invertedInterpolant;
        return fromGreenToBlue( remappedFirstHalf );
    }
    else
    {
     	float remappedSecondHalf = 2.0 - 2.0 * invertedInterpolant; 
        return fromRedToGreen( remappedSecondHalf );
    }
}

vec3 heat7( float interpolant )
{
 	if( interpolant < 1.0 / 6.0 )
    {
        float firstSegmentInterpolant = 6.0 * interpolant;
        return ( 1.0 - firstSegmentInterpolant ) * vec3(0.0, 0.0, 0.0) + firstSegmentInterpolant * vec3(0.0, 0.0, 1.0);
    }
    else if( interpolant < 5.0 / 6.0 )
    {
        float midInterpolant = 0.25 * ( 6.0 * interpolant - 1.0 );
        return heat5( midInterpolant );
    }
    else
    {
    	float lastSegmentInterpolant = 6.0 * interpolant - 5.0; 
        return ( 1.0 - lastSegmentInterpolant ) * vec3(1.0, 0.0, 0.0) + lastSegmentInterpolant * vec3(1.0, 1.0, 1.0);
    }
}


void main() {
  FragColor = vec4(heat7(color.r), 1.0);
}