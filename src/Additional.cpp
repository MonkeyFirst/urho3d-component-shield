    MyApp(Context* context) : Application(context)
    {
		Shield::RegisterObject(context);
    }

	void SetupAllSceneShields(Scene* scene)
	{
		PODVector<Node*> nodes;

		scene->GetChildrenWithComponent<StaticModel>(nodes, true);
		for (int i = 0; i < nodes.Size(); i++)
		{
			if (nodes[i]->GetVar("shield").GetInt() > 0)
			{
				Color c = nodes[i]->GetVar("color").GetColor();
				
				nodes[i]->CreateComponent<Shield>();
				nodes[i]->GetComponent<Shield>()->ShieldRange(0.8f);
				nodes[i]->GetComponent<Shield>()->ShieldActiveTime(1.0f);
				nodes[i]->GetComponent<Shield>()->ShieldHitPower(1.0f);
				nodes[i]->GetComponent<Shield>()->ShieldColor(c);

			}
		}
	}
	
	void RayCastToShields(Scene* scene, Camera* camera, Vector3 wsFrom, float normalizedFirePower = -1.0f)
	{
		Ray ray = camera->GetScreenRay(0.5f, 0.5f);

		PODVector<RayQueryResult> results;
		RayOctreeQuery query(results, ray, RAY_TRIANGLE, 1000.0f, DRAWABLE_GEOMETRY, 127);

		Octree* octree = scene->GetComponent<Octree>();
		octree->RaycastSingle(query);
		//octree->Raycast(query);

		if (results.Size())
		{
			for (unsigned int i = 0; i < results.Size(); i++)
			{
				RayQueryResult& result = results[i];

				Vector3 hitNormal = result.normal_;
				Vector3 hitPoint = result.position_;
				Node* hitNode = result.node_;

				int shieldPower = hitNode->GetVar("shield").GetInt();

				// if we hit node with StaticModel and shield still have a power
				if (shieldPower > 0)
				{
					using namespace ShieldDamage;

					VariantMap vm = GetEventDataMap();
					Node* shieldNode = hitNode->GetChild("shield");
					vm[P_NODE] = Variant(shieldNode);
					vm[P_POINT] = Variant(hitPoint);
					vm[P_NORMAL] = Variant(hitNormal);
					vm[P_FROM] = Variant(wsFrom);
					vm[P_FIREPOWER] = Variant(normalizedFirePower);
					
					SendEvent(E_SHIELDDAMAGE, vm);
					// change shield power
					hitNode->SetVar("shield", Variant(shieldPower - 10));
				}
				break;
				
			}
		}
		
	}
	
	void HandleMouseButtonDown(StringHash eventType, VariantMap& eventData)
	{
		using namespace MouseButtonDown;
		int button = eventData[P_BUTTON].GetInt();

		if (button == MOUSEB_LEFT) 
		{

			Viewport* vp = player->vp;
			Camera* camera = vp->GetCamera();
			Vector3 firefrom = player->cameraNode->GetWorldPosition();

			RayCastToShields(gameScene->scene, camera, firefrom);			
		}
	}