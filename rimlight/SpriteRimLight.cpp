

#include "SpriteRimLight.h"
using namespace cocos2d;


const char *_fragSource =

""
"// April. 27th. 2o14			\n"
"// Specified by eminem			\n"

"#ifdef GL_ES					\n"
"precision mediump float;		\n"
"#endif							\n"

"varying vec4 v_fragmentColor;		\n"
"varying vec2 v_texCoord;			\n"
"uniform sampler2D CC_Texture0;		\n"

"uniform vec4 gaussianCoefficient;	\n"
"uniform vec2 onePixelSize;			\n"
"uniform vec4 rimLight;				\n"

"#define TranslatedColor vec4(rimLight.r * step(0, thisV.a), rimLight.g * step(0, thisV.a), rimLight.b * step(0,thisV.a), thisV.a)		\n"
"//#define TranslatedColor vec4(0, 1 * step(0, thisV.a), 0, thisV.a)																	\n"

"void main() {                                             \n"
"	if(gaussianCoefficient.x > 0.0) {                      \n"
"	    vec4 sum = vec4(0.0);								\n"
"	    vec2 offset;										\n"
"	    float weight;										\n"
"	    float squareX;										\n"
"	    vec4 thisV;											\n"

"	    for(float dx = 0.0; dx <= gaussianCoefficient.x; dx += 1.0) {       \n"
"	        squareX = dx * dx;												\n"
"	        weight = gaussianCoefficient.z * exp(squareX * gaussianCoefficient.y);	\n"
	        
"	        offset.x = -dx * onePixelSize.x;									\n"
"	        offset.y = 0.0;													\n"

"	        thisV = texture2D(CC_Texture0, v_texCoord + offset);			\n"
"	        sum += TranslatedColor * weight;								\n"
	        
"	        offset.x = dx * onePixelSize.x;									\n"

"	        thisV = texture2D(CC_Texture0, v_texCoord + offset);			\n"
"	        sum += TranslatedColor * weight;								\n"
	        
"	        for(float dy = 1.0; dy <= gaussianCoefficient.x; dy += 1.0) {   \n"
"	            weight = gaussianCoefficient.z * exp((squareX + dy * dy) * gaussianCoefficient.y);	\n"
	            
"	            offset.x = -dx * onePixelSize.x;								\n"
"	            offset.y = -dy * onePixelSize.y;								\n"

"	            thisV = texture2D(CC_Texture0, v_texCoord + offset);		\n"
"	            sum += TranslatedColor * weight;							\n"

"	            offset.y = dy * onePixelSize.y;								\n"

"	            thisV = texture2D(CC_Texture0, v_texCoord + offset);		\n"
"	            sum += TranslatedColor * weight;							\n"
	            
"	            offset.x = dx * onePixelSize.x;								\n"

"	            thisV = texture2D(CC_Texture0, v_texCoord + offset);		\n"
"	            sum += TranslatedColor * weight;							\n"
	            
"	            offset.y = -dy * onePixelSize.y;								\n"

"	            thisV = texture2D(CC_Texture0, v_texCoord + offset);		\n"
"	            sum += TranslatedColor * weight;							\n"
"	        }																\n"
"	    }																	\n"

"	    thisV = texture2D(CC_Texture0, v_texCoord);							\n"
"	    sum -= TranslatedColor * gaussianCoefficient.z;						\n"
"	    sum /= gaussianCoefficient.w;										\n"
"	    gl_FragColor = sum * v_fragmentColor;								\n"
"	}																		\n"
"	else {        \n"
"	    gl_FragColor = texture2D(CC_Texture0, v_texCoord) * v_fragmentColor;\n"
"	}																		\n"
"}																			\n"
;

#define _DefaultBlurSize	25

SpriteRimLight::SpriteRimLight()
{
	//TODO
	setBlurSize(_DefaultBlurSize);
	setRimColor(0, 1, 0);
}

SpriteRimLight::~SpriteRimLight()
{
}

SpriteRimLight* SpriteRimLight::create(const char *pszFileName)
{
    SpriteRimLight* pRet = new SpriteRimLight();
    if (pRet && pRet->initWithFile(pszFileName))
    {
        pRet->autorelease();
		return pRet;
    }
	delete pRet;
	return nullptr;
}

bool SpriteRimLight::initWithTexture(Texture2D* texture, const Rect& rect)
{
    _blurRadius = 0;
    if( SpriteRimLightBase::initWithTexture(texture, rect) ) 
    {
        auto s = getTexture()->getContentSizeInPixels();

        _pixelSize = Point(1/s.width, 1/s.height);
        _samplingRadius = 0;
        initProgram();

		setBlurSize(_DefaultBlurSize);
        return true;
    }
    return false;
}

