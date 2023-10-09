#pragma once

#include <AzCore/Component/Component.h>
#include <AzFramework/Entity/EntityDebugDisplayBus.h>
#include <AzToolsFramework/API/ComponentEntitySelectionBus.h>
#include <AzToolsFramework/ToolsComponents/EditorComponentBase.h>
#include <AzFramework/Spawnable/Spawnable.h>
#include <AzFramework/Spawnable/SpawnableEntitiesInterface.h>
namespace ROS2::Demo
{
   //! ROS2 Camera Editor sensor component class
   //! Allows turning an entity into a camera sensor in Editor
   //! Component draws camera frustum in the Editor
   class EnvironmentSpawnerEditorComponent
       : public AzToolsFramework::Components::EditorComponentBase
   {
   public:
       EnvironmentSpawnerEditorComponent();
       ~EnvironmentSpawnerEditorComponent() override = default;
       AZ_EDITOR_COMPONENT(EnvironmentSpawnerEditorComponent, "{e11dede2-6670-11ee-8c99-0242ac120002}");
       static void Reflect(AZ::ReflectContext* context);
       static void GetRequiredServices(AZ::ComponentDescriptor::DependencyArrayType& required);
       static void GetIncompatibleServices(AZ::ComponentDescriptor::DependencyArrayType& incompatible);
       static void GetProvidedServices(AZ::ComponentDescriptor::DependencyArrayType& provided);

       void Activate() override;
       void Deactivate() override;

       // AzToolsFramework::Components::EditorComponentBase overrides
       void BuildGameEntity(AZ::Entity* gameEntity) override;

   private:
       AZ::Data::Asset<AzFramework::Spawnable> m_spawnable;
       AzFramework::EntitySpawnTicket m_spawnTicket;
       void OnSpawnableChanged();
       void SpawnEntity();


   };
} // namespace ROS2
