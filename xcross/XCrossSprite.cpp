
#include "XCrossSprite.h"

using namespace cocos2d;


const char *vertexSource = 
"													\n\
attribute vec4 a_position;							\n\
attribute vec2 a_texCoord;							\n\
attribute vec4 a_color;								\n\
													\n\
#ifdef GL_ES										\n\
varying lowp vec4 v_fragmentColor;					\n\
varying mediump vec2 v_texCoord;					\n\
#else												\n\
varying vec4 v_fragmentColor;						\n\
varying vec2 v_texCoord;							\n\
#endif												\n\
													\n\
void main()											\n\
{													\n\
    gl_Position = CC_MVPMatrix * a_position;		\n\
	v_fragmentColor = a_color;						\n\
	v_texCoord = a_texCoord;						\n\
}													\n\
";

const char *fragmentSource = 
"											\n\
#ifdef GL_ES								\n\
precision lowp float;						\n\
#endif										\n\
											\n\
varying vec4 v_fragmentColor;				\n\
varying vec2 v_texCoord;					\n\
uniform sampler2D CC_Texture0;				\n\
uniform sampler2D CC_Texture1;				\n\
											\n\
void main()									\n\
{											\n\
	vec4 mskColor = texture2D(CC_Texture1, v_texCoord);										\n\
	vec4 point = texture2D(CC_Texture0, v_texCoord);										\n\
	vec4 color = v_fragmentColor * texture2D(CC_Texture0, v_texCoord);						\n\
	float grey = dot(point.rgb, vec3(0.399, 0.187, 0.214));									\n\
	float alpha2 = point.a * v_fragmentColor.a;												\n\
	float s = step(0.2, mskColor.r);														\n\
	grey = grey * alpha2;																	\n\
	gl_FragColor = color * s;																\n\
}																							\n\
";

GLProgram* XCrossSprite::sharedProgram = NULL;
GLuint XCrossSprite::_tex1Location = -1;

GLProgram* XCrossSprite::getCrossedProgram()
{
	if( ! sharedProgram )
	{
		sharedProgram = new GLProgram();
		bool mustBeTrue = sharedProgram->initWithByteArrays(vertexSource, fragmentSource);
		CCAssert(mustBeTrue, "or else ??");
		sharedProgram->bindAttribLocation(GLProgram::ATTRIBUTE_NAME_POSITION, GLProgram::VERTEX_ATTRIB_POSITION);
		sharedProgram->bindAttribLocation(GLProgram::ATTRIBUTE_NAME_COLOR, GLProgram::VERTEX_ATTRIB_COLOR);
		sharedProgram->bindAttribLocation(GLProgram::ATTRIBUTE_NAME_TEX_COORD, GLProgram::VERTEX_ATTRIB_TEX_COORDS);
		sharedProgram->link();
		sharedProgram->updateUniforms();
		_tex1Location = sharedProgram->getUniformLocation("CC_Texture1");
	}
	return sharedProgram;
}

XCrossSprite::XCrossSprite()
{
	_sprite1 = NULL;
	_ordinary = true;
}

void XCrossSprite::setOrdinary(bool ordinary)
{
	//
	_ordinary = ordinary;
	if( ordinary )
	{
		setShaderProgram(ShaderCache::getInstance()->getProgram(GLProgram::SHADER_NAME_POSITION_TEXTURE_COLOR));
	}
	else
	{
		setShaderProgram(getCrossedProgram());
	}
}

XCrossSprite::~XCrossSprite()
{
	CC_SAFE_RELEASE_NULL(_sprite1);
}

XCrossSprite* XCrossSprite::create(const char *file, const char *file1)
{
	XCrossSprite *rv = new XCrossSprite();
	if(rv&&rv->initWithFiles(file, file1))
	{
		rv->autorelease();
		rv->setOrdinary(false);
		return rv;
	}
	delete rv;
	return NULL;
}

void XCrossSprite::onDraw(const kmMat4 &transform, bool transformUpdated)
{
	GL::enableVertexAttribs( cocos2d::GL::VERTEX_ATTRIB_FLAG_POS_COLOR_TEX);
	GLProgram* program = getShaderProgram();
	program->use();
	program->setUniformsForBuiltins(transform);
    GL::blendFunc(_blendFunc.src, _blendFunc.dst);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, _sprite1->getTexture()->getName());
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, getTexture()->getName());
	//?
	GL::bindTexture2D(getTexture()->getName());	//This cache inccurs problems.
	glUniform1i(_tex1Location, 1);

	const int kQuadSize = sizeof(_quad.bl);
    long offset = (long)&_quad;
    int diff = offsetof( ccV3F_C4B_T2F, vertices);
    glVertexAttribPointer(kCCVertexAttrib_Position, 3, GL_FLOAT, GL_FALSE, kQuadSize, (void*) (offset + diff));
    diff = offsetof( ccV3F_C4B_T2F, texCoords);
    glVertexAttribPointer(kCCVertexAttrib_TexCoords, 2, GL_FLOAT, GL_FALSE, kQuadSize, (void*)(offset + diff));
    diff = offsetof( ccV3F_C4B_T2F, colors);
    glVertexAttribPointer(kCCVertexAttrib_Color, 4, GL_UNSIGNED_BYTE, GL_TRUE, kQuadSize, (void*)(offset + diff));

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    CHECK_GL_ERROR_DEBUG();
    CC_INCREMENT_GL_DRAWS(1);
}

bool XCrossSprite::initWithFiles(const char *file, const char *file1)
{
	_sprite1 = CCSprite::create(file1);		//~And it should success.
	_sprite1->retain();
	return initWithFile(file);
}

void XCrossSprite::draw(Renderer *renderer, const kmMat4 &transform, bool transformUpdated)
{
	if( _ordinary )
	{
		XCrossSpriteBase::draw(renderer, transform, transformUpdated);
		return;
	}

	_customCommand.init(_globalZOrder);
    _customCommand.func = CC_CALLBACK_0(XCrossSprite::onDraw, this, transform, transformUpdated);
    renderer->addCommand(&_customCommand);
}
