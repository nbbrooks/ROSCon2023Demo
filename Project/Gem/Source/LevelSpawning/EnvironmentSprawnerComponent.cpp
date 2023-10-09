#include "EnvironmentSprawnerComponent.h"

#include <AzCore/Asset/AssetSerializer.h>
#include <AzCore/Component/TransformBus.h>
#include <AzCore/Serialization/EditContext.h>
#include <AzCore/Serialization/EditContextConstants.inl>
#include <AzFramework/Components/TransformComponent.h>
#include <AzFramework/Physics/SystemBus.h>
#include <ImGuiBus.h>
#include <LmbrCentral/Shape/BoxShapeComponentBus.h>
#include <imgui/imgui.h>
#include <AzCore/std/smart_ptr/make_shared.h>
namespace ROS2::Demo
{

    EnvironmentSpawnerComponent::EnvironmentSpawnerComponent(const AZ::Data::Asset<AzFramework::Spawnable>& spawnable, AZStd::shared_ptr<AzFramework::EntitySpawnTicket> spawnTicket):
        m_spawnable(spawnable),
        m_spawnTicket(spawnTicket)
    {
    }

    void EnvironmentSpawnerComponent::Reflect(AZ::ReflectContext* context)
    {
        if (auto serializeContext = azrtti_cast<AZ::SerializeContext*>(context))
        {
            serializeContext->Class<EnvironmentSpawnerComponent>()->Version(1)->Field(
                "Spawnable", &EnvironmentSpawnerComponent::m_spawnable);
            AZ::EditContext* editContext = serializeContext->GetEditContext();
            if (editContext)
            {
                editContext->Class<EnvironmentSpawnerComponent>("EnvironmentSpawnerComponent", "EnvironmentSpawnerComponent")
                    ->ClassElement(AZ::Edit::ClassElements::EditorData, "")
                    ->Attribute(AZ::Edit::Attributes::AppearsInAddComponentMenu, AZ_CRC("Game"))
                    ->Attribute(AZ::Edit::Attributes::Category, "ROS2 Utilities")
                    ->DataElement(AZ::Edit::UIHandlers::Default, &EnvironmentSpawnerComponent::m_spawnable, "m_spawnable", "m_spawnable");
            }
        }
    }

    void EnvironmentSpawnerComponent::Activate()
    {
        AZ::TickBus::Handler::BusConnect();
    }

    void EnvironmentSpawnerComponent::Deactivate()
    {
        m_spawnTicket.reset();
        AZ::TickBus::Handler::BusDisconnect();
    }

    void EnvironmentSpawnerComponent::OnTick(float deltaTime, AZ::ScriptTimePoint time)
    {
       if (m_spawnTicket == nullptr)
       {
            SpawnEntity();
       }
        AZ::TickBus::Handler::BusDisconnect();
    }
    void EnvironmentSpawnerComponent::SpawnEntity()
    {
        const auto parentId = m_entity->GetId();
        auto m_currentTransform = AZ::Transform::CreateIdentity();
        AZ::TransformBus::EventResult(m_currentTransform, parentId, &AZ::TransformBus::Events::GetWorldTM);

        auto spawner = AZ::Interface<AzFramework::SpawnableEntitiesDefinition>::Get();
        AZ_Assert(spawner, "Unable to get spawnable entities definition");
        AzFramework::SpawnAllEntitiesOptionalArgs optionalArgs;
        m_spawnTicket = AZStd::make_shared<AzFramework::EntitySpawnTicket>(m_spawnable);
        // Set the pre-spawn callback to set the name of the root entity to the name of the spawnable
        optionalArgs.m_preInsertionCallback = [m_currentTransform](auto id, auto view)
        {
            if (view.empty())
            {
                return;
            }
            AZ::Entity* root = *view.begin();
            AZ_Assert(root, "Invalid root entity");
            // update the name each child entity
           for (auto childIterator = view.begin() + 1; childIterator != view.end(); ++childIterator)
           {
               auto* childEntity = *childIterator;
               AZ_Printf("EnvironmentSpawnerEditorComponent", "child entity name : %s", childEntity->GetName().c_str());
           }

            auto* transformInterface = root->FindComponent<AzFramework::TransformComponent>();
            if (transformInterface)
            {
               transformInterface->SetWorldTM(m_currentTransform);
            }

        };

       optionalArgs.m_completionCallback = [](auto ticket, auto result)
       {
           AZ_Printf("EnvironmentSpawnerEditorComponent", "spawn completed  : %d", ticket);
       };

        optionalArgs.m_priority = AzFramework::SpawnablePriority_Highest;
        spawner->SpawnAllEntities(*m_spawnTicket, optionalArgs);
        AZ_Printf("EnvironmentSpawnerEditorComponent", "spawn ticked  : %d", m_spawnTicket->GetId());
    }

    void EnvironmentSpawnerComponent::SetSpawnable(const AZ::Data::Asset<AzFramework::Spawnable>& spawnable)
    {
        m_spawnable = spawnable;
    }
} // namespace ROS2::Demo
