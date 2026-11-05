
#include <Geode/Geode.hpp>
#include <Geode/modify/PauseLayer.hpp>

using namespace geode::prelude;

class OffsetSettingsPopup : public Popup<> {
protected:
    bool setup() override {
        this->setTitle("Input Offset Timing Display");

        auto winSize = this->getContentSize();

        auto logo = CCSprite::create("logo.png");

        if (logo) {
            logo->setScale(0.35f);
            logo->setPosition(
                ccp(winSize.width / 2, winSize.height - 45)
            );
            this->addChild(logo);
        }

        auto info = CCLabelBMFont::create(
            "Configure your offset display",
            "bigFont.fnt"
        );

        info->setScale(0.4f);
        info->setPosition(
            ccp(winSize.width / 2, winSize.height - 90)
        );
        this->addChild(info);

        auto enabledLabel = CCLabelBMFont::create(
            "Enable Offset Display",
            "bigFont.fnt"
        );

        enabledLabel->setScale(0.45f);
        enabledLabel->setPosition(
            ccp(winSize.width / 2, winSize.height / 2 + 25)
        );
        this->addChild(enabledLabel);

        auto toggle = CCMenuItemToggler::create(
            CCMenuItemSpriteExtra::create(
                ButtonSprite::create("ON"),
                nullptr
            ),
            CCMenuItemSpriteExtra::create(
                ButtonSprite::create("OFF"),
                nullptr
            )
        );

        toggle->setPosition(
            ccp(winSize.width / 2, winSize.height / 2 - 10)
        );

        auto menu = CCMenu::create();
        menu->addChild(toggle);
        menu->setPosition(ccp(0, 0));
        this->addChild(menu);

        auto positionLabel = CCLabelBMFont::create(
            "Display Position",
            "bigFont.fnt"
        );

        positionLabel->setScale(0.45f);
        positionLabel->setPosition(
            ccp(winSize.width / 2, winSize.height / 2 - 55)
        );
        this->addChild(positionLabel);

        auto positionValue = CCLabelBMFont::create(
            "Top Center",
            "bigFont.fnt"
        );

        positionValue->setScale(0.4f);
        positionValue->setPosition(
            ccp(winSize.width / 2, winSize.height / 2 - 85)
        );
        this->addChild(positionValue);

        return true;
    }

public:
    static OffsetSettingsPopup* create() {
        auto ret = new OffsetSettingsPopup();

        if (ret && ret->init(360, 240)) {
            ret->autorelease();
            return ret;
        }

        CC_SAFE_DELETE(ret);
        return nullptr;
    }
};

class $modify(MyPauseLayer, PauseLayer) {
    void customSetup() {
        PauseLayer::customSetup();

        auto logo = CCSprite::create("logo.png");

        if (!logo) {
            log::error("Failed to load logo.png");
            return;
        }

        logo->setScale(0.30f);

        auto button = CCMenuItemSpriteExtra::create(
            logo,
            this,
            menu_selector(MyPauseLayer::onOffsetSettings)
        );

        button->setID("offset-settings-button");

        auto menu = CCMenu::create();
        menu->setID("offset-settings-menu");
        menu->addChild(button);

        auto winSize = CCDirector::sharedDirector()->getWinSize();

        menu->setPosition(
            ccp(
                winSize.width / 2,
                winSize.height / 2 - 100
            )
        );

        this->addChild(menu);
    }

    void onOffsetSettings(CCObject*) {
        OffsetSettingsPopup::create()->show();
    }
};
