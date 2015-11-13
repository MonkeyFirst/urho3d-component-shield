# urho3d-component-shield
urho3d-component-shield

Using:
Add node.var with field name "shield" (int > 0) (optional "color" (color)) for all nodes in scene with models what will be armored with this shield. save changes.

```
#include "Shield.h"

void Game::Start()
{
    ...
    Shield::RegisterObject(context_);
    ...
}
```

On level loading or scene create call:
```
SetupAllSceneShields(Scene* scene)
```
To create Shield for all objects in scene what have a "shield" var.

Do shoting with this method: 
```
void RayCastToShields(Scene* scene, Camera* camera, Vector3 wsFrom, float normalizedFirePower = -1.0f)
```

On mouse button or else where...
```
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
```
	
If you have a several types of guns with various power, you may use last parameter in RayCastToShields() on each gun type fire, with adjustment for last parameter 

```
normalizedFirePower =[0..1] // -1 = internaly mean use default hit power
```

