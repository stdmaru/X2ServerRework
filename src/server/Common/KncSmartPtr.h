#pragma once

#include <memory>

#define SmartPointer( classname ) class classname; typedef std::shared_ptr<classname> classname##Ptr
#define SmartPointers( structname ) struct structname; typedef std::shared_ptr<structname> structname##Ptr
