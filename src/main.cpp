#include <Geode/Geode.hpp>
#include <Geode/modify/UILayer.hpp>
#include <Geode/modify/GJBaseGameLayer.hpp>

#include <chrono>
#include <cmath>

using namespace geode::prelude;
using Clock = std::chrono::steady_clock;

static constexpr double TICK_MS = 1000.0 / 240.0;

static Clock::time_point g_lastTick;
static Clock::time_point g_inputTime;

static bool g_hasTick = false;
static bool g_hasInput = false;

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

        auto winSize = CCDirector::sharedDirector()->getWinSize();

        m_label = CCLabelBMFont::create(
            "0.00 ms",
            "bigFont.fnt"
        );

        if (!m_label)
            return false;

        m_label->setScale(0.45f);
        m_label->setPosition(
            ccp(winSize.width / 2, winSize.height - 30)
        );

        this->addChild(m_label);

        return true;
    }

    void updateOffset(double offset) {
        if (!m_label)
            return;

        m_label->setString(
            fmt::format("{:+.2f} ms", offset).c_str()
            );

        double absOffset = std::abs(offset);

        if (absOffset <= 1.0) {
            m_label->setColor(ccc3(0, 255, 0));
        }
        else if (absOffset <= 2.5) {
            m_label->setColor(ccc3(255, 255, 0));
        }
        else {
            m_label->setColor(ccc3(255, 60, 60));
        }
    }
};

static TimingDisplayLayer* s_display = nullptr;

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

class $modify(MyBaseGameLayer, GJBaseGameLayer) {
    void update(float dt) {
        GJBaseGameLayer::update(dt);

        g_lastTick = Clock::now();
        g_hasTick = true;

        if (g_hasInput && s_display) {
            double offset = std::chrono::duration<double, std::milli>(
                g_inputTime - g_lastTick
            ).count();

            s_display->updateOffset(offset);

            g_hasInput = false;
        }
    }

    void handleButton(bool down, int button, bool player1) {
        if (down) {
            g_inputTime = Clock::now();
            g_hasInput = true;
        }

        GJBaseGameLayer::handleButton(down, button, player1);
    }
};
