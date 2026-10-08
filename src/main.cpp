#include <Geode/Geode.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include <Geode/binding/PlayerObject.hpp>
#include <Geode/binding/ButtonSprite.hpp>
#include <Geode/binding/CCMenuItemSpriteExtra.hpp>

using namespace geode::prelude;

class $modify(AutoClickerPlayLayer, PlayLayer) {
    struct Fields {
        bool enabled = false;
        double timer = 0.0;
        CCMenuItemSpriteExtra* button = nullptr;
    };

    bool init(GJGameLevel* level, bool useReplay, bool dontCreateObjects) {
        if (!PlayLayer::init(level, useReplay, dontCreateObjects))
            return false;

        this->schedule(schedule_selector(
            AutoClickerPlayLayer::autoClickTick
        ));

        this->addAutoClickerButton();
        return true;
    }

    void addAutoClickerButton() {
        auto size = CCDirector::sharedDirector()->getWinSize();

        auto menu = CCMenu::create();
        menu->setID("auto-clicker-menu");
        menu->setPosition({size.width - 75.f, 55.f});
        menu->setZOrder(999);

        auto sprite = ButtonSprite::create(
            "AUTO: OFF",
            "bigFont.fnt",
            "GJ_button_01.png",
            0.8f
        );

        auto button = CCMenuItemSpriteExtra::create(
            sprite,
            this,
            menu_selector(
                AutoClickerPlayLayer::onAutoClicker
            )
        );

        button->setID("auto-clicker-button");

        menu->addChild(button);
        this->addChild(menu);

        m_fields->button = button;
    }

    void onAutoClicker(CCObject* sender) {
        m_fields->enabled = !m_fields->enabled;
        m_fields->timer = 0.0;

        auto button =
            static_cast<CCMenuItemSpriteExtra*>(sender);

        auto sprite =
            static_cast<ButtonSprite*>(button->getNormalImage());

        sprite->setString(
            m_fields->enabled
                ? "AUTO: ON"
                : "AUTO: OFF"
        );

        button->updateSprite();
    }

    void autoClickTick(float dt) {
        if (!m_fields->enabled)
            return;

        auto player = this->m_player1;

        if (!player || player->m_isDead)
            return;

        auto cps =
            Mod::get()->getSettingValue<int64_t>("cps");

        if (cps < 1)
            cps = 1;

        if (cps > 60)
            cps = 60;

        m_fields->timer += dt;

        double interval =
            1.0 / static_cast<double>(cps);

        while (m_fields->timer >= interval) {
            m_fields->timer -= interval;

            player->pushButton(PlayerButton::Jump);
            player->releaseButton(PlayerButton::Jump);
        }
    }

    void onExit() {
        m_fields->enabled = false;
        PlayLayer::onExit();
    }
};
