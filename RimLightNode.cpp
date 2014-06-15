
#include "RimLightNode.h"
#include "SpriteRimLight.h"

using namespace cocos2d;

#define _Delete(a)	\
	if(a)\
	{\
		a->removeFromParentAndCleanup(true);\
		a = nullptr;\
	}

RimLightNode::RimLightNode():
	_light(nullptr),
	_origin(nullptr)
{
	for(int i=0;i<sizeof(_fambs)/sizeof(_fambs[0]);++i){
		_fambs[i] = 0;
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
	do{
		CC_BREAK_IF ( ! RimLightNodeBase::init() );
		_Delete(_light)
		_Delete(_origin)

		_origin = Sprite::create(pszPathName);
		addChild(_origin, 1);

		_spritePath = pszPathName;
		rv = true;
	}while(0);
	return rv;
}

void RimLightNode::setRimLight(float r, float g, float b)
{
	if( ! _light || r != _fambs[0] || g != _fambs[1] || b != _fambs[2]){
		_Delete(_light);
		_fambs[0] = r;
		_fambs[1] = g;
		_fambs[2] = b;
		_light = createLight(_spritePath.c_str(), r,g,b);
		addChild(_light,0);
	}
}

cocos2d::Node* RimLightNode::createLight(const char *pszPath, float r, float g, float b)
{
	SpriteRimLight *light = SpriteRimLight::create(pszPath);
	light->setRimColor(r,g,b);
	Size csz = light->getContentSize();
	int width = int(csz.width + 0.5f);
	int height= int(csz.height+ 0.5f);
	light->setPosition(ccp(csz.width * 0.5f, csz.height * 0.5f));
	RenderTexture *tex =  RenderTexture::create(width, height);
	tex->setKeepMatrix(true);
	tex->begin();
	light->visit();
	tex->end();
	return tex;
}