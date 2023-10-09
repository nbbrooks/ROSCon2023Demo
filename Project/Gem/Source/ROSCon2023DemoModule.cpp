/*
 * Copyright (c) Contributors to the Open 3D Engine Project.
 * For complete copyright and license terms please see the LICENSE at the root of this distribution.
 *
 * SPDX-License-Identifier: Apache-2.0 OR MIT
 *
 */

#include <ROSCon2023DemoModuleInterface.h>
#include <ROSCon2023DemoSystemComponent.h>

namespace ROSCon2023Demo
{
    class ROSCon2023DemoModule : public ROSCon2023DemoModuleInterface
    {
    public:
        AZ_RTTI(ROSCon2023DemoModule, "{e23a1379-787c-481e-ad83-c0e04a3d06fe}", ROSCon2023DemoModuleInterface);
        AZ_CLASS_ALLOCATOR(ROSCon2023DemoModule, AZ::SystemAllocator);
    };
} // namespace ROS2

AZ_DECLARE_MODULE_CLASS(Gem_ROSCon2023Demo, ROSCon2023Demo::ROSCon2023DemoModule)


