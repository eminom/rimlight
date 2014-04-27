
#include "RimLightNode.h"
#include "SpriteRimLight.h"

using namespace cocos2d;

RimLightNode::RimLightNode():
	_light(nullptr),
	_origin(nullptr)
{
	
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
	do{
		CC_BREAK_IF ( ! RimLightNodeBase::init() );
		CCAssert( ! _light, "must be nullptr");
		CCAssert( ! _origin, "must be nullptr");
		_origin = Sprite::create(pszPathName);
		_light = SpriteRimLight::create(pszPathName);
		addChild(_origin, 1);
		addChild(_light, 0);
		rv = true;
	}while(0);
	return rv;
}

void RimLightNode::setRimLight(float r, float g, float b)
{
	_light->setRimColor(r,g,b);
}