#include "FoilWrapperController.h"
#include "ScriptSpawnSytemBus.h"
#include <AzCore/Asset/AssetSerializer.h>
#include <AzCore/Serialization/EditContext.h>
#include <AzCore/Serialization/SerializeContext.h>
#include <AzFramework/Components/TransformComponent.h>
#include <Integration/SimpleMotionComponentBus.h>
namespace ROS2::Demo
{
    void FoilWrapperConfig::Reflect(AZ::ReflectContext* context)
    {
        AZ::SerializeContext* serialize = azrtti_cast<AZ::SerializeContext*>(context);
        if (serialize)
        {
            serialize->Class<FoilWrapperConfig>()
                ->Version(1)
                ->Field("FoilWrapperEntityId", &FoilWrapperConfig::m_foilWrapperEntityId)
                ->Field("CollisionTrigger", &FoilWrapperConfig::m_collisionTrigger)
                ->Field("SpawnablePayloadFoiled", &FoilWrapperConfig::m_spawnablePayloadFoiled);
            if (AZ::EditContext* editContext = serialize->GetEditContext())
            {
                // clang-format off
                editContext->Class<FoilWrapperConfig>("FoilWrapperConfig", "FoilWrapperConfig")
                    ->ClassElement(AZ::Edit::ClassElements::EditorData, "")
                        ->Attribute(AZ::Edit::Attributes::Category, "Demo")
                        ->Attribute(AZ::Edit::Attributes::AppearsInAddComponentMenu, AZ_CRC_CE("Game"))
                    ->DataElement(AZ::Edit::UIHandlers::Default, &FoilWrapperConfig::m_foilWrapperEntityId, "m_foilWrapperEntityId", "")
                    ->DataElement(AZ::Edit::UIHandlers::Default, &FoilWrapperConfig::m_collisionTrigger, "m_collisionTrigger", "")
                    ->DataElement(AZ::Edit::UIHandlers::Default, &FoilWrapperConfig::m_spawnablePayloadFoiled, "m_spawnablePayloadFoiled", "");
                // clang-format on
            }
        }
    }

    void FoilWrapper::Reflect(AZ::ReflectContext* context)
    {
        FoilWrapperConfig::Reflect(context);
        if (AZ::SerializeContext* serialize = azrtti_cast<AZ::SerializeContext*>(context))
        {
            serialize->Class<FoilWrapper, AZ::Component>()->Version(1)->Field("FoilWrapperConfig", &FoilWrapper::m_configuration);
            if (AZ::EditContext* editContext = serialize->GetEditContext())
            {
                // clang-format off
                editContext->Class<FoilWrapper>("FoilWrapper", "FoilWrapper")
                    ->ClassElement(AZ::Edit::ClassElements::EditorData, "")
                        ->Attribute(AZ::Edit::Attributes::Category, "Demo")
                        ->Attribute(AZ::Edit::Attributes::AppearsInAddComponentMenu, AZ_CRC_CE("Game"))
                    ->DataElement(AZ::Edit::UIHandlers::Default, &FoilWrapper::m_configuration, "m_configuration", "");
                // clang-format on
            }
        }
    }

    void FoilWrapper::Activate()
    {
        m_onTriggerEnterHandler = AzPhysics::SimulatedBodyEvents::OnTriggerEnter::Handler(
            [&]([[maybe_unused]] AzPhysics::SimulatedBodyHandle bodyHandle, [[maybe_unused]] const AzPhysics::TriggerEvent& event)
            {
                const auto entityId = event.m_otherBody->GetEntityId();
                if (!isObjectBox(entityId))
                {
                    return;
                }
                AZ_Printf("FoilWrapper", "Entered box with entity id %s", entityId.ToString().c_str());
                if (m_state == FoilWrapperState::Idle)
                {
                    m_state = FoilWrapperState::Starting;
                }
                m_collidingEntities.push_back(entityId);
            });

        AZ::TickBus::Handler::BusConnect();
    }

    void FoilWrapper::Deactivate()
    {
        AZ::TickBus::Handler::BusDisconnect();
    }

