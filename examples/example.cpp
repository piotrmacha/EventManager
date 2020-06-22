#include "../include/EventManager.h"

#include <cstdio>

struct IntEvent { int Value; };
struct FloatEvent { float Value; };
struct DoubleEvent { double Value; };

// Alias for EventManager with default Container and Mutex
using EventManager = Fay::BaseEventManager<>;

// Observer that subscribes to IntEvent and FloatEvent
struct MyObserver : Fay::Observer<IntEvent, FloatEvent>
{
    void OnEvent(const IntEvent& event) override
    {
        printf("MyObserver::OnEvent(IntEvent): %d\n", event.Value);
    }

    void OnEvent(const FloatEvent& event) override
    {
        printf("MyObserver::OnEvent(FloatEvent): %.2f\n", event.Value);
    }
};

int main()
{
    MyObserver observer;

    EventManager eventManager;
    eventManager.Subscribe(&observer);

    // Dispatched to MyObserver
    eventManager.Dispatch(IntEvent{ 1 });

    // Dispatched to MyObserver
    eventManager.Dispatch(FloatEvent{ 2.34f });

    // NOT dispatched to MyObserver because it doesn't subscribe to DoubleEvent
    eventManager.Dispatch(DoubleEvent{ 5.67 });

    eventManager.Unsubscribe(&observer);
}