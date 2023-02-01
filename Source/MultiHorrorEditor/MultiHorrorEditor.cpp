#include "MultiHorrorEditor.h"

#include "BasicSpawnerVisualizer.h"
#include "Modules/ModuleManager.h"
#include "Modules/ModuleInterface.h"
#include "MultiHorror/BasicSpawnerComponent.h"

IMPLEMENT_GAME_MODULE(FMultiHorrorEditorModule, MultiHorrorEditor);

void FMultiHorrorEditorModule::StartupModule()
{
	//Check if editor is valid
	if(GUnrealEd)
	{
		//Registering the visualizer
		TSharedPtr<FBasicSpawnerVisualizer> SpawnerVisualizer = MakeShareable(new FBasicSpawnerVisualizer);
		if(SpawnerVisualizer.IsValid())
		{
			GUnrealEd->RegisterComponentVisualizer(UBasicSpawnerComponent::StaticClass()->GetFName(), SpawnerVisualizer);

			SpawnerVisualizer->OnRegister();
		}
	}
}

void FMultiHorrorEditorModule::ShutdownModule()
{
	//Check if editor is valid
	if(GUnrealEd)
	{
		GUnrealEd->UnregisterComponentVisualizer(UBasicSpawnerComponent::StaticClass()->GetFName());
	}
}
