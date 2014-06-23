
#ifndef _XCROSSSPRITE_DEF_HEADER___
#define _XCROSSSPRITE_DEF_HEADER___

// XCrossSprite for Cocos2d-X 3.0
// written by eminem
// May. 2o14

#include "cocos2d.h"

typedef cocos2d::Sprite XCrossSpriteBase;
class XCrossSprite : public XCrossSpriteBase
{
	typedef XCrossSprite this_class;

protected:
	XCrossSprite();

public:
	~XCrossSprite();
	static XCrossSprite* create(const char *file, const char *file1);
	void setOrdinary(bool ordinary);

private:
	bool initWithFiles(const char *file, const char *file1);

private:
	//Overrides
	void draw(cocos2d::Renderer *renderer, const kmMat4 &transform, bool transformUpdated)override;
	void onDraw(const kmMat4 &transform, bool transformUpdated);

private:
	cocos2d::Sprite *_sprite1;
	bool _ordinary;
	cocos2d::CustomCommand _customCommand;

	//Static
private:
	static cocos2d::GLProgram *getCrossedProgram();
	static cocos2d::GLProgram *sharedProgram;
	static GLuint _tex1Location;
};

#endif