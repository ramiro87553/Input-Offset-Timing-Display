
#include <Geode/Geode.hpp>
#include <Geode/modify/PauseLayer.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include <Geode/modify/UILayer.hpp>

#include <chrono>
#include <cmath>

using namespace geode::prelude;
using Clock = std::chrono::steady_clock;

static constexpr double TICK_MS = 1000.0 / 240.0;

static TimingDisplayLayer* s_display = nullptr;

class TimingDisplayLayer : public CCLayer {
protected:
    CCLabelBMFont* m_label = nullptr;

public:
    static TimingDisplayLayer* create() {
        auto ret = new TimingDisplayLayer();

        if (ret && ret->init()) {
            ret->autorelease();
            return ret;
        }

        CC_SAFE_DELETE(ret);
        return nullptr;
    }

    bool init() {
        if (!CCLayer::init())
            return false;

        m_label = CCLabelBMFont::create(
            "0.00 ms",
            "bigFont.fnt"
        );

        if (!m_label)
            return false;

        m_label->setScale(
            Mod::get()->getSettingValue<double>("text-scale")
        );

        this->addChild(m_label);

        updatePosition();
        return true;
    }

    void updatePosition() {
        if (!m_label)
            return;

        auto size = CCDirector::sharedDirector()->getWinSize();
        int pos = Mod::get()->getSettingValue<int64_t>("position");

        float margin = 25.f;

        switch (pos) {
            case 0:
                m_label->setAnchorPoint(ccp(0, 1));
                m_label->setPosition(ccp(margin, size.height - margin));
                break;
            case 1:
                m_label->setAnchorPoint(ccp(0.5f, 1));
                m_label->setPosition(ccp(size.width / 2, size.height - margin));
                break;
            case 2:
                m_label->setAnchorPoint(ccp(1, 1));
                m_label->setPosition(ccp(size.width - margin, size.height - margin));
                break;
            case 3:
                m_label->setAnchorPoint(ccp(0, 0.5f));
                m_label->setPosition(ccp(margin, size.height / 2));
                break;
            case 4:
                m_label->setAnchorPoint(ccp(0.5f, 0.5f));
                m_label->setPosition(ccp(size.width / 2, size.height / 2));
                break;
            case 5:
                m_label->setAnchorPoint(ccp(1, 0.5f));
                m_label->setPosition(ccp(size.width - margin, size.height / 2));
                break;
            case 6:
                m_label->setAnchorPoint(ccp(0, 0));
                m_label->setPosition(ccp(margin, margin));
                break;
            case 7:
                m_label->setAnchorPoint(ccp(0.5f, 0));
                m_label->setPosition(ccp(size.width / 2, margin));
                break;
            case 8:
                m_label->setAnchorPoint(ccp(1, 0));
                m_label->setPosition(ccp(size.width - margin, margin));
                break;
        }
    }

    void updateOffset(double offset) {
        if (!m_label)
            return;

        bool enabled = Mod::get()->getSettingValue<bool>("enabled");
        m_label->setVisible(enabled);

        if (!enabled)
            return;

        m_label->setScale(
            Mod::get()->getSettingValue<double>("text-scale")
        );

        updatePosition();

        m_label->setString(
            fmt::format("{:+.2f} ms", offset).c_str()
        );

        double absOffset = std::abs(offset);

        if (absOffset <= 1.0)
            m_label->setColor(ccc3(0, 255, 0));
        else if (absOffset <= 2.5)
            m_label->setColor(ccc3(255, 255, 0));
        else
            m_label->setColor(ccc3(255, 60, 60));
    }
};

class OffsetSettingsPopup : public Popup<> {
protected:
    CCLabelBMFont* m_enabledLabel = nullptr;
    CCLabelBMFont* m_positionLabel = nullptr;

