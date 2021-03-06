

#ifndef _RIM_LIGHT_NODE_DEF_H__
#define _RIM_LIGHT_NODE_DEF_H__

//#Rimlight node for cocos2d-x 3.0
//eminem

#include "cocos2d.h"

class SpriteRimLight;

typedef cocos2d::Node RimLightNodeBase;
class RimLightNode:public RimLightNodeBase
{
private:
	~RimLightNode();
	RimLightNode();

public:
	static RimLightNode* create(const char *pszPathName);
	void setRimLight(float r, float g, float b);

private:
	bool initWithFileName(const char *pszPathName);
	cocos2d::Node* createLight(const char *pszPathName, float r, float g, float b);

private:
	float _ambs[3];
	cocos2d::Node *_light;
	cocos2d::Sprite *_origin;
	std::string _spritePathName;
};

#endif