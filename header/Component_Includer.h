#pragma once
#include"UIManager.h"
#include"UI/ButtonWithMask.h"
#include"Script/Button.h"
#include"Script/Joy.h"
#include"UI/Button_Empty.h"
#include"Script/clickThrough.h"
#define REGISTER_UI(ClassName) \
    static_assert(std::is_base_of_v<UICreateable, ClassName>, \
        "Registered class must inherit from UIBehavior"); \
    static const bool ClassName##_Registered = []() { \
        UIManager::template RegisterUI<ClassName>(#ClassName); \
        return true; \
    }()
#define REGISTER_LOGIC(ClassName) \
    static_assert(std::is_base_of_v<ScriptBehavior, ClassName>, \
        "Registered class must inherit from UIBehavior"); \
    static const bool ClassName##_Registered = []() { \
        UIManager::template RegisterLogic<ClassName>(#ClassName); \
        return true; \
    }()

REGISTER_UI(ButtonWithMask);
REGISTER_LOGIC(Button);
REGISTER_LOGIC(Joy);
REGISTER_UI(Button_Empty);
REGISTER_LOGIC(clickThrough);
