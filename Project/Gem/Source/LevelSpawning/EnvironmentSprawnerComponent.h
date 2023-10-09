/*
* Copyright (c) Contributors to the Open 3D Engine Project.
* For complete copyright and license terms please see the LICENSE at the root
* of this distribution.
*
* SPDX-License-Identifier: Apache-2.0 OR MIT
*
*/
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
       EnvironmentSpawnerComponent(const AZ::Data::Asset<AzFramework::Spawnable>& spawnable);

       ~EnvironmentSpawnerComponent() = default;

       static void Reflect(AZ::ReflectContext* context);
       void Activate() override;
       void Deactivate() override;
       void SetSpawnable(const AZ::Data::Asset<AzFramework::Spawnable>& spawnable);
       // TickBus
       void OnTick(float deltaTime, AZ::ScriptTimePoint time) override;
   private:
       AZ::Data::Asset<AzFramework::Spawnable> m_spawnable;
       AzFramework::EntitySpawnTicket m_spawnTicket;
       void SpawnEntity();
   };
} // namespace ROS2::Demo
