#pragma once

#include <AzCore/Component/Component.h>
#include <AzFramework/Entity/EntityDebugDisplayBus.h>

#include <AzFramework/Spawnable/Spawnable.h>
#include <AzFramework/Spawnable/SpawnableEntitiesInterface.h>
#include <AzCore/Component/TickBus.h>
namespace ROS2::Demo
{
    class EnvironmentSpawnerComponent : public AZ::Component, private AZ::TickBus::Handler
   {
   public:
       AZ_COMPONENT(EnvironmentSpawnerComponent, { "4a7c02c1-4fe0-4899-a41c-8f72235d2cbe" });

       EnvironmentSpawnerComponent() = default;
       EnvironmentSpawnerComponent(const AZ::Data::Asset<AzFramework::Spawnable>& spawnable, AZStd::shared_ptr<AzFramework::EntitySpawnTicket> spawnTicket);

       ~EnvironmentSpawnerComponent() = default;

       static void Reflect(AZ::ReflectContext* context);
       void Activate() override;
       void Deactivate() override;
       void SetSpawnable(const AZ::Data::Asset<AzFramework::Spawnable>& spawnable);
       // TickBus
       void OnTick(float deltaTime, AZ::ScriptTimePoint time) override;
   private:
       AZ::Data::Asset<AzFramework::Spawnable> m_spawnable;
       AZStd::shared_ptr<AzFramework::EntitySpawnTicket> m_spawnTicket;
       void SpawnEntity();
   };
} // namespace ROS2::Demo
