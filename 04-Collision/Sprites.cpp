#include "Sprites.h"
#include "Game.h"
#include "debug.h"

CSprite::CSprite(int id, int left, int top, int right, int bottom, LPDIRECT3DTEXTURE9 tex, DWORD TimeAnimation, int totalFrames, int collum, int row)
{
	this->id = id;
	this->left = left;
	this->top = top;
	this->right = right;
	this->bottom = bottom;
	this->texture = tex;
	this->timeAnimation = TimeAnimation;
	this->totalFrames = totalFrames - 1;
	this->column = collum;
	this->row = row;
	this->frameWidth = this->right / this->column;
	this->frameHeight = this->bottom / this->row;
	this->currentFrame = 0;
	this->timeAccumulated = 0;
}

CSprites * CSprites::__instance = NULL;

CSprites *CSprites::GetInstance()
{
	if (__instance == NULL) __instance = new CSprites();
	return __instance;
}

void CSprite::Draw(float x, float y, int alpha)
{
	CGame * game = CGame::GetInstance();
	game->Draw(x, y, texture, left, top, right, bottom, alpha);
}



void CSprite::DrawFrame(int idFrame, float X, float Y, int Column, float FrameWidth, float Frameheight, int row, int alpha, int R, int G, int B)
{
	spriteHandler = CGame::GetInstance()->GetSpriteHandler();
	RECT r;
	if (row == 1)
	{
		r.left = (idFrame % Column) * FrameWidth;
		r.top = (idFrame / Column) * Frameheight;
		r.right = r.left + FrameWidth;
		r.bottom = r.top + Frameheight;
	}
	else
	{
		r.left = (idFrame % column) * FrameWidth;
		r.top = (int)(idFrame / ((this->totalFrames + 1) / row)) * Frameheight;
		r.right = r.left + FrameWidth;
		r.bottom = r.top + Frameheight;
	}
	
	D3DXVECTOR3 p(trunc(X), trunc(Y), 0);
	spriteHandler->Draw(texture, &r, NULL, &p, D3DCOLOR_ARGB(alpha, R, G, B));
}

void CSprite::Update(DWORD dt)
{
	timeAccumulated += dt;
	if (timeAccumulated >= timeAnimation)
	{
		timeAccumulated -= timeAnimation;
		this->Next();
	}
}

void CSprite::Next()
{
	currentFrame++;
	if (currentFrame > totalFrames)
		currentFrame = 0;
}



void CSprites::Add(int id, int left, int top, int right, int bottom, LPDIRECT3DTEXTURE9 tex)
{
	LPSPRITE s = new CSprite(id, left, top, right, bottom, tex, 100);
	sprites[id] = s;
}

LPSPRITE CSprites::Get(int id)
{
	return sprites[id];
}

void CAnimation::Add(int spriteId, DWORD time)
{
	int t = time;
	if (time == 0) t=this->defaultTime;

	LPSPRITE sprite = CSprites::GetInstance()->Get(spriteId);
	LPANIMATION_FRAME frame = new CAnimationFrame(sprite, t);
	frames.push_back(frame);
}

void CAnimation::Render(D3DXVECTOR2 pos, int alpha)
{
	Render(pos.x, pos.y, alpha);
}

void CAnimation::Render(float x, float y, int alpha)
{
	DWORD now = GetTickCount();
	if (currentFrame == -1) 
	{
		currentFrame = 0; 
		lastFrameTime = now;
	}
	else
	{
		DWORD t = frames[currentFrame]->GetTime();
		if (now - lastFrameTime > t)
		{
			currentFrame++;
			lastFrameTime = now;
			if (currentFrame == frames.size()) currentFrame = 0;
		}
		
	}

	frames[currentFrame]->GetSprite()->Draw(x, y, alpha);
}

CAnimations * CAnimations::__instance = NULL;

CAnimations * CAnimations::GetInstance()
{
	if (__instance == NULL) __instance = new CAnimations();
	return __instance;
}

void CAnimations::Add(int id, LPANIMATION ani)
{
	animations[id] = ani;
}

LPANIMATION CAnimations::Get(int id)
{
	return animations[id];
}