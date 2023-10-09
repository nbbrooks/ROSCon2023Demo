#include "EnvironmentSprawnerEditorComponent.h"
#include <AzCore/Serialization/SerializeContext.h>
#include <AzCore/Serialization/EditContext.h>
#include <AzCore/Asset/AssetSerializer.h>
#include <AzFramework/Components/TransformComponent.h>
#include "EnvironmentSprawnerComponent.h"
namespace ROS2::Demo
{
    EnvironmentSpawnerEditorComponent::EnvironmentSpawnerEditorComponent()
   {

   }


   void EnvironmentSpawnerEditorComponent::Reflect(AZ::ReflectContext* context)
   {
       if (auto* serialize = azrtti_cast<AZ::SerializeContext*>(context))
       {
           serialize->Class<EnvironmentSpawnerEditorComponent, AzToolsFramework::Components::EditorComponentBase>()
               ->Version(0)
               ->Field("Spawnable", &EnvironmentSpawnerEditorComponent::m_spawnable);

           if (AZ::EditContext* editContext = serialize->GetEditContext())
           {
               editContext->Class<EnvironmentSpawnerEditorComponent>("EnvironmentSpawnerEditorComponent", "Allow to spawn environment, with some changes in it")
                   ->ClassElement(AZ::Edit::ClassElements::EditorData, "")
                   ->Attribute(AZ::Edit::Attributes::Category, "ROS2")
                   ->Attribute(AZ::Edit::Attributes::AppearsInAddComponentMenu, AZ_CRC("Game"))
                   ->DataElement(AZ::Edit::UIHandlers::Default, &EnvironmentSpawnerEditorComponent::m_spawnable, "m_spawnable", "m_spawnable")
                   ->Attribute(AZ::Edit::Attributes::ChangeNotify, &EnvironmentSpawnerEditorComponent::OnSpawnableChanged);
           }
       }
   }

   void EnvironmentSpawnerEditorComponent::Activate()
   {
       AZ_Printf("EnvironmentSpawnerEditorComponent", "Activate");
       if (m_spawnable.GetId().IsValid())
       {
           SpawnEntity();
       }
   }

   void EnvironmentSpawnerEditorComponent::Deactivate()
   {

       AZ_Printf("EnvironmentSpawnerEditorComponent", "Deactivate");


   }

   void EnvironmentSpawnerEditorComponent::GetRequiredServices(AZ::ComponentDescriptor::DependencyArrayType& required)
   {
   }

   void EnvironmentSpawnerEditorComponent::GetIncompatibleServices(AZ::ComponentDescriptor::DependencyArrayType& incompatible)
   {
   }

   void EnvironmentSpawnerEditorComponent::GetProvidedServices(AZ::ComponentDescriptor::DependencyArrayType& provided)
   {
       provided.push_back(AZ_CRC_CE("EnvironmentSpawnerEditorComponentService"));
   }

   void EnvironmentSpawnerEditorComponent::BuildGameEntity(AZ::Entity* gameEntity)
   {
       AZ_Printf("EnvironmentSpawnerEditorComponent", "BuildGameEntity");
       gameEntity->CreateComponent<EnvironmentSpawnerComponent>(m_spawnable);

   }

   void EnvironmentSpawnerEditorComponent::OnSpawnableChanged()
   {
       AZ_Printf("EnvironmentSpawnerEditorComponent", "OnSpawnableChanged");
       SpawnEntity();
   }

   void EnvironmentSpawnerEditorComponent::SpawnEntity()
   {
//       const auto parentId = m_entity->GetId();
//       auto m_currentTransform = AZ::Transform::CreateIdentity();
//       AZ::TransformBus::EventResult(m_currentTransform, parentId, &AZ::TransformBus::Events::GetWorldTM);
//       auto transform = AZ::Transform::CreateIdentity();
//       auto spawner = AZ::Interface<AzFramework::SpawnableEntitiesDefinition>::Get();
//       AZ_Assert(spawner, "Unable to get spawnable entities definition");
//       AzFramework::SpawnAllEntitiesOptionalArgs optionalArgs;
//       AzFramework::EntitySpawnTicket ticket(m_spawnable);
//       // Set the pre-spawn callback to set the name of the root entity to the name of the spawnable
//       optionalArgs.m_preInsertionCallback = [transform, parentId](auto id, auto view)
//       {
//           if (view.empty())
//           {
//               return;
//           }
//           AZ::Entity* root = *view.begin();
//           AZ_Assert(root, "Invalid root entity");
//           // update the name each child entity
////           for (auto childIterator = view.begin() + 1; childIterator != view.end(); ++childIterator)
////           {
////               auto* childEntity = *childIterator;
////               AZ_Assert(childEntity, "Invalid child entity");
////           }
//
//           auto* transformInterface = root->FindComponent<AzFramework::TransformComponent>();
//           transformInterface->SetWorldTM(transform);
//           if (parentId.IsValid())
//           {
//               transformInterface->SetParent(parentId);
//           }
//       };
//
//       optionalArgs.m_completionCallback = [](auto ticket, auto result)
//       {
//           AZ_Printf("EnvironmentSpawnerEditorComponent", "Spawn completed %d", result);
//       };
//
//       optionalArgs.m_priority = AzFramework::SpawnablePriority_Highest;
//       spawner->SpawnAllEntities(ticket, optionalArgs);
//       m_spawnTicket = ticket;
//       AZ_Printf("EnvironmentSpawnerEditorComponent", "spawn ticked  : %d", m_spawnTicket.GetId());

   }
} // namespace ROS2
