#pragma once
#include <vector>
#include "../Actor/ActorBase.h"
class ModelMaterial;
class ModelRenderer;

class Mist : public ActorBase
{
public:
	// コンストラクタ
	Mist(void);
	// デストラクタ
	~Mist(void);
	void Init(void) override;
	void Update(void) override;
	void Draw(void) override;
	std::vector<Transform>& GetMistTransforms(void);

private:
	std::vector<Transform> transforms_;
	
	std::unique_ptr<ModelMaterial> material_;
	std::vector<std::unique_ptr<ModelRenderer>> renderer_;

	float time_;
};

