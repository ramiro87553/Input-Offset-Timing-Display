#include <Geode/Geode.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include <Geode/modify/UILayer.hpp>

using namespace geode::prelude;

class TimingDisplayLayer : public CCLayer {
protected:
    CCLabelBMFont* m_timingLabel;
    
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
        if (!CCLayer::init()) return false;

        m_timingLabel = CCLabelBMFont::create("Offset: 0.00ms", "bigFont.fnt");
        m_timingLabel->setScale(0.4f);
        m_timingLabel->setPosition(ccp(150, 270));
        m_timingLabel->setOpacity(200);
        this->addChild(m_timingLabel);

        return true;
    }

    void updateTiming(double offsetMs) {
        if (m_timingLabel) {
            std::string text = fmt::format("Offset: {:.2f} ms", offsetMs);
            m_timingLabel->setString(text.c_str());
            
            if (std::abs(offsetMs) < 4.0) {
                m_timingLabel->setColor(ccc3(0, 255, 0));
            } else if (std::abs(offsetMs) < 8.0) {
                m_timingLabel->setColor(ccc3(255, 255, 0));
            } else {
                m_timingLabel->setColor(ccc3(255, 50, 50));
            }
        }
    }
};

static TimingDisplayLayer* s_timingDisplay = nullptr;

class $modify(MyUILayer, UILayer) {
    bool init(GJBaseGameLayer* gameLayer) {
        if (!UILayer::init(gameLayer)) return false;

        s_timingDisplay = TimingDisplayLayer::create();
        this->addChild(s_timingDisplay, 100);

        return true;
    }
};

class $modify(MyPlayLayer, PlayLayer) {
    void update(float dt) {
        PlayLayer::update(dt);

        if (s_timingDisplay) {
            double targetFrameTime = 1.0 / 240.0;
            double currentFrameTime = static_cast<double>(dt);
            double offset = (currentFrameTime - targetFrameTime) * 1000.0;

            s_timingDisplay->updateTiming(offset);
        }
    }
};
