#version 400

layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

uniform mat4 mvMatrix;
uniform mat4 mvpMatrix;
uniform mat4 norMatrix;
uniform vec4 lightPos;

uniform float waterLevel;

in vec3 normals[];
in float currentdepth[];

out gs_outputs
{
	float vertDepth;
	vec4 position;
	float diffTerm;
	float specTerm;
	vec2 TexCoord;
	float fogCoordinate;
};


void lightingCalculations(vec4 posn, vec3 normal)
{
	// Lighting calculations
	vec4 posnEye = mvMatrix * posn;
	vec4 normalEye = norMatrix * vec4(normal, 0);
	vec4 lgtVec = normalize(lightPos - posnEye);
	vec4 viewVec = normalize(vec4(-posnEye.xyz, 0));
	vec4 halfVec = normalize(lgtVec + viewVec);
	
	diffTerm = max(dot(lgtVec, normalEye), 0);

	specTerm = (posn.y == waterLevel)?max(dot(halfVec, normalEye), 0.0):0.0;

	fogCoordinate = posnEye.z / posnEye.w; // for fog
}


void main()
{
	
	float maxHeight = gl_in[0].gl_Position.y;
	maxHeight = max(maxHeight, gl_in[1].gl_Position.y);
	maxHeight = max(maxHeight, gl_in[2].gl_Position.y);

	for (int i = 0;  i <= 3; i++) 
	{
		// Current vertex position
		vec4 posn = gl_in[i].gl_Position;

		// Lighting calculations
		vec3 normal = (maxHeight>waterLevel)?normals[i]:normalize(vec3(0.0, 1.0, 0.0));
		lightingCalculations(posn, normal);

		// Texture coordinates
		TexCoord = posn.xz/5;
		
		// Water depth colour variation
		vertDepth = currentdepth[i];

		gl_Position = mvpMatrix * posn;
		position = posn;
		EmitVertex();
	}
	EndPrimitive();
}