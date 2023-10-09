
#include <ROSCon2023DemoModuleInterface.h>
#include "ROSCon2023DemoEditorSystemComponent.h"
#include "LevelSpawning/EnvironmentSprawnerEditorComponent.h"
namespace ROSCon2023Demo
{
    class ROSCon2023DemoEditorModule
        : public ROSCon2023DemoModuleInterface
    {
    public:
        AZ_RTTI(ROSCon2023DemoEditorModule, "{DE3B8F50-4B74-4F64-8A38-FC8FDB202F02}", ROSCon2023DemoModuleInterface);
        AZ_CLASS_ALLOCATOR(ROSCon2023DemoEditorModule, AZ::SystemAllocator);

        ROSCon2023DemoEditorModule()
        {
            // Push results of [MyComponent]::CreateDescriptor() into m_descriptors here.
            // Add ALL components descriptors associated with this gem to m_descriptors.
            // This will associate the AzTypeInfo information for the components with the the SerializeContext, BehaviorContext and EditContext.
            // This happens through the [MyComponent]::Reflect() function.
            m_descriptors.insert(m_descriptors.end(), {
                ROSCon2023DemoEditorSystemComponent::CreateDescriptor(),
                //ROS2::Demo::EnvironmentSpawnerEditorComponent::CreateDescriptor(),
            });
        }

        /**
         * Add required SystemComponents to the SystemEntity.
         * Non-SystemComponents should not be added here
         */
        AZ::ComponentTypeList GetRequiredSystemComponents() const override
        {
            return AZ::ComponentTypeList {
                azrtti_typeid<ROSCon2023DemoEditorSystemComponent>(),
            };
        }
    };
}// namespace ROSCon2023Demo

AZ_DECLARE_MODULE_CLASS(Gem_ROSCon2023Demo, ROSCon2023Demo::ROSCon2023DemoEditorModule)
