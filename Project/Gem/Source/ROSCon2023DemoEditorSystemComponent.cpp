
#include <AzCore/Serialization/SerializeContext.h>
#include "ROSCon2023DemoEditorSystemComponent.h"

namespace ROSCon2023Demo
{
    void ROSCon2023DemoEditorSystemComponent::Reflect(AZ::ReflectContext* context)
    {
        if (auto serializeContext = azrtti_cast<AZ::SerializeContext*>(context))
        {
            serializeContext->Class<ROSCon2023DemoEditorSystemComponent, ROSCon2023DemoSystemComponent>()
                ->Version(0);
        }
    }

    ROSCon2023DemoEditorSystemComponent::ROSCon2023DemoEditorSystemComponent() = default;

    ROSCon2023DemoEditorSystemComponent::~ROSCon2023DemoEditorSystemComponent() = default;

    void ROSCon2023DemoEditorSystemComponent::GetProvidedServices(AZ::ComponentDescriptor::DependencyArrayType& provided)
    {
        BaseSystemComponent::GetProvidedServices(provided);
        provided.push_back(AZ_CRC_CE("ROSCon2023DemoEditorService"));
    }

    void ROSCon2023DemoEditorSystemComponent::GetIncompatibleServices(AZ::ComponentDescriptor::DependencyArrayType& incompatible)
    {
        BaseSystemComponent::GetIncompatibleServices(incompatible);
        incompatible.push_back(AZ_CRC_CE("ROSCon2023DemoEditorService"));
    }

    void ROSCon2023DemoEditorSystemComponent::GetRequiredServices([[maybe_unused]] AZ::ComponentDescriptor::DependencyArrayType& required)
    {
        BaseSystemComponent::GetRequiredServices(required);
    }

    void ROSCon2023DemoEditorSystemComponent::GetDependentServices([[maybe_unused]] AZ::ComponentDescriptor::DependencyArrayType& dependent)
    {
        BaseSystemComponent::GetDependentServices(dependent);
    }

    void ROSCon2023DemoEditorSystemComponent::Activate()
    {
        ROSCon2023DemoSystemComponent::Activate();
        AzToolsFramework::EditorEvents::Bus::Handler::BusConnect();
    }

    void ROSCon2023DemoEditorSystemComponent::Deactivate()
    {
        AzToolsFramework::EditorEvents::Bus::Handler::BusDisconnect();
        ROSCon2023DemoSystemComponent::Deactivate();
    }

} // namespace ROSCon2023Demo
