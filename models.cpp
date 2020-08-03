//Lamberts Model (Vertex & Fragment Shader):
#version 400

in vec4 VertexPosition;
in vec4 VertexNormal;
out vec4 vVertexPosition;
out vec4 vVertexNormal;

uniform vec4 Position; 
uniform vec3 Ld;  
uniform vec3 Kd; 

uniform mat4 ModelViewMatrix;
uniform mat4 NormalMatrix;
uniform mat4 ProjectionMatrix;
uniform mat4 MVP;

void main()
{
	vVertexPosition = ModelViewMatrix * VertexPosition;
	vVertexNormal =	normalize(NormalMatrix *  VertexNormal);
	gl_Position = MVP * VertexPosition;
}

#version 400

in vec4 vVertexPosition;
in vec4 vVertexNormal;

uniform vec4 Position; 
uniform vec3 Ld;  
uniform vec3 Kd; 

out vec4 FragColor;

void main()
{
   vec4 L = normalize(Position - vVertexPosition);   
   vec4 Idiff = vec4(Ld,1.0) * max(dot(vVertexNormal,L), 0.0);  
   Idiff = clamp(Idiff, 0.0, 1.0); 

   FragColor = Idiff;
}

//Phong Model (Vertex & Fragment Shader):
#version 400

in vec4 VertexPosition;
in vec4 VertexNormal;
out vec4 vVertexPosition;
out vec4 vVertexNormal;

uniform vec4 Position; 
uniform vec3 La;
uniform vec3 Ld; 
uniform vec3 Ls; 

uniform vec3 Ka; 
uniform vec3 Kd; 
uniform vec3 Ks;
uniform float Shininess;

uniform mat4 ModelViewMatrix;
uniform mat4 NormalMatrix;
uniform mat4 ProjectionMatrix;
uniform mat4 MVP;

void main()
{
	vVertexPosition = ModelViewMatrix * VertexPosition;
	vVertexNormal =	NormalMatrix *  VertexNormal;
	gl_Position = MVP * VertexPosition;
}


#version 400

in vec4 vVertexPosition;
in vec4 vVertexNormal;

uniform vec4 Position; 
uniform vec3 La;
uniform vec3 Ld; 
uniform vec3 Ls; 

uniform vec3 Ka; 
uniform vec3 Kd; 
uniform vec3 Ks;
uniform float Shininess;

out vec4 FragColor;

void main()
{
	vec3 tnorm = normalize(vec3(vVertexNormal));
	vec3 s = normalize(vec3(Position -vVertexPosition));
	vec3 v = normalize(-vVertexPosition.xyz);
	vec3 r = reflect( -s, tnorm );
	vec3 ambient = La * Ka;
	float sDotN = max(dot(s,tnorm), 0.0 );
	vec3 diffuse = Ld * Kd * sDotN;
	vec3 spec = vec3(0.0);
	if( sDotN > 0.0 )
		spec = Ls * Ks *pow( max(dot(r,v), 0.0 ), Shininess );

	vec3 LightIntensity = ambient + diffuse + spec;
   FragColor = vec4(LightIntensity, 1.0);
}


//Blinn Phong Model (Vertex & Fragment Shader):
#version 400

in vec4 VertexPosition;
in vec4 VertexNormal;
out vec4 vVertexPosition;
out vec4 vVertexNormal;

uniform vec4 Position; 
uniform vec3 La;
uniform vec3 Ld; 
uniform vec3 Ls; 

uniform vec3 Ka; 
uniform vec3 Kd; 
uniform vec3 Ks;
uniform float Shininess;

uniform mat4 ModelViewMatrix;
uniform mat4 NormalMatrix;
uniform mat4 ProjectionMatrix;
uniform mat4 MVP;

void main()
{
	vVertexPosition = ModelViewMatrix * VertexPosition;
	vVertexNormal =	NormalMatrix *  VertexNormal;
	gl_Position = MVP * VertexPosition;
}


#version 400

in vec4 vVertexPosition;
in vec4 vVertexNormal;

uniform vec4 Position; 
uniform vec3 La;
uniform vec3 Ld; 
uniform vec3 Ls; 

uniform vec3 Ka; 
uniform vec3 Kd; 
uniform vec3 Ks;
uniform float Shininess;

out vec4 FragColor;

void main()
{
	vec3 tnorm = normalize(vec3(vVertexNormal));
	vec3 s = normalize(vec3(Position-vVertexPosition));
	vec3 v = normalize(-vVertexPosition.xyz);
	vec3 h = normalize(v+s);
	
	vec3 ambient = La * Ka;
	
	float hDotN = max(dot(s,tnorm), 0.0 );

	vec3 diffuse = Ld * Kd * hDotN;
	
	vec3 spec = vec3(0.0);
	if( hDotN > 0.0 )
		spec = Ls * Ks *pow( max(dot(h,tnorm), 0.0 ), Shininess );

	vec3 LightIntensity = ambient + diffuse + spec;
	FragColor = vec4(LightIntensity, 1.0);
}

//Cook Torrance Model (Vertex & Fragment Shader):

#version 400

in vec4 VertexPosition;
in vec4 VertexNormal;

out vec4 vVertexPosition;
out vec4 vVertexNormal;

uniform vec4 Position; 
uniform float Shininess;

