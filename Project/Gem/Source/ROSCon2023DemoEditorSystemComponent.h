
#pragma once

#include <AzToolsFramework/API/ToolsApplicationAPI.h>

#include <ROSCon2023DemoSystemComponent.h>

namespace ROSCon2023Demo
{
    /// System component for ROSCon2023Demo editor
    class ROSCon2023DemoEditorSystemComponent
        : public ROSCon2023DemoSystemComponent
        , protected AzToolsFramework::EditorEvents::Bus::Handler
    {
        using BaseSystemComponent = ROSCon2023DemoSystemComponent;
    public:
        AZ_COMPONENT(ROSCon2023DemoEditorSystemComponent, "{D50BDB0E-A345-45E3-BB35-B506AC84F055}", BaseSystemComponent);
        static void Reflect(AZ::ReflectContext* context);

        ROSCon2023DemoEditorSystemComponent();
        ~ROSCon2023DemoEditorSystemComponent();

    private:
        static void GetProvidedServices(AZ::ComponentDescriptor::DependencyArrayType& provided);
        static void GetIncompatibleServices(AZ::ComponentDescriptor::DependencyArrayType& incompatible);
        static void GetRequiredServices(AZ::ComponentDescriptor::DependencyArrayType& required);
        static void GetDependentServices(AZ::ComponentDescriptor::DependencyArrayType& dependent);

        // AZ::Component
        void Activate() override;
        void Deactivate() override;
    };
} // namespace ROSCon2023Demo
