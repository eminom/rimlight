

#include "SpriteRimLight.h"
using namespace cocos2d;

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
    GLchar * fragSource = (GLchar*) String::createWithContentsOfFile(
                                FileUtils::getInstance()->fullPathForFilename("Shaders/example_Blur.fsh").c_str())->getCString();  
    auto program = new GLProgram();
    program->initWithByteArrays(ccPositionTextureColor_vert, fragSource);
    setShaderProgram(program);
    program->release();
    
    CHECK_GL_ERROR_DEBUG();
    
    program->bindAttribLocation(GLProgram::ATTRIBUTE_NAME_POSITION, GLProgram::VERTEX_ATTRIB_POSITION);
    program->bindAttribLocation(GLProgram::ATTRIBUTE_NAME_COLOR, GLProgram::VERTEX_ATTRIB_COLOR);
    program->bindAttribLocation(GLProgram::ATTRIBUTE_NAME_TEX_COORD, GLProgram::VERTEX_ATTRIB_TEX_COORDS);

    CHECK_GL_ERROR_DEBUG();
    
    program->link();
    
    CHECK_GL_ERROR_DEBUG();
    
    program->updateUniforms();
    
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
    
    //
    // Attributes
    //
    #define kQuadSize sizeof(_quad.bl)
    size_t offset = (size_t)&_quad;
    
    // vertex
    int diff = offsetof( V3F_C4B_T2F, vertices);
    glVertexAttribPointer(GLProgram::VERTEX_ATTRIB_POSITION, 3, GL_FLOAT, GL_FALSE, kQuadSize, (void*) (offset + diff));
    
    // texCoods
    diff = offsetof( V3F_C4B_T2F, texCoords);
    glVertexAttribPointer(GLProgram::VERTEX_ATTRIB_TEX_COORDS, 2, GL_FLOAT, GL_FALSE, kQuadSize, (void*)(offset + diff));
    
    // color
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