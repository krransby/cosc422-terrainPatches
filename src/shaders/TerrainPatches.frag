#version 330

uniform sampler2D tSampler1; // water
uniform sampler2D tSampler2; // grass
uniform sampler2D tSampler3; // rock
uniform sampler2D tSampler4; // snow

uniform float fogEnabled;
uniform float waterLevel;
uniform float snowLevel;
uniform float wireframe;

in gs_outputs
{
	float vertDepth;
    vec4 position;
	float diffTerm;
	float specTerm;
	vec2 TexCoord;
	float fogCoordinate;
};


float fogFact(float coordinate) 
{
    float fogDensity = 0.009;

    float result;

    result = exp(-pow(fogDensity * coordinate, 2.0));

    result = 1.0 - clamp(result, 0.0, 1.0);
	return result;
}


float lerp(float t, float a, float b) 
{
	return clamp((t - a) / (b - a), 0, 1);
}


vec4 weights(float height)
{
	// 0, H/4, H/2, 3H/4, Hmax
	float h[5] = { 0, waterLevel, (snowLevel / 2), (3 * snowLevel / 4), snowLevel };

	float bamt = 1.33f; // blend amount

	float rock = lerp(height, h[2], h[2] * bamt);
	float snow = lerp(height, h[3], h[3] * bamt);
	float grass = (lerp(height, h[1], h[2]) <= 1)?1.0:0.0;
    float water = (height <= h[1])?1.0:0.0;
		
	return vec4(water, grass-rock, rock-snow, snow);
}


void main() 
{   
    // Do not output any textures / lighting / fog if in wireframe mode
    if (wireframe == 0.0)
    {
        // Texturing
        vec4 texColor1 = texture(tSampler1, TexCoord); //Get colour from water texture
        vec4 texColor2 = texture(tSampler2, TexCoord); //Get colour from grass texture
        vec4 texColor3 = texture(tSampler3, TexCoord); //Get colour from rock texture
        vec4 texColor4 = texture(tSampler4, TexCoord); //Get colour from snow texture

        vec4 texWgts = weights(position.y);

        vec4 material = (texWgts.x * texColor1
                        + texWgts.y * texColor2 
                        + texWgts.z * texColor3 
                        + texWgts.w * texColor4);

    
        // water depth
        if (texWgts.x == 1) 
        {
            material = texColor1 * vec4(max(1-(vertDepth*0.25), 0.4));      
        }
    

        // Lighting
        vec4 white = vec4(1.0);
        vec4 grey = vec4(0.4);
        float shininess = 100.0;
        vec4 ambOut = grey * material;
        vec4 diffOut = material * diffTerm;
        vec4 specOut = white * pow(specTerm, shininess);

        gl_FragColor = ambOut + diffOut + specOut;

        // Fog
        if (fogEnabled == 1.0) 
        {
            gl_FragColor = mix(gl_FragColor, white, fogFact(fogCoordinate));
        }
    } 
    else 
    {
        gl_FragColor = vec4(0.0, 0.0, 1.0, 1.0); // clean wireframe view
    }
}
