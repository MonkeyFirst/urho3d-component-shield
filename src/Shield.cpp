#include "Urho3DAll.h"
#include "Shield.h"

int Shield::numShields = 0;

void Shield::ShieldField::Free() 
{
	node->Remove();
	sm->Remove();
}

Shield::Shield(Context* context) : LogicComponent(context)
{
	SetUpdateEventMask(USE_FIXEDUPDATE | USE_UPDATE);
}

void Shield::RegisterObject(Context* context) 
{
	context->RegisterFactory<Shield>();
}

void Shield::Start() 
{
	ResourceCache* cache = GetSubsystem<ResourceCache>();

	node = GetNode()->CreateChild("shield", LOCAL);
	//model = node->CreateComponent<StaticModel>();

	// Clone material because we do not won't share one material between others model, when we do animation.
	// if mat do not have individual animation settings in same time with others mat of this type, it my be one common for many models.
	matCloneName = "ShieldMaterial_" + String(Shield::numShields);

	mat = cache->GetResource<Material>("Materials/ShieldMaterial.xml")->Clone(matCloneName);
	model = cache->GetResource<Model>("Models/shield.mdl");
	
	defaultColor = Color(0.2f, 0.7f, 1.0f);

	ShieldColor(defaultColor);

	SubscribeToEvent(E_SHIELDDAMAGE, URHO3D_HANDLER(Shield, HandleGotDamage));

	// incriment count of shields in scene, this is for matCloneNaming	
	Shield::numShields++;
}

void Shield::Stop() 
{

	Shield::numShields--;
}

void Shield::Update(float timeStep) 
{
	// calculate each shield field
	for (unsigned int i = 0; i<shields.Size(); i++)
	{
		if (shields[i].second_.isShieldActive)
			if (shields[i].second_.shieldVisibleTime > 0.0f)
			{
				float phase = Lerp(1.0f, -0.9f, sinf((shields[i].second_.shieldVisibleTime / shieldActiveTime)));
				shields[i].second_.mat->SetShaderParameter("Phase", Variant(phase));
				shields[i].second_.mat->SetShaderParameter("HitPower", Variant(shieldHitPower));
			}
	}
}

void Shield::FixedUpdate(float timeStep) 
{
	for (unsigned int i = 0; i<shields.Size(); i++) 
	{
		if (shields[i].second_.isShieldActive)
		{
			if (shields[i].second_.shieldVisibleTime > 0.0f)
			{
				shields[i].second_.shieldVisibleTime -= timeStep;

			}
			else 
			{
				shields[i].second_.isShieldActive = false;
			}
		}
		else
		{
			shields[i].second_.Free();
			shields.Erase(i);
		}
	}
}


void Shield::HandleGotDamage(StringHash eventType, VariantMap& eventData) 
{
	using namespace ShieldDamage;
	Node* hitNode = (Node*)eventData[P_NODE].GetVoidPtr();

	// Check is this event for this node?
	if (hitNode != node) return;
	Vector3 wp = node->GetWorldPosition();

	Shield* shield = (Shield*)eventData[P_SHIELD].GetVoidPtr();
	Vector3 point = eventData[P_POINT].GetVector3();
	Vector3 normal = eventData[P_NORMAL].GetVector3();
	Vector3 from = eventData[P_FROM].GetVector3();
	float hitPower = eventData[P_FIREPOWER].GetFloat();

	// check for near hit with exesting fields hits (hit's theshold)
	for (unsigned int i = 0; i < shields.Size(); i++)
	{
		Vector3 impNormal = shields[i].second_.impactNormal;
		float l = (wp - from).Normalized().Length();
		float difference = impNormal.DotProduct((wp-from).Normalized());
		if ((difference) > 0.95f) 
		{
			// if we got a hit to already hitted region restore time effect
			shields[i].second_.shieldVisibleTime = shieldActiveTime;
			return;
		}
	}
	
	// buid new shield field oriented into impact point
	int numShield = shields.Size();
	Pair<Node*, ShieldField> pair;
	pair.first_ = GetNode()->CreateChild("node_" + String(numShield), LOCAL);
	pair.first_->SetPosition(Vector3::ZERO);
	pair.first_->LookAt(from);

	pair.second_.isShieldActive = true;
	pair.second_.mat = mat->Clone(matCloneName + String(numShield));
	pair.second_.node = pair.first_;
	pair.second_.sm = pair.second_.node->CreateComponent<StaticModel>();
	pair.second_.sm->SetModel(model);
	pair.second_.sm->SetMaterial(0, pair.second_.mat);
	pair.second_.sm->SetViewMask(128); // we ignore this mask then fire to objects
	pair.second_.shieldVisibleTime = shieldActiveTime;
	Vector3 impNorm = wp - from;
	pair.second_.impactNormal = impNorm.Normalized();

	if (hitPower >= 0.0f) shieldHitPower = hitPower;

	shields.Push(pair);
}

void Shield::ShieldRange(float radius = 1.0f)
{

	for (unsigned int i = 0; i < shields.Size(); i++)
	{
		shields[i].first_->SetScale(Vector3::ONE * radius);
	}
}

void Shield::ShieldActiveTime(float time = 0.5f)
{
	shieldActiveTime = time;
}

void Shield::ShieldHitPower(float normalizedPower)
{
	shieldHitPower = normalizedPower;
}

void Shield::ShieldColor(Color color)
{
	if (mat)
	{
		color.a_ = 1.0f;
		mat->SetShaderParameter("MatDiffColor", Variant(color));
	}
}