uniform mat4 ModelViewMatrix;
uniform mat4 NormalMatrix;
uniform mat4 ProjectionMatrix;
uniform mat4 MVP;

void main ()
{
	vVertexNormal = NormalMatrix*VertexNormal;
	vVertexPosition = ModelViewMatrix*VertexPosition;
	gl_Position = MVP * VertexPosition;
}

#version 400

in vec4 vVertexPosition;
in vec4 vVertexNormal;

uniform vec4 Position; 
uniform float Shininess;

vec3 uRealRefractIdx = vec3(1.5,0.1,0.2);
vec3 uImagRefractIdx = vec3(1.5,0.2,0.2);

out vec4 FragColor;

void main()
{
    vec3 lightDir = normalize(vVertexPosition.xyz - Position.xyz);
    vec3 normal  = normalize(vVertexNormal.xyz);
    vec3 viewDir = normalize(-vVertexPosition.xyz);
    vec3 halfDir = normalize(-(lightDir + viewDir) / 2.0);

    vec3 two = vec3(2.0, 2.0, 2.0);
    vec3 realRefractIdxSquared = pow(uRealRefractIdx, two);
    vec3 imagRefractIdxSquared = pow(uImagRefractIdx, two);
    vec3 refrIdxSqSum = realRefractIdxSquared + imagRefractIdxSquared;
    float incidentCos = clamp(dot(normal, halfDir), 0.0, 1.0);
    float incidentCosSq = pow(incidentCos, 2.0);
    vec3 twiceNormalIncidentCos = 2.0 * uRealRefractIdx * incidentCos;
    vec3 fresPt1 = (refrIdxSqSum * incidentCosSq - twiceNormalIncidentCos + 1.0) /
                   (refrIdxSqSum * incidentCosSq + twiceNormalIncidentCos + 1.0);
    vec3 fresPt2 = (refrIdxSqSum - twiceNormalIncidentCos + incidentCosSq) /
                   (refrIdxSqSum + twiceNormalIncidentCos + incidentCosSq);
    vec3 fresnel = (pow(fresPt1, two) + pow(fresPt2, two)) / 2.0;

    float nDotH = clamp(dot(normal, halfDir), 0.0, 1.0);
    float nDotV = clamp(dot(normal, -viewDir), 0.0, 1.0);
    float nDotL = clamp(dot(normal, -lightDir), 0.0, 1.0);
    float vDotH = clamp(dot(-viewDir, halfDir), 0.0, 1.0);
    float geoAttenTerm1 = (2.0 * nDotH * nDotV) / vDotH;
    float geoAttenTerm2 = (2.0 * nDotH * nDotL) / vDotH;
    float masking = min(1.0, min(geoAttenTerm1, geoAttenTerm2));
    
    float distribution = pow(nDotH, Shininess);

    vec3 cookTorrance = ((fresnel*distribution*masking)/(2.0*nDotL*nDotV));

    FragColor = vec4(cookTorrance.xyz, 1.0);
}

//Ward Model (Vertex & Fragment Shader):

#version 400

in vec4 VertexPosition;
in vec4 VertexNormal;
out vec4 vVertexPosition;
out vec4 vVertexNormal;

uniform vec4 Position; 
uniform vec3 La;

uniform mat4 ModelViewMatrix;
uniform mat4 NormalMatrix;
uniform mat4 ProjectionMatrix;
uniform mat4 MVP;

void main ()
{
	vVertexNormal = NormalMatrix*VertexNormal;
	vVertexPosition = ModelViewMatrix*VertexPosition;
	gl_Position = MVP * VertexPosition;
}

#version 400

in vec4 vVertexPosition;
in vec4 vVertexNormal;
uniform vec4 Position; 
uniform vec3 La;

float alphaX=0.5, alphaY=0.13; // asinotropic roughness
float pD = 0.6;
float pS = 0.8;
float PI = 3.14159;

out vec4 FragColor;

void main ()
{
	vec3 color = vec3(0.0,0.9,0.5);
	vec3 L = normalize(vVertexPosition.xyz - Position.xyz);
    vec3 normal = normalize(vVertexNormal.xyz);
    vec3 eyeDir = normalize(-vVertexPosition.xyz);
	vec3 e= vec3(1.0,1.0,0.0);
    vec3 tangent = normalize(cross(normal,e));
    vec3 bitangent = normalize(cross(normal,tangent));

    float NdotL = dot(normal, L);
    float diffuse = pD * max(NdotL, 0.0);

    vec3 halfwayVector = normalize(-(L + eyeDir)/2.0);
    float NdotV = dot(normal, eyeDir);
    float HdotN = dot(halfwayVector, normal);
    float VdotN = dot(eyeDir, normal);
    float HdotTAlphaX = dot(halfwayVector, tangent) / alphaX;
    float HdotBAlphaY = dot(halfwayVector, bitangent) / alphaY;

    float exponent = exp(-2.0*((HdotTAlphaX * HdotTAlphaX)+(HdotBAlphaY*HdotBAlphaY))/(1.0+HdotN));
    float specular = pS * sqrt(max(0.0, NdotL / NdotV)) * exponent;
      
    vec3 finalValue = color*(diffuse + specular + La);
    
    FragColor = vec4(finalValue, 1.0);
}
