#pragma once

#include <map>
#include <list>
#include <memory>
#include <typeindex>
#include <functional>

#include "../EventBus/Event.h"

#include "../Logger/Logger.h"

class IEventCallback {
public:
    void Execute(Event& e) {
        Call(e);
    }

    virtual ~IEventCallback() = default;
private:
    virtual void Call(Event& e) = 0;
};

template<typename TOwner, typename TEvent>
class EventCallback : public IEventCallback {
private:
    using CallBackFunction = void(TOwner::*)(TEvent&);
public:
    EventCallback(TOwner* ownerInstance, CallBackFunction callBackFunction)
        : ownerInstance(ownerInstance), callBackFunction(callBackFunction)
    {

    }
    
    virtual ~EventCallback() override = default;

private:
    virtual void Call(Event& e) override {
        std::invoke(callBackFunction, ownerInstance, static_cast<TEvent&>(e));
    }

    CallBackFunction callBackFunction;
    TOwner* ownerInstance;
};

using HandlerList = std::list<std::unique_ptr<IEventCallback>>;

class EventBus {
public:
    void Reset() {
        subscribers.clear();
    }

    template<typename TEvent, typename TOwner>
    void SubscribeEvent(TOwner* ownerInstance, void(TOwner::*callbackFunction)(TEvent&)) {
        if(!subscribers[typeid(TEvent)].get()) {
            subscribers[typeid(TEvent)] = std::make_unique<HandlerList>();
        }
        auto subscriber = std::make_unique<EventCallback<TOwner, TEvent>>(ownerInstance, callbackFunction);
        subscribers[typeid(TEvent)]->push_back(std::move(subscriber));
    }

    template<typename TEvent, typename ...TArgs>
    void EmitEvent(TArgs&& ...args) {
        auto& handlers = subscribers[typeid(TEvent)];
        if (handlers) {
            for (auto it = handlers->begin(); it != handlers->end(); it ++) {
                auto handler = it->get();
                TEvent event(std::forward<TArgs>(args)...);
                handler->Execute(event);
            }
        }
    }


private:
    std::map<std::type_index, std::unique_ptr<HandlerList>> subscribers;
};