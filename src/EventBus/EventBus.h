#pragma once

#include <list>
#include <memory>
#include <typeindex>

#include "../EventBus/Event.h"

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
    EventCallback(TOwner* ownerInstance, CallBackFunction callbackFunction)
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
    template<typename TEvent, typename TOwner>
    void SubscribeEvent(TOwner* ownerInstance, void(TOwner::*callbackFunction)(TEvent&)) {
        auto eventType = std::type_index(typeid(TEvent));
        if(!subscribers[eventType].get()) {
            subscribers[eventType] = std::make_unique<HandlerList>();
        }
        auto subscriber = std::make_unique<EventCallback<TOwner, TEvent>>(ownerInstance, callbackFunction);
        subscribers[eventType]->push_back(std::move(subscriber));
    }

    template<typename TEvent, typename ..TArgs>
    void EmitEvent(TArgs&& ..args) {
        auto handlers = subscribers[std::type_index(typeid(TEvent))];
        if (handlers) {
            for (auto it = handler->begin(); it != handler->end(); it ++) {
                auto handler = it->get();
                TEvent event(std::forward<TArgs>(args)..);
                handler->Execute(event);
            }
        }
    }


private:
    std::map<std::type_index, std::unique_ptr<HandlerList>> subscribers;
};