void SpriteRimLight::initProgram()
{
	auto program = getGaussShader();    
	setShaderProgram(program);
    CHECK_GL_ERROR_DEBUG();
    pixelSizeLocation = program->getUniformLocation("onePixelSize");
    coefficientLocation = program->getUniformLocation("gaussianCoefficient");
	rimLightLocation = program->getUniformLocation("rimLight");

    CHECK_GL_ERROR_DEBUG();
}

void SpriteRimLight::draw(Renderer *renderer, const kmMat4 &transform, bool transformUpdated)
{
	_customCommand.init(_globalZOrder);
    _customCommand.func = CC_CALLBACK_0(SpriteRimLight::onDraw, this, transform, transformUpdated);
    renderer->addCommand(&_customCommand);
}

void SpriteRimLight::onDraw(const kmMat4 &transform, bool transformUpdated)
{
    GL::enableVertexAttribs(cocos2d::GL::VERTEX_ATTRIB_FLAG_POS_COLOR_TEX );
    BlendFunc blend = getBlendFunc();
    GL::blendFunc(blend.src, blend.dst);

    auto program = getShaderProgram();
    program->use();
    program->setUniformsForBuiltins(transform);
    program->setUniformLocationWith2f(pixelSizeLocation, _pixelSize.x, _pixelSize.y);
    program->setUniformLocationWith4f(coefficientLocation, _samplingRadius, _scale,_cons,_weightSum);
	program->setUniformLocationWith4f(rimLightLocation, _rimRed, _rimGreen, _rimBlue, 0);
    GL::bindTexture2D( getTexture()->getName());
    #define kQuadSize sizeof(_quad.bl)
    size_t offset = (size_t)&_quad;
    int diff = offsetof( V3F_C4B_T2F, vertices);
    glVertexAttribPointer(GLProgram::VERTEX_ATTRIB_POSITION, 3, GL_FLOAT, GL_FALSE, kQuadSize, (void*) (offset + diff));
    diff = offsetof( V3F_C4B_T2F, texCoords);
    glVertexAttribPointer(GLProgram::VERTEX_ATTRIB_TEX_COORDS, 2, GL_FLOAT, GL_FALSE, kQuadSize, (void*)(offset + diff));
    diff = offsetof( V3F_C4B_T2F, colors);
    glVertexAttribPointer(GLProgram::VERTEX_ATTRIB_COLOR, 4, GL_UNSIGNED_BYTE, GL_TRUE, kQuadSize, (void*)(offset + diff));
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    CC_INCREMENT_GL_DRAWN_BATCHES_AND_VERTICES(1,4);
}

void SpriteRimLight::setBlurSize(float f)
{
    if(_blurRadius == (int)f)
	{
        return;
	}

    _blurRadius = (int)f;

    _samplingRadius = _blurRadius;
    if (_samplingRadius > 10)
    {
        _samplingRadius = 10;
    }
    if (_blurRadius > 0)
    {
        float sigma = _blurRadius / 2.0f;
        _scale = -0.5f / (sigma * sigma);
        _cons = -1.0f * _scale / 3.141592f;
        _weightSum = -_cons;

        float weight;
        int squareX;
        for(int dx = 0; dx <= _samplingRadius; ++dx)
        {
            squareX = dx * dx;
            weight = _cons * exp(squareX * _scale);
            _weightSum += 2.0 * weight;
            for (int dy = 1; dy <= _samplingRadius; ++dy)
            {
                weight = _cons * exp((squareX + dy * dy) * _scale);
                _weightSum += 4.0 * weight;
            }
        }
    }
    log("_blurRadius:%d",_blurRadius);
}

void SpriteRimLight::setRimColor(float r, float g, float b)
{
	_rimRed =  r;
	_rimGreen = g;
	_rimBlue = b;
}

GLProgram* SpriteRimLight::getGaussShader()
{
	static GLProgram *sProgram = nullptr;
	if( ! sProgram )
	{
		sProgram = new GLProgram();
		sProgram->initWithByteArrays(ccPositionTextureColor_vert, _fragSource);
		sProgram->bindAttribLocation(GLProgram::ATTRIBUTE_NAME_POSITION, GLProgram::VERTEX_ATTRIB_POSITION);
		sProgram->bindAttribLocation(GLProgram::ATTRIBUTE_NAME_COLOR, GLProgram::VERTEX_ATTRIB_COLOR);
		sProgram->bindAttribLocation(GLProgram::ATTRIBUTE_NAME_TEX_COORD, GLProgram::VERTEX_ATTRIB_TEX_COORDS);
		CHECK_GL_ERROR_DEBUG();
		sProgram->link();
		CHECK_GL_ERROR_DEBUG();
		sProgram->updateUniforms();
		CHECK_GL_ERROR_DEBUG();
	}
	return sProgram;
}
