#version 150 core

 uniform sampler2D diffuseTex ;
 uniform sampler2D bumpTex ; // New !

 uniform vec3 cameraPos ;
 uniform vec4 lightColour ;
 uniform vec3 lightPos ;
  uniform vec3 lightDirection;
 uniform float lightRadius ;
uniform float lightCutOff ; 
in Vertex {
vec3 colour ;
vec2 texCoord ;
vec3 normal ;
vec3 tangent ; // New !
vec3 binormal ; // New !
vec3 worldPos ;
} IN ;

out vec4 fragColour ;
void main ( void ) {

vec3 lightZone = normalize(lightPos - IN.worldPos);

float  lightDistance = dot(lightZone , normalize(-lightDirection));
    
if(lightDistance  > lightCutOff) 
{       
   vec4 diffuse = texture ( diffuseTex , IN . texCoord );
// New !
mat3 TBN = mat3 ( IN.tangent , IN.binormal , IN.normal );
// New !
vec3 normal = normalize ( TBN * ( texture ( bumpTex ,
IN.texCoord ). rgb * 2.0 - 1.0));
 vec3 incident = normalize ( lightPos - IN.worldPos );
float lambert = max (0.0 , dot ( incident , normal )); // Different !

float dist = length ( lightPos - IN.worldPos );
float atten = 1.0 - clamp ( dist / lightRadius , 0.0 , 1.0);

vec3 viewDir = normalize ( cameraPos - IN . worldPos );
vec3 halfDir = normalize ( incident + viewDir );

float rFactor = max (0.0 , dot ( halfDir , normal )); // Different !
float sFactor = pow ( rFactor , 33.0 );

vec3 colour = ( diffuse.rgb * lightColour.rgb );
colour += ( lightColour.rgb * sFactor ) * 0.33;
fragColour = vec4 ( colour * atten * lambert , diffuse.a );
//ambient
fragColour.rgb += ( diffuse.rgb * lightColour.rgb ) * 0.1;
}
    else 
    {
        // else, use ambient light so scene isn't completely dark outside the spotlight.
        fragColour = vec4(0.0,0.0,0.0, 1.0);
    }

}