

// April. 27th. 2o14
// Specified by eminem

#ifdef GL_ES
precision mediump float;
#endif

varying vec4 v_fragmentColor;
varying vec2 v_texCoord;

uniform sampler2D CC_Texture0;

uniform vec4 gaussianCoefficient;
uniform vec2 onePixelSize;
uniform vec4 rimLight;

#define TranslatedColor vec4(rimLight.r * step(0, thisV.a), rimLight.g * step(0, thisV.a), rimLight.b * step(0,thisV.a), thisV.a)
//#define TranslatedColor vec4(0, 1 * step(0, thisV.a), 0, thisV.a)

void main() {
	if(gaussianCoefficient.x > 0.0) {
	    vec4 sum = vec4(0.0);
	    vec2 offset;
	    float weight;    
	    float squareX;
	    vec4 thisV;

	    for(float dx = 0.0; dx <= gaussianCoefficient.x; dx += 1.0) {
	        squareX = dx * dx;
	        weight = gaussianCoefficient.z * exp(squareX * gaussianCoefficient.y);
	        
	        offset.x = -dx * onePixelSize.x;
	        offset.y = 0.0;

	        thisV = texture2D(CC_Texture0, v_texCoord + offset);
	        sum += TranslatedColor * weight;
	        
	        offset.x = dx * onePixelSize.x;

	        thisV = texture2D(CC_Texture0, v_texCoord + offset);
	        sum += TranslatedColor * weight;
	        
	        for(float dy = 1.0; dy <= gaussianCoefficient.x; dy += 1.0) {
	            weight = gaussianCoefficient.z * exp((squareX + dy * dy) * gaussianCoefficient.y);
	            
	            offset.x = -dx * onePixelSize.x;
	            offset.y = -dy * onePixelSize.y;

	            thisV = texture2D(CC_Texture0, v_texCoord + offset);
	            sum += TranslatedColor * weight;

	            offset.y = dy * onePixelSize.y;

	            thisV = texture2D(CC_Texture0, v_texCoord + offset);
	            sum += TranslatedColor * weight;
	            
	            offset.x = dx * onePixelSize.x;

	            thisV = texture2D(CC_Texture0, v_texCoord + offset);
	            sum += TranslatedColor * weight;
	            
	            offset.y = -dy * onePixelSize.y;

	            thisV = texture2D(CC_Texture0, v_texCoord + offset);
	            sum += TranslatedColor * weight;
	        }
	    }

	    thisV = texture2D(CC_Texture0, v_texCoord);
	    sum -= TranslatedColor * gaussianCoefficient.z;
	    sum /= gaussianCoefficient.w;
	    gl_FragColor = sum * v_fragmentColor;
	}
	else {
	    gl_FragColor = texture2D(CC_Texture0, v_texCoord) * v_fragmentColor;
	}
}