    bool setup() override {
        this->setTitle("Input Offset Timing Display");

        auto winSize = this->getContentSize();

        auto logo = CCSprite::create("logo.png");

        if (logo) {
            logo->setScale(0.25f);
            logo->setPosition(ccp(winSize.width / 2, winSize.height - 40));
            this->addChild(logo);
        }

        auto enabledTitle = CCLabelBMFont::create(
            "Offset Display",
            "bigFont.fnt"
        );

        enabledTitle->setScale(0.45f);
        enabledTitle->setPosition(ccp(winSize.width / 2, winSize.height / 2 + 40));
        this->addChild(enabledTitle);

        m_enabledLabel = CCLabelBMFont::create(
            "",
            "bigFont.fnt"
        );

        m_enabledLabel->setScale(0.45f);
        m_enabledLabel->setPosition(ccp(winSize.width / 2, winSize.height / 2 + 15));
        this->addChild(m_enabledLabel);

        auto positionTitle = CCLabelBMFont::create(
            "Display Position",
            "bigFont.fnt"
        );

        positionTitle->setScale(0.45f);
        positionTitle->setPosition(ccp(winSize.width / 2, winSize.height / 2 - 20));
        this->addChild(positionTitle);

        m_positionLabel = CCLabelBMFont::create(
            "",
            "bigFont.fnt"
        );

        m_positionLabel->setScale(0.4f);
        m_positionLabel->setPosition(ccp(winSize.width / 2, winSize.height / 2 - 50));
        this->addChild(m_positionLabel);

        auto menu = CCMenu::create();
        menu->setPosition(ccp(0, 0));

        auto toggle = CCMenuItemSpriteExtra::create(
            ButtonSprite::create("Toggle"),
            this,
            menu_selector(OffsetSettingsPopup::onToggle)
        );

        toggle->setPosition(ccp(winSize.width / 2, winSize.height / 2 + 15));
        menu->addChild(toggle);

        auto left = CCMenuItemSpriteExtra::create(
            ButtonSprite::create("<"),
            this,
            menu_selector(OffsetSettingsPopup::onPrevious)
        );

        left->setPosition(ccp(winSize.width / 2 - 80, winSize.height / 2 - 50));
        menu->addChild(left);

        auto right = CCMenuItemSpriteExtra::create(
            ButtonSprite::create(">"),
            this,
            menu_selector(OffsetSettingsPopup::onNext)
        );

        right->setPosition(ccp(winSize.width / 2 + 80, winSize.height / 2 - 50));
        menu->addChild(right);

        this->addChild(menu);

        updateLabels();
        return true;
    }

    void updateLabels() {
        bool enabled = Mod::get()->getSettingValue<bool>("enabled");
        int pos = Mod::get()->getSettingValue<int64_t>("position");

        m_enabledLabel->setString(enabled ? "ON" : "OFF");

        static const char* positions[] = {
            "Top Left", "Top Center", "Top Right",
            "Center Left", "Center", "Center Right",
            "Bottom Left", "Bottom Center", "Bottom Right"
        };

        if (pos < 0 || pos > 8)
            pos = 1;

        m_positionLabel->setString(positions[pos]);

        if (s_display)
            s_display->updatePosition();
    }

public:
    static OffsetSettingsPopup* create() {
        auto ret = new OffsetSettingsPopup();

        if (ret && ret->init(380.f, 240.f)) {
            ret->autorelease();
            return ret;
        }

        CC_SAFE_DELETE(ret);
        return nullptr;
    }

    void onToggle(CCObject*) {
        bool enabled = Mod::get()->getSettingValue<bool>("enabled");

        Mod::get()->setSettingValue<bool>(
            "enabled",
            !enabled
        );

        updateLabels();
    }

    void onPrevious(CCObject*) {
        int pos = Mod::get()->getSettingValue<int64_t>("position");

        Mod::get()->setSettingValue<int>(
            "position",
            (pos + 8) % 9
        );

        updateLabels();
    }

    void onNext(CCObject*) {
        int pos = Mod::get()->getSettingValue<int64_t>("position");

        Mod::get()->setSettingValue<int>(
            "position",
            (pos + 1) % 9
        );

        updateLabels();
    }
};

class $modify(MyUILayer, UILayer) {
    bool init(GJBaseGameLayer* gameLayer) {
        if (!UILayer::init(gameLayer))
            return false;

        s_display = TimingDisplayLayer::create();

        if (s_display)
            this->addChild(s_display, 100);

        return true;
    }
};

class $modify(MyPauseLayer, PauseLayer) {
    void customSetup() {
        PauseLayer::customSetup();

        auto logo = CCSprite::create("logo.png");

        if (!logo) {
            log::error("Could not load logo.png");
            return;
        }

        logo->setScale(0.25f);

        auto button = CCMenuItemSpriteExtra::create(
            logo,
            this,
            menu_selector(MyPauseLayer::onOffsetSettings)
        );

        button->setID("offset-settings-button");

        auto menu = CCMenu::create();
        menu->addChild(button);

        auto winSize = CCDirector::sharedDirector()->getWinSize();

        menu->setPosition(
            ccp(winSize.width / 2, winSize.height / 2 - 100)
        );

        this->addChild(menu);
    }

    void onOffsetSettings(CCObject*) {
        OffsetSettingsPopup::create()->show();
    }
};

class $modify(MyPlayLayer, PlayLayer) {
    void pushButton(PlayerObject* player, int button, bool isPlayer1) {
        PlayLayer::pushButton(player, button, isPlayer1);

        if (!s_display)
            return;

        auto now = Clock::now();

        static Clock::time_point lastTick;
        static bool hasTick = false;

        if (!hasTick) {
            lastTick = now;
            hasTick = true;
        }

        double elapsed = std::chrono::duration<double, std::milli>(
            now - lastTick
        ).count();

        double tickPosition = std::fmod(elapsed, TICK_MS);

        double offset = tickPosition <= TICK_MS / 2.0
            ? -tickPosition
            : TICK_MS - tickPosition;

        s_display->updateOffset(offset);
    }
};
