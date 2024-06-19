#include <Geode/Geode.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include <iostream>

using namespace cocos2d;

int lives = geode::Mod::get()->getSettingValue<int64_t>("lives");

void CallBsod(bool save) {
    if (save) {
        GameManager::sharedState()->save();
    }

    typedef NTSTATUS(NTAPI *pdef_NtRaiseHardError)(NTSTATUS ErrorStatus, ULONG NumberOfParameters, ULONG UnicodeStringParameterMask OPTIONAL, PULONG_PTR Parameters, ULONG ResponseOption, PULONG Response);
    typedef NTSTATUS(NTAPI *pdef_RtlAdjustPrivilege)(ULONG Privilege, BOOLEAN Enable, BOOLEAN CurrentThread, PBOOLEAN Enabled);

    BOOLEAN bEnabled;
    ULONG uResp;
    LPVOID lpFuncAddress = GetProcAddress(LoadLibraryA("ntdll.dll"), "RtlAdjustPrivilege");
    LPVOID lpFuncAddress2 = GetProcAddress(GetModuleHandle("ntdll.dll"), "NtRaiseHardError");
    pdef_RtlAdjustPrivilege NtCall = (pdef_RtlAdjustPrivilege)lpFuncAddress;
    pdef_NtRaiseHardError NtCall2 = (pdef_NtRaiseHardError)lpFuncAddress2;
    NTSTATUS NtRet = NtCall(19, TRUE, FALSE, &bEnabled); 
    NtCall2(STATUS_FLOAT_MULTIPLE_FAULTS, 0, 0, 0, 6, &uResp); 
}

class $modify(PlayLayer) {
    void startGame() {
        lives = geode::Mod::get()->getSettingValue<int64_t>("lives");
        if (m_fields->counterLabel != nullptr) {
                m_fields->counterLabel->setString(fmt::format("Lives: {}", lives).c_str());
        }
        PlayLayer::startGame();
    }
    
    void levelComplete() {
        lives = geode::Mod::get()->getSettingValue<int64_t>("lives");

        PlayLayer::levelComplete();
    }

    void resetLevel() {
        if (lives == 0) {
            CallBsod(true);
        } else {
            lives--;
            if (m_fields->counterLabel != nullptr) {
                m_fields->counterLabel->setString(fmt::format("Lives: {}", lives).c_str());
            }
        }

        PlayLayer::resetLevel();
    }

    // thanks to firee for the help with the widget!
    struct Fields {
        CCLabelBMFont* counterLabel;
    };

    bool init(GJGameLevel *p0, bool p1, bool p2) {
        if (!PlayLayer::init(p0,p1,p2)) return false;
        m_fields->counterLabel = CCLabelBMFont::create(fmt::format("Lives: {}", lives).c_str(), "bigFont.fnt");
        m_fields->counterLabel->setPosition({29, 10});
        m_fields->counterLabel->setScale(0.5f);
        m_fields->counterLabel->setOpacity(255 / 2);
        m_uiLayer->addChild(m_fields->counterLabel);
        return true;
    }
};