

#ifndef _SPRITE_RIM_LIGHT_DEF_H__
#define _SPRITE_RIM_LIGHT_DEF_H__

#include "cocos2d.h"

typedef cocos2d::Sprite SpriteRimLightBase;
class SpriteRimLight : public SpriteRimLightBase
{
private:
	~SpriteRimLight();
	SpriteRimLight();

public:
	void setRimColor(float r, float g, float b);
    void setBlurSize(float f);
    static SpriteRimLight* create(const char *pszFileName);

private:
    bool initWithTexture(cocos2d::Texture2D* texture, const cocos2d::Rect&  rect);
    virtual void draw(cocos2d::Renderer *renderer, const kmMat4 &transform, bool transformUpdated) override;
    void initProgram();

protected:
    void onDraw(const kmMat4 &transform, bool transformUpdated);

private:
	static cocos2d::GLProgram* getSharedProgram();

private:
    int       _blurRadius;
    cocos2d::Point     _pixelSize;

    int       _samplingRadius;
    //gaussian = cons * exp( (dx*dx + dy*dy) * scale);
    float     _scale;
    float     _cons;
    float     _weightSum;

    GLuint pixelSizeLocation;
    GLuint coefficientLocation;
	GLuint rimLightLocation;

	float _rimRed;
	float _rimGreen;
	float _rimBlue;

    cocos2d::CustomCommand _customCommand;
};


#endif