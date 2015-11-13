#pragma once

URHO3D_EVENT(E_SHIELDDAMAGE, ShieldDamage)
{
	URHO3D_PARAM(P_NODE, Node);                // ptr
	URHO3D_PARAM(P_SHIELD, Shield);            // ptr
	URHO3D_PARAM(P_POINT, Vector3);            // vec3
	URHO3D_PARAM(P_NORMAL, Vector3);           // vec3
	URHO3D_PARAM(P_FROM, Vector3);             // vec3
	URHO3D_PARAM(P_FIREPOWER, float);          // float
}

class Shield : public LogicComponent
{
	URHO3D_OBJECT(Shield, LogicComponent);
public:
	Shield(Context* context);
	static void RegisterObject(Context* context);

	void Start();
	void Stop();
	void Update(float timeStep);
	void FixedUpdate(float timeStep);
	/// for adjust scale in pater node
	void ShieldRange(float radius);
	/// how long shield will do diffuse on own surface
	void ShieldActiveTime(float time);
	/// base color of shield grid (MatDiffColor)
	void ShieldColor(Color color);
	/// how much are shield will affected by hit
	void ShieldHitPower(float normalizedPower);
	
private:
	struct ShieldField
	{
		SharedPtr<Node> node;
		SharedPtr<StaticModel> sm;
		SharedPtr<Material> mat;
		bool isShieldActive;
		float shieldVisibleTime;
		Vector3 impactNormal;
		void Free();
	};

	//Pair<Node, StaticModel> shieldLevel;
	Vector<Pair<Node*, ShieldField>> shields;

	SharedPtr<Node> node;
	SharedPtr<Model> model;
	SharedPtr<Material> mat;
	SharedPtr<Text3D> shieldText;

	float shieldActiveTime = 0.3f; // falloff sec
	float shieldVisibleTime;
	float shieldHitPower = 0.2f; // half diffuse effect on hit
	Color defaultColor;
	bool shieldActive;
	static int numShields;
	String matCloneName;
	
	void AddHit(Vector3& newHitPos);


	void HandleGotDamage(StringHash eventType, VariantMap& eventData);

};