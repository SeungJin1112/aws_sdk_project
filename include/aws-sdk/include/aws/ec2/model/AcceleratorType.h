﻿/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#pragma once
#include <aws/ec2/EC2_EXPORTS.h>
#include <aws/core/utils/memory/stl/AWSString.h>

namespace Aws
{
namespace EC2
{
namespace Model
{
  enum class AcceleratorType
  {
    NOT_SET,
    gpu,
    fpga,
    inference
  };

namespace AcceleratorTypeMapper
{
AWS_EC2_API AcceleratorType GetAcceleratorTypeForName(const Aws::String& name);

AWS_EC2_API Aws::String GetNameForAcceleratorType(AcceleratorType value);
} // namespace AcceleratorTypeMapper
} // namespace Model
} // namespace EC2
} // namespace Aws
