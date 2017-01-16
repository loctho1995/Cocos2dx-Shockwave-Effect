#include "LayerEffect.h"

LayerEffect::LayerEffect()
{
	texture = nullptr;
}

LayerEffect::~LayerEffect()
{
	CC_SAFE_RELEASE_NULL(texture);
	CC_SAFE_RELEASE_NULL(textureTemp);
}

bool LayerEffect::init()
{
	if (!Layer::init())
	{
		return false;
	}

	Size size = this->getContentSize();
	
	this->setDefaultProgram();
	
	texture = RenderTexture::create(this->getContentSize().width, this->getContentSize().height);
	texture->getSprite()->setPosition(this->getContentSize().width / 2, this->getContentSize().height / 2);
	texture->getSprite()->getTexture()->setAntiAliasTexParameters();
	texture->retain();

	textureTemp = RenderTexture::create(this->getContentSize().width, this->getContentSize().height);
	textureTemp->getSprite()->setPosition(this->getContentSize().width / 2, this->getContentSize().height / 2);
	textureTemp->getSprite()->getTexture()->setAntiAliasTexParameters();
	textureTemp->retain();

	textCoords[0] = Tex2F(0.0, 0.0);
	textCoords[1] = Tex2F(0.0, 1.0);
	textCoords[2] = Tex2F(1.0, 1.0);
	textCoords[3] = Tex2F(0.0, 0.0);
	textCoords[4] = Tex2F(1.0, 1.0);
	textCoords[5] = Tex2F(1.0, 0.0);

	vertices[0] = Vec2(0, 0);
	vertices[1] = Vec2(0, size.height);
	vertices[2] = Vec2(size.width, size.height);
	vertices[3] = Vec2(0, 0);
	vertices[4] = Vec2(size.width, size.height);
	vertices[5] = Vec2(size.width, 0);

	colors[0] = Color4F::WHITE;
	colors[1] = Color4F::WHITE;
	colors[2] = Color4F::WHITE;
	colors[3] = Color4F::WHITE;
	colors[4] = Color4F::WHITE;
	colors[5] = Color4F::WHITE;

	this->scheduleUpdate();
	
	return true;
}

void LayerEffect::update(float dt)
{
	Node::update(dt);
	
	for (auto child : childrenEffect)
	{
		if(child->isEffectEnable())
			child->update(dt);
	}
}

void LayerEffect::setColor(Color3B color)
{
	this->bgColor = Color4F(color);
}

void LayerEffect::setDefaultProgram()
{
	this->setGLProgram(ShaderCache::getInstance()->getGLProgram(GLProgram::SHADER_NAME_POSITION_TEXTURE_COLOR));
}

void LayerEffect::setContentSize(const Size & size)
{
	Layer::setContentSize(size);

	if (texture != nullptr)
	{
		texture->removeFromParentAndCleanup(true);

		texture = RenderTexture::create(size.width, size.height);
		texture->getSprite()->setAnchorPoint(Vec2(0.5, 0.5));
		this->addChild(texture);
	}
}

void LayerEffect::setScale(float scale)
{
	Layer::setScale(scale);

	if (texture != nullptr)
		texture->setScale(scale);

	if (textureTemp != nullptr)
	textureTemp->setScale(scale);
}

void LayerEffect::addChildEffect(EffectNode *child)
{
	child->retain();
	
	childrenEffect.pushBack(child);
}

void LayerEffect::addChildWithoutEffect(Node *node)
{
	node->setTag(LAYER_EFFECT_CHILD__NONE_EFFECT_TAG);
	this->addChild(node);
}

void LayerEffect::visit(Renderer *renderer, const Mat4 &transform, uint32_t flags)
{
	texture->beginWithClear(bgColor.r, bgColor.g, bgColor.b, bgColor.a);
	
	// quick return if not visible. children won't be drawn.
	if (!_visible)
	{
		return;
	}

	uint32_t rflag = processParentFlags(transform, flags);

	// IMPORTANT:
	// To ease the migration to v3.0, we still support the Mat4 stack,
	// but it is deprecated and your code should not rely on it
	_director->pushMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW);
	_director->loadMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW, _modelViewTransform);
	
	bool visibleByCamera = isVisitableByVisitingCamera();

	int i = 0;

	if (!_children.empty())
	{
		sortAllChildren();
		// draw children zOrder < 0
		
		for (; i < _children.size(); i++)
		{
			auto node = _children.at(i);
			
			if (node->getTag() == LAYER_EFFECT_CHILD__NONE_EFFECT_TAG)
				continue;

			if (node && node->getLocalZOrder() < 0)
				node->visit(renderer, _modelViewTransform, rflag);
			else
				break;
		}
		// self draw
		if (visibleByCamera)
			this->draw(renderer, _modelViewTransform, rflag);

		for (auto it = _children.cbegin() + i; it != _children.cend(); ++it)
		{
			if ((*it)->getTag() == LAYER_EFFECT_CHILD__NONE_EFFECT_TAG)
				continue;

			(*it)->visit(renderer, _modelViewTransform, rflag);
		}
	}

	//all child drawn on texture
	texture->end();
	
	textureTemp->begin();
	texture->getSprite()->visit(renderer, transform, rflag);
	textureTemp->end();	
	Director::getInstance()->getRenderer()->render();
	
	textureTemp->getSprite()->visit(renderer, transform, rflag);

	for (auto child : childrenEffect)
	{
		if(child->isEffectEnable())
		{
			texture->begin();
			//apply children effect to texture
			childrenCommand.init(this->getGlobalZOrder());
			childrenCommand.func = CC_CALLBACK_0(LayerEffect::onChilrenEffectDraw, this, child);
			renderer->addCommand(&childrenCommand);
			texture->end();

			//copy to texture
			textureTemp->begin();
			texture->getSprite()->visit(renderer, transform, rflag);
			textureTemp->end();

			Director::getInstance()->getRenderer()->render();
		}
	}
	
	texture->getSprite()->visit(renderer, transform, rflag);

	for (size_t i = 0; i < _children.size(); i++)
	{
		auto child = _children.at(i);

		if (child->getTag() == LAYER_EFFECT_CHILD__NONE_EFFECT_TAG)
		{
			child->visit(renderer, transform, rflag);
		}
	}

	_director->popMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW);
}

void LayerEffect::removeEffect(EffectNode *node)
{
	node->release();
	
	childrenEffect.eraseObject(node);
}

void LayerEffect::draw(Renderer *renderer, const Mat4 &transform, uint32_t flags)
{
	Layer::draw(renderer, transform, flags);
}

void LayerEffect::onChilrenEffectDraw(EffectNode *node)
{
	auto p = node->getGLProgramState()->getGLProgram();
	p->use();
	p->setUniformsForBuiltins();
	
	node->setupUniforms();
	
	GL::enableVertexAttribs(GL::VERTEX_ATTRIB_FLAG_POS_COLOR_TEX);
	GL::bindTexture2D(textureTemp->getSprite()->getTexture()->getName());
	
	glVertexAttribPointer(GLProgram::VERTEX_ATTRIB_POSITION, 2, GL_FLOAT, GL_FALSE, 0, vertices);
	glVertexAttribPointer(GLProgram::VERTEX_ATTRIB_TEX_COORD, 2, GL_FLOAT, GL_FALSE, 0, textCoords);
	glVertexAttribPointer(GLProgram::VERTEX_ATTRIB_COLOR, 4, GL_FLOAT, GL_FALSE, 0, colors);
	
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	glDrawArrays(GL_TRIANGLES, 0, (GLsizei)6);
}