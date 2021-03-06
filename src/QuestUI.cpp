#include "beatsaber-hook/shared/utils/utils.h"
#include "custom-types/shared/register.hpp"

#include "QuestUI.hpp"
#include "ModSettingsInfos.hpp"
#include "CustomTypes/Components/ExternalComponents.hpp"
#include "CustomTypes/Data/CustomDataType.hpp"
#include "CustomTypes/Components/Backgroundable.hpp"
#include "CustomTypes/Components/ScrollViewContent.hpp"
#include "CustomTypes/Components/Settings/IncrementSetting.hpp"
#include "CustomTypes/Components/FlowCoordinators/ModSettingsFlowCoordinator.hpp"

#include "BeatSaberUI.hpp"
#include "InternalBeatSaberUI.hpp"
#include "Sprites/ModSettingsButton.hpp"

#include "GlobalNamespace/MainMenuViewController.hpp"
#include "UnityEngine/Transform.hpp"
#include "UnityEngine/Events/UnityAction.hpp"
#include "Polyglot/LocalizedTextMeshProUGUI.hpp"
#include "HMUI/ViewController_AnimationDirection.hpp"
#include "HMUI/ButtonSpriteSwap.hpp"

#include "customlogger.hpp"

using namespace QuestUI;

Logger& getLogger() {
    static auto logger = new Logger(ModInfo{"questui", VERSION}, LoggerOptions(false, false));
    return *logger;
}

ModSettingsFlowCoordinator* flowCoordinator = nullptr;

HMUI::FlowCoordinator* QuestUI::GetModSettingsFlowCoordinator() {
    return flowCoordinator;
}

int QuestUI::GetModsCount() {
    return ModSettingsInfos::get().size();
}

void OnMenuModSettingsButtonClick(UnityEngine::UI::Button* button) {
    getLogger().info("MenuModSettingsButtonClick");
    if(!flowCoordinator)
        flowCoordinator = BeatSaberUI::CreateFlowCoordinator<ModSettingsFlowCoordinator*>();
    BeatSaberUI::GetMainFlowCoordinator()->PresentFlowCoordinator(flowCoordinator, nullptr, HMUI::ViewController::AnimationDirection::Horizontal, false, false);
}

MAKE_HOOK_OFFSETLESS(OptionsViewController_DidActivate, void, GlobalNamespace::OptionsViewController* self, bool firstActivation, bool addedToHierarchy, bool screenSystemEnabling) {
    OptionsViewController_DidActivate(self, firstActivation, addedToHierarchy, screenSystemEnabling);
    if(firstActivation) {
        flowCoordinator = nullptr;
        BeatSaberUI::ClearCache();
        if(GetModsCount() > 0) {
            UnityEngine::UI::Button* avatarButton = self->settingsButton;
            UnityEngine::UI::Button* button = UnityEngine::Object::Instantiate(avatarButton);
            static auto modSettingsStr = il2cpp_utils::createcsstr("Mod Settings", il2cpp_utils::StringType::Manual);
            button->set_name(modSettingsStr);
            static auto wrapperStr = il2cpp_utils::createcsstr("Wrapper", il2cpp_utils::StringType::Manual);
            UnityEngine::Transform* AvatarParent = self->get_transform()->Find(wrapperStr);
            button->get_transform()->SetParent(AvatarParent, false);
            button->get_transform()->SetAsFirstSibling();
            button->get_onClick()->AddListener(il2cpp_utils::MakeDelegate<UnityEngine::Events::UnityAction*>(classof(UnityEngine::Events::UnityAction*), (Il2CppObject*)nullptr, OnMenuModSettingsButtonClick));
            
            UnityEngine::Object::Destroy(button->GetComponentInChildren<Polyglot::LocalizedTextMeshProUGUI*>());

            button->GetComponentInChildren<TMPro::TextMeshProUGUI*>()->SetText(modSettingsStr);
            HMUI::ButtonSpriteSwap* spriteSwap = button->get_gameObject()->GetComponent<HMUI::ButtonSpriteSwap*>();
            spriteSwap->normalStateSprite = BeatSaberUI::Base64ToSprite(ModSettingsButtonSprite_Normal, 256, 256);
            spriteSwap->disabledStateSprite = spriteSwap->normalStateSprite;
            spriteSwap->highlightStateSprite = BeatSaberUI::Base64ToSprite(ModSettingsButtonSprite_Highlight, 256, 256);
            spriteSwap->pressedStateSprite = spriteSwap->highlightStateSprite;
        }
    }
}

void QuestUI::Init() {
    static bool init = false;
    if(!init) {
        init = true;
        il2cpp_functions::Init();
        custom_types::Register::RegisterType<ExternalComponents>();
        custom_types::Register::RegisterType<CustomDataType>();
        custom_types::Register::RegisterType<Backgroundable>();
        custom_types::Register::RegisterType<ScrollViewContent>();
        custom_types::Register::RegisterType<IncrementSetting>();
        custom_types::Register::RegisterType<ModSettingsButtonsViewController>();
        custom_types::Register::RegisterType<ModSettingsFlowCoordinator>();
        INSTALL_HOOK_OFFSETLESS(getLogger(), OptionsViewController_DidActivate, il2cpp_utils::FindMethodUnsafe("", "OptionsViewController", "DidActivate", 3));
    }
}

void Register::RegisterModSettings(ModInfo modInfo, bool showModInfo, std::string title, Il2CppReflectionType* il2cpp_type, Register::Type type, Register::DidActivateEvent didActivateEvent) {
    ModSettingsInfos::ModSettingsInfo info = {};
    info.modInfo = modInfo;
    info.showModInfo = showModInfo;
    info.title = title;
    info.type = type;
    info.il2cpp_type = reinterpret_cast<System::Type*>(il2cpp_type);
    info.viewController = nullptr;
    info.flowCoordinator = nullptr;
    info.didActivateEvent = didActivateEvent;
    ModSettingsInfos::add(info);
}