    bool FoilWrapper::isObjectBox(const AZ::EntityId entityId) const
    {
        bool isBox = false;
        LmbrCentral::TagComponentRequestBus::EventResult(isBox, entityId, &LmbrCentral::TagComponentRequests::HasTag, BoxTag);
        return isBox;
    }

    void FoilWrapper::OnTick(float delta, AZ::ScriptTimePoint timePoint)
    {
        AzPhysics::SystemInterface* physicsSystem = AZ::Interface<AzPhysics::SystemInterface>::Get();
        AZ_Assert(physicsSystem, "No physics system.");

        AzPhysics::SceneInterface* sceneInterface = AZ::Interface<AzPhysics::SceneInterface>::Get();
        AZ_Assert(sceneInterface, "No scene intreface.");

        AzPhysics::SceneHandle defaultSceneHandle = sceneInterface->GetSceneHandle(AzPhysics::DefaultPhysicsSceneName);
        AZ_Assert(defaultSceneHandle != AzPhysics::InvalidSceneHandle, "Invalid default physics scene handle.");

        // Connect the trigger handlers if not already connected, it is circumventing the issue GH-16188, the
        // RigidbodyNotificationBus should be used instead.
        if (!m_onTriggerEnterHandler.IsConnected())
        {
            AZ_Assert(m_configuration.m_foilWrapperEntityId.IsValid(), "Invalid foil wrapper entity id.");
            if (m_configuration.m_foilWrapperEntityId.IsValid())
            {
                AZStd::pair<AzPhysics::SceneHandle, AzPhysics::SimulatedBodyHandle> foundBody =
                    physicsSystem->FindAttachedBodyHandleFromEntityId(m_configuration.m_collisionTrigger);
                AZ_Warning("FoilWrapper", foundBody.first != AzPhysics::InvalidSceneHandle, "No body found for m_collisionTrigger.");
                if (foundBody.first != AzPhysics::InvalidSceneHandle)
                {
                    AzPhysics::SimulatedBodyEvents::RegisterOnTriggerEnterHandler(
                        foundBody.first, foundBody.second, m_onTriggerEnterHandler);
                }
            }
        }
        if (m_state == FoilWrapperState::Idle)
        {
            m_timer = 0;
        }
        if (m_state == FoilWrapperState::Starting)
        {
            m_timer += delta;
            if (m_timer > 10.0)
            {
                AZ_Printf("FoilWrapper", "Warpping starterd");
                EMotionFX::Integration::SimpleMotionComponentRequestBus::Event(
                    m_configuration.m_foilWrapperEntityId, &EMotionFX::Integration::SimpleMotionComponentRequestBus::Events::PlayMotion);
                m_state = FoilWrapperState::Wrapping;
                m_timer = 0;
            }
        }
        if (m_state == FoilWrapperState::Wrapping)
        {
            m_timer += delta;
            if (m_timer > 10.0)
            {
                AZ::Transform transform = AZ::Transform::CreateIdentity();
                AZ::TransformBus::EventResult(transform, m_configuration.m_collisionTrigger, &AZ::TransformBus::Events::GetWorldTM);
                for (auto& entityId : m_collidingEntities)
                {
                    AZ::Entity* entity{};
                    AZ::ComponentApplicationBus::BroadcastResult(entity, &AZ::ComponentApplicationRequests::FindEntity, entityId);
                    if (entity)
                    {
                        const auto& entityName = entity->GetName();
                        AZ_Printf("FoilWrapper", "Despawning box with entity with name %s", entityName.c_str());
                        ScriptSpawnSystemRequestBus::Broadcast(&ScriptSpawnSystemRequestBus::Events::DespawnBox, entityName);
                    }
                }
                ScriptSpawnSystemRequestBus::Broadcast(
                    &ScriptSpawnSystemRequestBus::Events::SpawnAsset, m_configuration.m_spawnablePayloadFoiled, transform, "foo");

                m_collidingEntities.clear();
                m_state = FoilWrapperState::Idle;
                AZ_Printf("FoilWrapper", "Done, going to idle");
                m_timer = 0;
            }
        }
    }
} // namespace ROS2::Demo
