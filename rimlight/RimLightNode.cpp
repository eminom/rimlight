
#include "RimLightNode.h"
#include "SpriteRimLight.h"

using namespace cocos2d;

#define _Delete(a)	\
	if(a)\
	{\
		a->removeFromParentAndCleanup(true);\
		a = 0;\
	}

RimLightNode::RimLightNode():
	_light(nullptr),
	_origin(nullptr)
{
	for(int i=0;i<sizeof(_ambs)/sizeof(_ambs[0]);++i)
	{
		_ambs[i] = 0;
	}
}

RimLightNode::~RimLightNode()
{
}

RimLightNode* RimLightNode::create(const char *pszPathName)
{
	RimLightNode* rv = new RimLightNode();
	if( rv && rv->initWithFileName(pszPathName))
	{
		rv->autorelease();
		return rv;
	}
	delete rv;
	return nullptr;
}

bool RimLightNode::initWithFileName(const char *pszPathName)
{
	bool rv = false;
	_spritePathName = pszPathName;
	do{
		CC_BREAK_IF ( ! RimLightNodeBase::init() );

		_Delete(_light)
		_Delete(_origin)

		_origin = Sprite::create(pszPathName);
		addChild(_origin, 1);
		rv = true;
	}while(0);
	return rv;
}

void RimLightNode::setRimLight(float r, float g, float b)
{
	if( !_light || r != _ambs[0] || g != _ambs[1] || b != _ambs[2])
	{
		_ambs[0] = r;
		_ambs[1] = g;
		_ambs[2] = b;

		_Delete(_light)
		_light = createLight(_spritePathName.c_str(), r,g,b);
		addChild(_light, 0);
	}
}

cocos2d::Node* RimLightNode::createLight(const char *pszPathName, float r, float g, float b)
{
	SpriteRimLight *rimLight = SpriteRimLight::create(pszPathName);
	return rimLight;

	Size contentSize = rimLight->getContentSize();
	int w = int(contentSize.width + 0.5f);
	int h = int(contentSize.height+ 0.5f);
	rimLight->setPosition(ccp(w * 0.5f, h * 0.5f));
	RenderTexture *tex = RenderTexture::create(w,h);
	//tex->setAnchorPoint(Point(0, 0));
    tex->setKeepMatrix(true);
	//tex->setVirtualViewport(ccp(0,0),CCRectMake(0,0,w,h),CCRectMake(0,0,w,h));

	tex->begin();
	rimLight->visit();
	tex->end();
	//return Sprite::createWithTexture(tex->getSprite()->getTexture());
	return tex;
}